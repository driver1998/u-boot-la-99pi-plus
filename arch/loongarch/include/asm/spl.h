/* SPDX-License-Identifier: GPL-2.0+ */
#ifndef	__ASM_LA_SPL_H
#define	__ASM_LA_SPL_H

enum {
	BOOT_DEVICE_NONE,
	BOOT_DEVICE_RAM,
	BOOT_DEVICE_MMC1,
	BOOT_DEVICE_MMC2,
	BOOT_DEVICE_MMC2_2,
	BOOT_DEVICE_NAND,
	BOOT_DEVICE_ONENAND,
	BOOT_DEVICE_NOR,
	BOOT_DEVICE_UART,
	BOOT_DEVICE_SPI,
	BOOT_DEVICE_USB,
	BOOT_DEVICE_SATA,
	BOOT_DEVICE_I2C,
	BOOT_DEVICE_BOARD,
	BOOT_DEVICE_DFU,
	BOOT_DEVICE_XIP,
	BOOT_DEVICE_BOOTROM,
};

#ifndef CONFIG_DM
extern gd_t gdata;
#endif

#endif /* __ASM_LA_SPL_H */
