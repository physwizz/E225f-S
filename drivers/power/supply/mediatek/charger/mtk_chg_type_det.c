/*
 * Copyright (C) 2016 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 */

#include <generated/autoconf.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/device.h>
#include <linux/pm_wakeup.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/proc_fs.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/writeback.h>
#include <linux/seq_file.h>
#include <linux/power_supply.h>
#include <linux/time.h>
#include <linux/uaccess.h>
#include <linux/reboot.h>
#include <linux/usb_notify.h>

#include <linux/of.h>
#include <linux/extcon.h>

#include <mt-plat/upmu_common.h>
#include <mach/upmu_sw.h>
#include <mach/upmu_hw.h>
#include <mt-plat/mtk_boot.h>
#include <mt-plat/charger_type.h>
#include <mt-plat/mtk_charger.h>
#include <pmic.h>
#include <tcpm.h>
#include <tcpci_core.h>

#include "mtk_charger_intf.h"
#if defined(CONFIG_AFC_CHARGER)
#include <mt-plat/afc_charger.h>
#endif

#ifdef CONFIG_BATTERY_SAMSUNG
#ifdef CONFIG_PDIC_NOTIFIER
#include <linux/usb/typec/common/pdic_notifier.h>
#endif
#if defined(CONFIG_BATTERY_NOTIFIER)
#include <linux/battery/battery_notifier.h>
#else
#include <linux/battery/sec_pd.h>
#endif
#include <../drivers/battery/common/sec_charging_common.h>

extern struct pdic_notifier_struct pd_noti;
#endif

#ifdef CONFIG_EXTCON_USB_CHG
struct usb_extcon_info {
	struct device *dev;
	struct extcon_dev *edev;

	unsigned int vbus_state;
	unsigned long debounce_jiffies;
	struct delayed_work wq_detcable;
};

static const unsigned int usb_extcon_cable[] = {
	EXTCON_USB,
	EXTCON_USB_HOST,
	EXTCON_NONE,
};
#endif

void __attribute__((weak)) fg_charger_in_handler(void)
{
	pr_notice("%s not defined\n", __func__);
}

struct chg_type_info {
	struct device *dev;
	struct charger_consumer *chg_consumer;
	struct tcpc_device *tcpc;
	struct notifier_block pd_nb;
	bool tcpc_kpoc;
	/* Charger Detection */
	struct mutex chgdet_lock;
	bool chgdet_en;
	atomic_t chgdet_cnt;
	wait_queue_head_t waitq;
	struct task_struct *chgdet_task;
	struct workqueue_struct *pwr_off_wq;
	struct work_struct pwr_off_work;
	struct workqueue_struct *chg_in_wq;
	struct work_struct chg_in_work;
	bool ignore_usb;
	bool plugin;
#ifdef CONFIG_MTK_TYPEC_WATER_DETECT
	bool water_detected;
#endif
	bool bypass_chgdet;
};

#ifdef CONFIG_FPGA_EARLY_PORTING
/*  FPGA */
int hw_charging_get_charger_type(void)
{
	return STANDARD_HOST;
}

#else

/* EVB / Phone */
static const char * const mtk_chg_type_name[] = {
	"Charger Unknown",
	"Standard USB Host",
	"Charging USB Host",
	"Non-standard Charger",
	"Standard Charger",
	"Apple 2.4A Charger",
	"Apple 2.1A Charger",
	"Apple 1.0A Charger",
	"Apple 0.5A Charger",
	"Samsung Charger",
	"Wireless Charger",
};

static void dump_charger_name(enum charger_type type)
{
	switch (type) {
	case CHARGER_UNKNOWN:
	case STANDARD_HOST:
	case CHARGING_HOST:
	case NONSTANDARD_CHARGER:
	case STANDARD_CHARGER:
	case APPLE_2_4A_CHARGER:
	case APPLE_2_1A_CHARGER:
	case APPLE_1_0A_CHARGER:
	case APPLE_0_5A_CHARGER:
	case SAMSUNG_CHARGER:
		pr_info("%s: charger type: %d, %s\n", __func__, type,
			mtk_chg_type_name[type]);
		break;
	default:
		pr_info("%s: charger type: %d, Not Defined!!!\n", __func__,
			type);
		break;
	}
}

