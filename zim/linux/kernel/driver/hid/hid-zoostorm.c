/*
 * HID driver for zoostorm play 2.4G Wireless Receiver
 */

/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License.
 */

#include <linux/device.h>
#include <linux/input.h>
#include <linux/hid.h>
#include <linux/module.h>

#include "hid-ids.h"

#define unk	KEY_UNKNOWN

/* A general keymap */
static const unsigned char hid_keyboard[256] = {
	  0,  0,  0,  0, 30, 48, 46, 32, 18, 33, 34, 35, 23, 36, 37, 38,
	 50, 49, 24, 25, 16, 19, 31, 20, 22, 47, 17, 45, 21, 44,  2,  3,
	  4,  5,  6,  7,  8,  9, 10, 11, 28,  1, 14, 15, 57, 12, 13, 26,
	 27, 43, 43, 39, 40, 41, 51, 52, 53, 58, 59, 60, 61, 62, 63, 64,
	 65, 66, 67, 68, 87, 88, 99, 70,119,110,102,104,111,107,109,106,
	105,108,103, 69, 98, 55, 74, 78, 96, 79, 80, 81, 75, 76, 77, 71,
	 72, 73, 82, 83, 86,127,116,117,183,184,185,186,187,188,189,190,
	191,192,193,194,134,138,130,132,128,129,131,137,133,135,136,113,
	115,114,unk,unk,unk,121,unk, 89, 93,124, 92, 94, 95,unk,unk,unk,
	122,123, 90, 91, 85,unk,unk,unk,unk,unk,unk,unk,111,unk,unk,unk,
	unk,unk,unk,unk,unk,unk,unk,unk,unk,unk,unk,unk,unk,unk,unk,unk,
	unk,unk,unk,unk,unk,unk,179,180,unk,unk,unk,unk,unk,unk,unk,unk,
	unk,unk,unk,unk,unk,unk,unk,unk,unk,unk,unk,unk,unk,unk,unk,unk,
	unk,unk,unk,unk,unk,unk,unk,unk,111,unk,unk,unk,unk,unk,unk,unk,
	 29, 42, 56,125, 97, 54,100,126,164,166,165,163,161,115,114,113,
	150,158,159,128,136,177,178,176,142,152,173,140,unk,unk,unk,unk
};

#define zs_map_key(c)	hid_map_usage(hi, usage, bit, max, EV_KEY, (c))
#define zs_map_key_clear(c)	hid_map_usage_clear(hi, usage, bit, max, EV_KEY, (c))

/*
 * return:
 * =0: parse usage by generic code
 * >0: skip generic parsing
 * <0: completely ignore this usage
 */
static int zoostorm_input_mapping(struct hid_device *hdev, struct hid_input *hi,
		struct hid_field *field, struct hid_usage *usage,
		unsigned long **bit, int *max)
{
	struct input_dev *input = hi->input;

	switch (usage->hid & HID_USAGE_PAGE) {
	case HID_UP_KEYBOARD:
		set_bit(EV_REP, input->evbit);
		if ((usage->hid & HID_USAGE) < 256) {
			if (!hid_keyboard[usage->hid & HID_USAGE])
				return -1;
			zs_map_key_clear(hid_keyboard[usage->hid & HID_USAGE]);
		} else {
			zs_map_key(KEY_UNKNOWN);
		}
		switch (usage->hid & HID_USAGE) {
		case 0x0028: zs_map_key_clear(KEY_ENTER); break;
		case 0x003a: zs_map_key_clear(KEY_WWW); break;
		case 0x004a: zs_map_key_clear(KEY_HOMEPAGE); break;
		default: return 0;
		}
		break;
	case HID_UP_CONSUMER:
		switch (usage->hid & HID_USAGE) {
		case 0x00b3: zs_map_key_clear(KEY_FASTFORWARD); break;
		case 0x00b4: zs_map_key_clear(KEY_REWIND); break;
		case 0x00b5: zs_map_key_clear(KEY_NEXTSONG); break;
		case 0x00b6: zs_map_key_clear(KEY_PREVIOUSSONG); break;
		case 0x00b7: zs_map_key_clear(KEY_STOPCD); break;
		case 0x00cd: zs_map_key_clear(KEY_PLAYPAUSE); break;
		case 0x00e2: zs_map_key_clear(KEY_MUTE); break;
		case 0x00e9: zs_map_key_clear(KEY_VOLUMEUP); break;
		case 0x00ea: zs_map_key_clear(KEY_VOLUMEDOWN); break;
		case 0x01bd: zs_map_key_clear(KEY_INFO); break;
		case 0x0341: zs_map_key_clear(KEY_SUBTITLE); break;
		case 0x0342: zs_map_key_clear(KEY_AUDIO); break;
		default: return 0;
		}
		break;
	case HID_UP_BUTTON:
		if (field->application == HID_GD_MOUSE) {
			switch (usage->hid & HID_USAGE) {
			case 0x0001: zs_map_key(BTN_MOUSE); break;
			case 0x0002: zs_map_key(BTN_BACK); break;
			case 0x0003: zs_map_key(KEY_MENU); break;
			default: return 0;
			}
		}
		break;
	default:
		return 0;
	}
	return 1;
}

static const struct hid_device_id zoostorm_devices[] = {
	{ HID_USB_DEVICE(USB_VENDOR_ID_ZOOSTORM, USB_DEVICE_ID_ZOOSTORM) },
	{ }
};
MODULE_DEVICE_TABLE(hid, zoostorm_devices);

static struct hid_driver zoostorm_driver = {
	.name = "zoostorm play",
	.id_table = zoostorm_devices,
	.input_mapping = zoostorm_input_mapping,
};

static int __init zoostorm_init(void)
{
	return hid_register_driver(&zoostorm_driver);
}

static void __exit zoostorm_exit(void)
{
	hid_unregister_driver(&zoostorm_driver);
}

module_init(zoostorm_init);
module_exit(zoostorm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("liangxiaoju <liangxiaoju@imtechnology.com.cn>");
MODULE_DESCRIPTION("hid driver for zoostorm play 2.4G Wireless Receiver");

