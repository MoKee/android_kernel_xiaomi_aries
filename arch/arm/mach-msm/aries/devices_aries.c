/* Copyright (c) 2012, LGE Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/memory.h>
#include <linux/persistent_ram.h>

#include <asm/setup.h>
#include <asm/sizes.h>
#include <asm/system_info.h>
#include <asm/memory.h>

#include <mach/board_aries.h>

#include <ram_console.h>

#ifdef CONFIG_ANDROID_PERSISTENT_RAM
static struct persistent_ram_descriptor pram_descs[] = {
#ifdef CONFIG_ANDROID_RAM_CONSOLE
    {
	.name = "ram_console",
	.size = ARIES_RAM_CONSOLE_SIZE,
    },
#endif
};

static struct persistent_ram aries_persistent_ram = {
    .size = ARIES_PERSISTENT_RAM_SIZE,
    .num_descs = ARRAY_SIZE(pram_descs),
    .descs = pram_descs,
};

void __init aries_add_persistent_ram(void)
{
    struct persistent_ram *pram = &aries_persistent_ram;
    struct membank* bank = &meminfo.bank[0];

    pram->start = bank->start + bank->size - ARIES_PERSISTENT_RAM_SIZE;

    persistent_ram_early_init(pram);
}
#endif

void __init aries_reserve(void)
{
    aries_add_persistent_ram();
}

#ifdef CONFIG_ANDROID_RAM_CONSOLE
static char bootreason[128] = {0,};
int __init aries_boot_reason(char *s)
{
	int n;

	if (*s == '=')
		s++;
	n = snprintf(bootreason, sizeof(bootreason),
		 "Boot info:\n"
		 "Last boot reason: %s\n", s);
	bootreason[n] = '\0';
	return 1;
}
__setup("bootreason", aries_boot_reason);

struct ram_console_platform_data ram_console_pdata = {
        .bootinfo = bootreason,
};

static struct platform_device ram_console_device = {
    .name = "ram_console",
    .id = -1,
    .dev = {
	.platform_data = &ram_console_pdata,
    }
};

void __init aries_add_ramconsole_devices(void)
{
    platform_device_register(&ram_console_device);
}
#endif /* CONFIG_ANDROID_RAM_CONSOLE */