/* Power Supply */
struct mt_charger {
	struct device *dev;
	struct power_supply_desc chg_desc;
	struct power_supply_config chg_cfg;
	struct power_supply *chg_psy;
	struct power_supply_desc ac_desc;
	struct power_supply_config ac_cfg;
	struct power_supply *ac_psy;
	struct power_supply_desc usb_desc;
	struct power_supply_config usb_cfg;
	struct power_supply *usb_psy;
	struct chg_type_info *cti;
	#ifdef CONFIG_EXTCON_USB_CHG
	struct usb_extcon_info *extcon_info;
	struct delayed_work extcon_work;
	#endif
	bool chg_online; /* Has charger in or not */
	enum charger_type chg_type;
};

static int mt_charger_online(struct mt_charger *mtk_chg)
{
#if defined(CONFIG_BATTERY_SAMSUNG)
	return 0;
#else
	int ret = 0;
	int boot_mode = 0;
#ifdef CONFIG_KPOC_GET_SOURCE_CAP_TRY
	struct chg_type_info *cti = mtk_chg->cti;
#endif

	if (!mtk_chg->chg_online) {
		boot_mode = get_boot_mode();
		if (boot_mode == KERNEL_POWER_OFF_CHARGING_BOOT ||
		    boot_mode == LOW_POWER_OFF_CHARGING_BOOT) {
			pr_notice("%s: Unplug Charger/USB\n", __func__);
#ifdef CONFIG_KPOC_GET_SOURCE_CAP_TRY
			pr_info("%s error_recovery_once = %d\n", __func__,
					cti->tcpc->pd_port.error_recovery_once);
			if (cti->tcpc->pd_port.error_recovery_once != 1) {
#endif /*CONFIG_KPOC_GET_SOURCE_CAP_TRY*/
				pr_notice("%s: system_state=%d\n", __func__,
					system_state);
				if (system_state != SYSTEM_POWER_OFF)
					kernel_power_off();
#ifdef CONFIG_KPOC_GET_SOURCE_CAP_TRY
			}
#endif /*CONFIG_KPOC_GET_SOURCE_CAP_TRY*/
		}
	}

	return ret;
#endif
}

/* Power Supply Functions */
static int mt_charger_get_property(struct power_supply *psy,
	enum power_supply_property psp, union power_supply_propval *val)
{
	struct mt_charger *mtk_chg = power_supply_get_drvdata(psy);

