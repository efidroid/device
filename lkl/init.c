#include <linux/binfmts.h>
#include <linux/init.h>
#include <linux/init_task.h>
#include <linux/personality.h>
#include <linux/reboot.h>
#include <linux/fs.h>
#include <linux/start_kernel.h>
#include <linux/syscalls.h>
#include <linux/tick.h>
#include <asm/host_ops.h>
#include <asm/irq.h>
#include <asm/unistd.h>
#include <asm/syscalls.h>
#include <asm/cpu.h>
#include <linux/firmware.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <uapi/linux/input.h>
#include <linux/platform_device.h>
#include <linux/device/rmi.h>
#include <linux/device/irq_gpio_tlmm.h>

/* touch screen device */
#define APQ8064_GSBI3_QUP_I2C_BUS_ID            3

int rmi_gpio_config(void *gpio_data, bool configure);

/* add built-in firmware to the kernel */
#include "firmware/rmi_firmware_tpk_ito_aries_p1.h"
#include "firmware/rmi_firmware_tpk_ito_aries_p2.h"
#include "firmware/rmi_firmware_biel_ito_aries.h"
#include "firmware/rmi_firmware_laibao_ito_aries.h"
#include "firmware/rmi_factory_test_data_tpk_aries_p1.h"
#include "firmware/rmi_factory_test_data_tpk_aries_p2.h"
#include "firmware/rmi_factory_test_data_biel_aries.h"
#include "firmware/rmi_factory_test_data_laibao_aries.h"
#define TPK_FIRMWARE_P1_NAME	"rmi4/s3202_ver5.img"
#define TPK_FIRMWARE_P2_NAME	"rmi4/S3202_ver5.img"
#define BIEL_FIRMWARE_NAME	"rmi4/BM001.img"
#define LAIBAO_FIRMWARE_NAME	"rmi4/Trs_lb00.img"
DECLARE_BUILTIN_FIRMWARE(TPK_FIRMWARE_P1_NAME, rmi_firmware_tpk_ito_p1);
DECLARE_BUILTIN_FIRMWARE(TPK_FIRMWARE_P2_NAME, rmi_firmware_tpk_ito_p2);
DECLARE_BUILTIN_FIRMWARE(BIEL_FIRMWARE_NAME, rmi_firmware_biel_ito);
DECLARE_BUILTIN_FIRMWARE(LAIBAO_FIRMWARE_NAME, rmi_firmware_laibao_ito);

static unsigned char rmi_key_map[] = {KEY_MENU, KEY_HOME, KEY_BACK};

static struct rmi_button_map rmi_button_map_s3202 = {
	.nbuttons		= 3,
	.map			= rmi_key_map,
};

static struct rmi_f54_self_test_data rmi_self_test_data_tpk_p1[] = {
	{min_max_tpk_p1, sizeof(min_max_tpk_p1)},
	{full_raw_capacitance_tpk_p1, sizeof(full_raw_capacitance_tpk_p1)},
	{high_resistance_tpk_p1, sizeof(high_resistance_tpk_p1)},
};

static struct rmi_f54_self_test_data rmi_self_test_data_tpk_p2[] = {
	{min_max_tpk_p2, sizeof(min_max_tpk_p2)},
	{full_raw_capacitance_tpk_p2, sizeof(full_raw_capacitance_tpk_p2)},
	{high_resistance_tpk_p2, sizeof(high_resistance_tpk_p2)},
};

static struct rmi_f54_self_test_data rmi_self_test_data_biel[] = {
	{min_max_biel, sizeof(min_max_biel)},
	{full_raw_capacitance_biel, sizeof(full_raw_capacitance_biel)},
	{high_resistance_biel, sizeof(high_resistance_biel)},
};

static struct rmi_f54_self_test_data rmi_self_test_data_laibao[] = {
	{min_max_laibao, sizeof(min_max_laibao)},
	{full_raw_capacitance_laibao, sizeof(full_raw_capacitance_laibao)},
	{high_resistance_laibao, sizeof(high_resistance_laibao)},
};

static struct rmi_config_info rmi_config_info_array[] = {
	{
		.product_name = "s3202_ver5",
		.self_test_data = rmi_self_test_data_tpk_p1,
	},
	{
		.product_name = "S3202_ver5",
		.self_test_data = rmi_self_test_data_tpk_p2,
	},
	{
		.product_name = "BM001",
		.self_test_data = rmi_self_test_data_biel,
	},
	{
		.product_name = "Trs_lb00",
		.self_test_data = rmi_self_test_data_laibao,
	},
};

static struct rmi_device_platform_data rmi_data = {
	.driver_name		= "rmi_generic",
	.sensor_name		= "s3202",
    .attn_gpio_irq = 0,
	.attn_polarity		= RMI_ATTN_ACTIVE_LOW,
	.level_triggered	= true,
	.gpio_config		= rmi_gpio_config,
	.axis_align		= {
		.flip_x			= 1,
		.flip_y			= 1,
		.staying_threshold	= 12,
		.landing_threshold	= 24,
		.landing_jiffies	= HZ/8,
	},
	.f11_type_b		= true,
	.reset_delay_ms		= 65,
	.power_management	= {
		.nosleep		= RMI_F01_NOSLEEP_ON,
	},
	.f1a_button_map		= &rmi_button_map_s3202,
	.config_info_array		= rmi_config_info_array,
	.config_array_size		= sizeof(rmi_config_info_array),
};


static struct i2c_board_info touch_device_info[] = {
#if 0
	{
		I2C_BOARD_INFO("atmel_mxt_ts", 0x4b),
		.platform_data = &mxt336s_platform_data,
//		.irq = MSM_GPIO_TO_INT(ATMEL_TS_I2C_INT_GPIO),
	},
#endif
#if 0
	{
		I2C_BOARD_INFO("rmi_i2c", 0x22),
		.platform_data = &rmi_data,
	},
#endif
	{
		I2C_BOARD_INFO("rmi_i2c", 0x20),
		.platform_data = &rmi_data,
	},
#if 0
	{
		I2C_BOARD_INFO("rmi_i2c", 0x70),
		.platform_data = &rmi_data,
	},
#endif
};

#define I2C_FFA  (1 << 1)

struct i2c_registry {
	u8                     machs;
	int                    bus;
	struct i2c_board_info *info;
	int                    len;
};


static struct i2c_registry touch_devices __initdata = {
	I2C_FFA,
	APQ8064_GSBI3_QUP_I2C_BUS_ID,
	touch_device_info,
	ARRAY_SIZE(touch_device_info),
};

void __init apq8064_init_input(void)
{
	printk(KERN_INFO "[Touch D] %s: NOT DCM KDDI, reg touchscreen driver \n",
	       __func__);

    rmi_data.attn_gpio_irq = lkl_gpio_tlmm_get_free_irq("rmi4_attn", 6);

	i2c_register_board_info(touch_devices.bus,
				touch_devices.info,
				touch_devices.len);
}

static int __init customize_machine(void)
{
    apq8064_init_input();

	return 0;
}
arch_initcall(customize_machine);