	switch (psp) {
	case POWER_SUPPLY_PROP_ONLINE:
		val->intval = 0;
		/* Force to 1 in all charger type */
		if (mtk_chg->chg_type != CHARGER_UNKNOWN)
			val->intval = 1;
		break;
	case POWER_SUPPLY_PROP_CHARGE_TYPE:
		val->intval = mtk_chg->chg_type;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

#ifdef CONFIG_EXTCON_USB_CHG
static void usb_extcon_detect_cable(struct work_struct *work)
{
	struct usb_extcon_info *info = container_of(to_delayed_work(work),
						struct usb_extcon_info,
						wq_detcable);

	/* check and update cable state */
	if (info->vbus_state)
		extcon_set_state_sync(info->edev, EXTCON_USB, true);
	else
		extcon_set_state_sync(info->edev, EXTCON_USB, false);
}
#endif

static int mt_charger_set_property(struct power_supply *psy,
	enum power_supply_property psp, const union power_supply_propval *val)
{
	struct mt_charger *mtk_chg = power_supply_get_drvdata(psy);
#ifdef CONFIG_BATTERY_SAMSUNG
	int ret = 0;
	int cable_type = 0;
	union power_supply_propval propval = {0, };
#if defined(CONFIG_PDIC_NOTIFIER)
	union power_supply_propval value;
#endif
#endif
	struct chg_type_info *cti = NULL;
	#ifdef CONFIG_EXTCON_USB_CHG
	struct usb_extcon_info *info;
	#endif

	pr_info("%s\n", __func__);

	if (!mtk_chg) {
		pr_notice("%s: no mtk chg data\n", __func__);
		return -EINVAL;
	}

#ifdef CONFIG_EXTCON_USB_CHG
	info = mtk_chg->extcon_info;
#endif

	cti = mtk_chg->cti;
	switch (psp) {
	case POWER_SUPPLY_PROP_ONLINE:
		mtk_chg->chg_online = val->intval;
		mt_charger_online(mtk_chg);
		return 0;
	case POWER_SUPPLY_PROP_CHARGE_TYPE:
		mtk_chg->chg_type = val->intval;
#if !defined(CONFIG_BATTERY_SAMSUNG)
		if (mtk_chg->chg_type != CHARGER_UNKNOWN)
			charger_manager_force_disable_power_path(
				cti->chg_consumer, MAIN_CHARGER, false);
		else if (!cti->tcpc_kpoc)
			charger_manager_force_disable_power_path(
				cti->chg_consumer, MAIN_CHARGER, true);
#endif
		break;
	default:
		return -EINVAL;
	}

	dump_charger_name(mtk_chg->chg_type);

	if (!cti->ignore_usb) {
		if (get_boot_mode() == KERNEL_POWER_OFF_CHARGING_BOOT ||
		    get_boot_mode() == LOW_POWER_OFF_CHARGING_BOOT) {
			/* do nothing */
			pr_notice("%s: do nothing in KPOC\n", __func__);
		} else {
			/* usb */
			if ((mtk_chg->chg_type == STANDARD_HOST) ||
				(mtk_chg->chg_type == CHARGING_HOST)) {
				struct otg_notify *o_notify = get_otg_notify();

				send_otg_notify(o_notify, NOTIFY_EVENT_USB_CABLE, 1);
				mt_usb_connect();
			} else if (mtk_chg->chg_type == NONSTANDARD_CHARGER) {
				mt_usb_connect();
#ifdef CONFIG_EXTCON_USB_CHG
				info->vbus_state = 1;
#endif
			} else {
				struct otg_notify *o_notify = get_otg_notify();

				send_otg_notify(o_notify, NOTIFY_EVENT_USB_CABLE, 0);
				mt_usb_disconnect();
#ifdef CONFIG_EXTCON_USB_CHG
				info->vbus_state = 0;
#endif
			}
		}
	}

	queue_work(cti->chg_in_wq, &cti->chg_in_work);
	#ifdef CONFIG_EXTCON_USB_CHG
	if (!IS_ERR(info->edev))
		queue_delayed_work(system_power_efficient_wq,
			&info->wq_detcable, info->debounce_jiffies);
	#endif

#if defined(CONFIG_BATTERY_SAMSUNG)
	if (psp == POWER_SUPPLY_PROP_CHARGE_TYPE) {
		psy = power_supply_get_by_name("battery");
		if (!psy) {
			pr_err("%s: Fail to get psy (battery)\n",
				__func__);
		} else {
			switch (mtk_chg->chg_type) {
			case CHARGING_HOST:
				cable_type = SEC_BATTERY_CABLE_USB_CDP;
				break;
			case NONSTANDARD_CHARGER:
				cable_type = SEC_BATTERY_CABLE_TIMEOUT;
				break;
			case STANDARD_HOST:
			case APPLE_0_5A_CHARGER:
				cable_type = SEC_BATTERY_CABLE_USB;
				break;
			case STANDARD_CHARGER:
			case SAMSUNG_CHARGER:
				cable_type = SEC_BATTERY_CABLE_TA;
				break;
			case APPLE_2_4A_CHARGER:
			case APPLE_2_1A_CHARGER:
			case APPLE_1_0A_CHARGER:
				cable_type = SEC_BATTERY_CABLE_TA;
				break;
			default:
				cable_type = SEC_BATTERY_CABLE_NONE;
			}
#if defined(CONFIG_PDIC_NOTIFIER)
			if ((tcpm_inquire_pd_connected(cti->tcpc)) &&
				(cable_type != SEC_BATTERY_CABLE_NONE)) {
				pr_info("%s: set SRCCAP\n", __func__);
				value.intval = 1;
				psy_do_property("battery", set,
						POWER_SUPPLY_EXT_PROP_SRCCAP, value);
			}
#endif
			pr_info("%s: battery ONLINE with: %d\n",
				__func__, cable_type);
			propval.intval = cable_type;
			ret = power_supply_set_property(psy,
				POWER_SUPPLY_PROP_ONLINE, &propval);
			if (ret < 0)
				pr_err("%s: psy online fail(%d)\n",
					__func__, ret);
		}
	}
#else
	power_supply_changed(mtk_chg->ac_psy);
	power_supply_changed(mtk_chg->usb_psy);
#endif

	return 0;
}

static int mt_ac_get_property(struct power_supply *psy,
	enum power_supply_property psp, union power_supply_propval *val)
{
	struct mt_charger *mtk_chg = power_supply_get_drvdata(psy);

	switch (psp) {
	case POWER_SUPPLY_PROP_ONLINE:
		val->intval = 0;
		/* Force to 1 in all charger type */
		if (mtk_chg->chg_type != CHARGER_UNKNOWN)
			val->intval = 1;
		/* Reset to 0 if charger type is USB */
		if ((mtk_chg->chg_type == STANDARD_HOST) ||
			(mtk_chg->chg_type == CHARGING_HOST))
			val->intval = 0;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int mt_usb_get_property(struct power_supply *psy,
	enum power_supply_property psp, union power_supply_propval *val)
{
	struct mt_charger *mtk_chg = power_supply_get_drvdata(psy);

	switch (psp) {
	case POWER_SUPPLY_PROP_ONLINE:
		if ((mtk_chg->chg_type == STANDARD_HOST) ||
			(mtk_chg->chg_type == CHARGING_HOST))
			val->intval = 1;
		else
			val->intval = 0;
		break;
	case POWER_SUPPLY_PROP_CURRENT_MAX:
		val->intval = 500000;
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_MAX:
		val->intval = 5000000;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static enum power_supply_property mt_charger_properties[] = {
	POWER_SUPPLY_PROP_ONLINE,
};

static enum power_supply_property mt_ac_properties[] = {
	POWER_SUPPLY_PROP_ONLINE,
};

static enum power_supply_property mt_usb_properties[] = {
	POWER_SUPPLY_PROP_ONLINE,
	POWER_SUPPLY_PROP_CURRENT_MAX,
	POWER_SUPPLY_PROP_VOLTAGE_MAX,
};

static void tcpc_power_off_work_handler(struct work_struct *work)
{
	pr_info("%s\n", __func__);
#if !defined(CONFIG_BATTERY_SAMSUNG)
	kernel_power_off();
#endif
}

static void charger_in_work_handler(struct work_struct *work)
{
	mtk_charger_int_handler();
	fg_charger_in_handler();
}

#ifdef CONFIG_TCPC_CLASS
static void plug_in_out_handler(struct chg_type_info *cti, bool en, bool ignore)
{
#ifdef CONFIG_MTK_TYPEC_WATER_DETECT
	if (get_boot_mode() == KERNEL_POWER_OFF_CHARGING_BOOT ||
	    get_boot_mode() == LOW_POWER_OFF_CHARGING_BOOT) {
		if (cti->water_detected) {
			pr_info("%s: water detected in KPOC, bypass bc1.2\n",
				__func__);
			return;
		}
	}
#endif
	mutex_lock(&cti->chgdet_lock);
	if (cti->chgdet_en == en)
		goto skip;
	cti->chgdet_en = en;
	cti->ignore_usb = ignore;
	cti->plugin = en;
	atomic_inc(&cti->chgdet_cnt);
	wake_up_interruptible(&cti->waitq);
skip:
	mutex_unlock(&cti->chgdet_lock);
}

static int pd_tcp_notifier_call(struct notifier_block *pnb,
				unsigned long event, void *data)
{
	struct tcp_notify *noti = data;
	struct chg_type_info *cti = container_of(pnb,
		struct chg_type_info, pd_nb);
	int vbus = 0;

	switch (event) {
	case TCP_NOTIFY_SINK_VBUS:
		if (tcpm_inquire_typec_attach_state(cti->tcpc) ==
						   TYPEC_ATTACHED_AUDIO)
			plug_in_out_handler(cti, !!noti->vbus_state.mv, true);
		break;
	case TCP_NOTIFY_TYPEC_STATE:
		if (noti->typec_state.old_state == TYPEC_UNATTACHED &&
		    (noti->typec_state.new_state == TYPEC_ATTACHED_SNK ||
		    noti->typec_state.new_state == TYPEC_ATTACHED_CUSTOM_SRC ||
		    noti->typec_state.new_state == TYPEC_ATTACHED_NORP_SRC)) {
			pr_info("%s USB Plug in, pol = %d\n", __func__,
					noti->typec_state.polarity);
			plug_in_out_handler(cti, true, false);
		} else if ((noti->typec_state.old_state == TYPEC_ATTACHED_SNK ||
		    noti->typec_state.old_state == TYPEC_ATTACHED_CUSTOM_SRC ||
		    noti->typec_state.old_state == TYPEC_ATTACHED_NORP_SRC ||
		    noti->typec_state.old_state == TYPEC_ATTACHED_AUDIO)
			&& noti->typec_state.new_state == TYPEC_UNATTACHED) {
#if defined(CONFIG_BATTERY_SAMSUNG)
			pr_info("%s USB Plug out\n", __func__);
			plug_in_out_handler(cti, false, false);
#endif
			if (cti->tcpc_kpoc) {
#ifdef CONFIG_KPOC_GET_SOURCE_CAP_TRY
				pr_info("%s error_recovery_once = %d\n", __func__,
						cti->tcpc->pd_port.error_recovery_once);
				if (cti->tcpc->pd_port.error_recovery_once == 1) {
					pr_info("%s KPOC error recovery once\n",
					__func__);
					plug_in_out_handler(cti, false, false);
					break;
				}
#endif /*CONFIG_KPOC_GET_SOURCE_CAP_TRY*/
				vbus = battery_get_vbus();
				pr_info("%s KPOC Plug out, vbus = %d\n",
					__func__, vbus);
				queue_work_on(cpumask_first(cpu_online_mask),
					      cti->pwr_off_wq,
					      &cti->pwr_off_work);
				break;
			}
#if ! defined(CONFIG_BATTERY_SAMSUNG)
			pr_info("%s USB Plug out\n", __func__);
			plug_in_out_handler(cti, false, false);
#endif
		} else if (noti->typec_state.old_state == TYPEC_ATTACHED_SRC &&
			noti->typec_state.new_state == TYPEC_ATTACHED_SNK) {
			pr_info("%s Source_to_Sink\n", __func__);
			plug_in_out_handler(cti, true, true);
		}  else if (noti->typec_state.old_state == TYPEC_ATTACHED_SNK &&
			noti->typec_state.new_state == TYPEC_ATTACHED_SRC) {
#if defined(CONFIG_BATTERY_SAMSUNG) && defined(CONFIG_PDIC_NOTIFIER)
			PD_NOTI_TYPEDEF pdic_noti;
#endif
			pr_info("%s Sink_to_Source\n", __func__);
#if defined(CONFIG_BATTERY_SAMSUNG) && defined(CONFIG_PDIC_NOTIFIER)
			pdic_noti.src = PDIC_NOTIFY_DEV_PDIC;
			pdic_noti.dest = PDIC_NOTIFY_DEV_BATT;
			pdic_noti.id = PDIC_NOTIFY_ID_POWER_STATUS;
			pdic_noti.sub1 = 0;
			pdic_noti.sub2 = 0;
			pdic_noti.sub3 = 0;
			pd_noti.sink_status.current_pdo_num = 0;
			pd_noti.sink_status.selected_pdo_num = 0;
			pd_noti.sink_status.available_pdo_num = 0;
			pd_noti.event = PDIC_NOTIFY_EVENT_PD_PRSWAP_SNKTOSRC;
			pdic_notifier_notify((PD_NOTI_TYPEDEF *)&pdic_noti, &pd_noti, 0);
#endif
			plug_in_out_handler(cti, false, true);
		}
		break;
#ifdef CONFIG_MTK_TYPEC_WATER_DETECT
	case TCP_NOTIFY_WD_STATUS:
		if (noti->wd_status.water_detected)
			cti->water_detected = true;
		else
			cti->water_detected = false;
		break;
#endif
	}
	return NOTIFY_OK;
}
#endif

static int chgdet_task_threadfn(void *data)
{
	struct chg_type_info *cti = data;
	bool attach = false, ignore_usb = false;
	int ret = 0;
	struct power_supply *psy = power_supply_get_by_name("charger");
	union power_supply_propval val = {.intval = 0};

	if (!psy) {
		pr_notice("%s: power supply get fail\n", __func__);
		return -ENODEV;
	}

	pr_info("%s: ++\n", __func__);
	while (!kthread_should_stop()) {
		ret = wait_event_interruptible(cti->waitq,
					     atomic_read(&cti->chgdet_cnt) > 0);
		if (ret < 0) {
			pr_info("%s: wait event been interrupted(%d)\n",
				__func__, ret);
			continue;
		}

		pm_stay_awake(cti->dev);
		mutex_lock(&cti->chgdet_lock);
		atomic_set(&cti->chgdet_cnt, 0);
		attach = cti->chgdet_en;
		ignore_usb = cti->ignore_usb;
		mutex_unlock(&cti->chgdet_lock);

		if (attach && ignore_usb) {
			cti->bypass_chgdet = true;
			goto bypass_chgdet;
		} else if (!attach && cti->bypass_chgdet) {
			cti->bypass_chgdet = false;
			goto bypass_chgdet;
		}

#ifdef CONFIG_MTK_EXTERNAL_CHARGER_TYPE_DETECT
		if (cti->chg_consumer)
			charger_manager_enable_chg_type_det(cti->chg_consumer,
							attach);
#else
		mtk_pmic_enable_chr_type_det(attach);
#endif
		goto pm_relax;
bypass_chgdet:
		val.intval = attach;
		ret = power_supply_set_property(psy, POWER_SUPPLY_PROP_ONLINE,
						&val);
		if (ret < 0)
			pr_notice("%s: power supply set online fail(%d)\n",
				  __func__, ret);
		if (tcpm_inquire_typec_attach_state(cti->tcpc) ==
						   TYPEC_ATTACHED_AUDIO)
			val.intval = attach ? NONSTANDARD_CHARGER :
					      CHARGER_UNKNOWN;
		else
			val.intval = attach ? STANDARD_HOST : CHARGER_UNKNOWN;
		ret = power_supply_set_property(psy,
						POWER_SUPPLY_PROP_CHARGE_TYPE,
						&val);
		if (ret < 0)
			pr_notice("%s: power supply set charge type fail(%d)\n",
				  __func__, ret);
pm_relax:
		pm_relax(cti->dev);
	}
	pr_info("%s: --\n", __func__);
	return 0;
}

#ifdef CONFIG_EXTCON_USB_CHG
static void init_extcon_work(struct work_struct *work)
{
	struct delayed_work *dw = to_delayed_work(work);
	struct mt_charger *mt_chg =
		container_of(dw, struct mt_charger, extcon_work);
	struct device_node *node = mt_chg->dev->of_node;
	struct usb_extcon_info *info;

	info = mt_chg->extcon_info;
	if (!info)
		return;

	if (of_property_read_bool(node, "extcon")) {
		info->edev = extcon_get_edev_by_phandle(mt_chg->dev, 0);
		if (IS_ERR(info->edev)) {
			schedule_delayed_work(&mt_chg->extcon_work,
				msecs_to_jiffies(50));
			return;
		}
	}

	INIT_DELAYED_WORK(&info->wq_detcable, usb_extcon_detect_cable);
}
#endif

static int mt_charger_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct chg_type_info *cti = NULL;
	struct mt_charger *mt_chg = NULL;
	#ifdef CONFIG_EXTCON_USB_CHG
	struct usb_extcon_info *info;
	#endif

	pr_info("%s\n", __func__);

	mt_chg = devm_kzalloc(&pdev->dev, sizeof(*mt_chg), GFP_KERNEL);
	if (!mt_chg)
		return -ENOMEM;

	mt_chg->dev = &pdev->dev;
	mt_chg->chg_online = false;
	mt_chg->chg_type = CHARGER_UNKNOWN;

	mt_chg->chg_desc.name = "charger";
	mt_chg->chg_desc.type = POWER_SUPPLY_TYPE_UNKNOWN;
	mt_chg->chg_desc.properties = mt_charger_properties;
	mt_chg->chg_desc.num_properties = ARRAY_SIZE(mt_charger_properties);
	mt_chg->chg_desc.set_property = mt_charger_set_property;
	mt_chg->chg_desc.get_property = mt_charger_get_property;
	mt_chg->chg_cfg.drv_data = mt_chg;

#if defined(CONFIG_BATTERY_SAMSUNG)
	mt_chg->ac_desc.name = "mtk-ac";
	mt_chg->ac_desc.type = POWER_SUPPLY_TYPE_UNKNOWN;
#else
	mt_chg->ac_desc.name = "ac";
	mt_chg->ac_desc.type = POWER_SUPPLY_TYPE_MAINS;
#endif
	mt_chg->ac_desc.properties = mt_ac_properties;
	mt_chg->ac_desc.num_properties = ARRAY_SIZE(mt_ac_properties);
	mt_chg->ac_desc.get_property = mt_ac_get_property;
	mt_chg->ac_cfg.drv_data = mt_chg;

#if defined(CONFIG_BATTERY_SAMSUNG)
	mt_chg->usb_desc.name = "mtk-usb";
	mt_chg->usb_desc.type = POWER_SUPPLY_TYPE_UNKNOWN;
#else
	mt_chg->usb_desc.name = "usb";
	mt_chg->usb_desc.type = POWER_SUPPLY_TYPE_USB;
#endif
	mt_chg->usb_desc.properties = mt_usb_properties;
	mt_chg->usb_desc.num_properties = ARRAY_SIZE(mt_usb_properties);
	mt_chg->usb_desc.get_property = mt_usb_get_property;
	mt_chg->usb_cfg.drv_data = mt_chg;

	mt_chg->chg_psy = power_supply_register(&pdev->dev,
		&mt_chg->chg_desc, &mt_chg->chg_cfg);
	if (IS_ERR(mt_chg->chg_psy)) {
		dev_notice(&pdev->dev, "Failed to register power supply: %ld\n",
			PTR_ERR(mt_chg->chg_psy));
		ret = PTR_ERR(mt_chg->chg_psy);
		return ret;
	}

	mt_chg->ac_psy = power_supply_register(&pdev->dev, &mt_chg->ac_desc,
		&mt_chg->ac_cfg);
	if (IS_ERR(mt_chg->ac_psy)) {
		dev_notice(&pdev->dev, "Failed to register power supply: %ld\n",
			PTR_ERR(mt_chg->ac_psy));
		ret = PTR_ERR(mt_chg->ac_psy);
		goto err_ac_psy;
	}

	mt_chg->usb_psy = power_supply_register(&pdev->dev, &mt_chg->usb_desc,
		&mt_chg->usb_cfg);
	if (IS_ERR(mt_chg->usb_psy)) {
		dev_notice(&pdev->dev, "Failed to register power supply: %ld\n",
			PTR_ERR(mt_chg->usb_psy));
		ret = PTR_ERR(mt_chg->usb_psy);
		goto err_usb_psy;
	}

	cti = devm_kzalloc(&pdev->dev, sizeof(*cti), GFP_KERNEL);
	if (!cti) {
		ret = -ENOMEM;
		goto err_no_mem;
	}
	cti->dev = &pdev->dev;

	cti->chg_consumer = charger_manager_get_by_name(cti->dev,
							"charger_port1");
	if (!cti->chg_consumer) {
		pr_info("%s: get charger consumer device failed\n", __func__);
		ret = -EINVAL;
		goto err_get_tcpc_dev;
	}

	ret = get_boot_mode();
	if (ret == KERNEL_POWER_OFF_CHARGING_BOOT ||
	    ret == LOW_POWER_OFF_CHARGING_BOOT)
		cti->tcpc_kpoc = true;
	pr_info("%s KPOC(%d)\n", __func__, cti->tcpc_kpoc);

	/* Init Charger Detection */
	mutex_init(&cti->chgdet_lock);
	atomic_set(&cti->chgdet_cnt, 0);

	init_waitqueue_head(&cti->waitq);
	cti->chgdet_task = kthread_run(
				chgdet_task_threadfn, cti, "chgdet_thread");
	ret = PTR_ERR_OR_ZERO(cti->chgdet_task);
	if (ret < 0) {
		pr_info("%s: create chg det work fail\n", __func__);
		return ret;
	}

	/* Init power off work */
	cti->pwr_off_wq = create_singlethread_workqueue("tcpc_power_off");
	INIT_WORK(&cti->pwr_off_work, tcpc_power_off_work_handler);

	cti->chg_in_wq = create_singlethread_workqueue("charger_in");
	INIT_WORK(&cti->chg_in_work, charger_in_work_handler);

	mt_chg->cti = cti;
	platform_set_drvdata(pdev, mt_chg);
	device_init_wakeup(&pdev->dev, true);

	#ifdef CONFIG_EXTCON_USB_CHG
	info = devm_kzalloc(mt_chg->dev, sizeof(*info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	info->dev = mt_chg->dev;
	mt_chg->extcon_info = info;

	INIT_DELAYED_WORK(&mt_chg->extcon_work, init_extcon_work);
	schedule_delayed_work(&mt_chg->extcon_work, 0);
	#endif

	pr_info("%s done\n", __func__);
	return 0;

err_get_tcpc_dev:
	devm_kfree(&pdev->dev, cti);
err_no_mem:
	power_supply_unregister(mt_chg->usb_psy);
err_usb_psy:
	power_supply_unregister(mt_chg->ac_psy);
err_ac_psy:
	power_supply_unregister(mt_chg->chg_psy);
	return ret;
}

static int mt_charger_remove(struct platform_device *pdev)
{
	struct mt_charger *mt_charger = platform_get_drvdata(pdev);
	struct chg_type_info *cti = mt_charger->cti;

	power_supply_unregister(mt_charger->chg_psy);
	power_supply_unregister(mt_charger->ac_psy);
	power_supply_unregister(mt_charger->usb_psy);

	pr_info("%s\n", __func__);
	if (cti->chgdet_task) {
		kthread_stop(cti->chgdet_task);
		atomic_inc(&cti->chgdet_cnt);
		wake_up_interruptible(&cti->waitq);
	}

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int mt_charger_suspend(struct device *dev)
{
	/* struct mt_charger *mt_charger = dev_get_drvdata(dev); */
	return 0;
}

static int mt_charger_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct mt_charger *mt_charger = platform_get_drvdata(pdev);

	if (!mt_charger) {
		pr_info("%s: get mt_charger failed\n", __func__);
		return -ENODEV;
	}

	power_supply_changed(mt_charger->chg_psy);
#if !defined(CONFIG_BATTERY_SAMSUNG)
	power_supply_changed(mt_charger->ac_psy);
	power_supply_changed(mt_charger->usb_psy);
#endif

	return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(mt_charger_pm_ops, mt_charger_suspend,
	mt_charger_resume);

static const struct of_device_id mt_charger_match[] = {
	{ .compatible = "mediatek,mt-charger", },
	{ },
};
static struct platform_driver mt_charger_driver = {
	.probe = mt_charger_probe,
	.remove = mt_charger_remove,
	.driver = {
		.name = "mt-charger-det",
		.owner = THIS_MODULE,
		.pm = &mt_charger_pm_ops,
		.of_match_table = mt_charger_match,
	},
};

/* Legacy api to prevent build error */
bool upmu_is_chr_det(void)
{
	struct mt_charger *mtk_chg = NULL;
	struct power_supply *psy = power_supply_get_by_name("charger");

	if (!psy) {
		pr_info("%s: get power supply failed\n", __func__);
		return -EINVAL;
	}
	mtk_chg = power_supply_get_drvdata(psy);
	return mtk_chg->chg_online;
}

/* Legacy api to prevent build error */
bool pmic_chrdet_status(void)
{
	if (upmu_is_chr_det())
		return true;

	pr_notice("%s: No charger\n", __func__);
	return false;
}

enum charger_type mt_get_charger_type(void)
{
	struct mt_charger *mtk_chg = NULL;
	struct power_supply *psy = power_supply_get_by_name("charger");

	if (!psy) {
		pr_info("%s: get power supply failed\n", __func__);
		return -EINVAL;
	}
	mtk_chg = power_supply_get_drvdata(psy);
	return mtk_chg->chg_type;
}

bool mt_charger_plugin(void)
{
	struct mt_charger *mtk_chg = NULL;
	struct power_supply *psy = power_supply_get_by_name("charger");
	struct chg_type_info *cti = NULL;

	if (!psy) {
		pr_info("%s: get power supply failed\n", __func__);
		return -EINVAL;
	}
	mtk_chg = power_supply_get_drvdata(psy);
	cti = mtk_chg->cti;
	pr_info("%s plugin:%d\n", __func__, cti->plugin);

	return cti->plugin;
}

static s32 __init mt_charger_det_init(void)
{
	return platform_driver_register(&mt_charger_driver);
}

static void __exit mt_charger_det_exit(void)
{
	platform_driver_unregister(&mt_charger_driver);
}

subsys_initcall(mt_charger_det_init);
module_exit(mt_charger_det_exit);

#ifdef CONFIG_TCPC_CLASS
static int __init mt_charger_det_notifier_call_init(void)
{
	int ret = 0;
	struct power_supply *psy = power_supply_get_by_name("charger");
	struct mt_charger *mt_chg = NULL;
	struct chg_type_info *cti = NULL;

	if (!psy) {
		pr_notice("%s: get power supply fail\n", __func__);
		return -ENODEV;
	}
	mt_chg = power_supply_get_drvdata(psy);
	cti = mt_chg->cti;

	cti->tcpc = tcpc_dev_get_by_name("type_c_port0");
	if (cti->tcpc == NULL) {
		pr_notice("%s: get tcpc dev fail\n", __func__);
		ret = -ENODEV;
		goto out;
	}
	cti->pd_nb.notifier_call = pd_tcp_notifier_call;
	ret = register_tcp_dev_notifier(cti->tcpc,
		&cti->pd_nb, TCP_NOTIFY_TYPE_ALL);
	if (ret < 0) {
		pr_notice("%s: register tcpc notifier fail(%d)\n",
			  __func__, ret);
		goto out;
	}
	pr_info("%s done\n", __func__);
out:
	power_supply_put(psy);
	return ret;
}
late_initcall(mt_charger_det_notifier_call_init);
#endif

MODULE_DESCRIPTION("mt-charger-detection");
MODULE_AUTHOR("MediaTek");
MODULE_LICENSE("GPL v2");

#endif /* CONFIG_MTK_FPGA */
