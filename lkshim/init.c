#include <rpm-smd.h>
#include <regulator.h>
#include <platform/timer.h>

// sdhc1: vdd
static uint32_t ldo17[][11]=
{
	{
		LDOA_RES_TYPE, 17,
		KEY_SOFTWARE_ENABLE, 4, GENERIC_DISABLE,
		KEY_MICRO_VOLT, 4, 0,
		KEY_CURRENT, 4, 0,
	},

	{
		LDOA_RES_TYPE, 17,
		KEY_SOFTWARE_ENABLE, 4, GENERIC_ENABLE,
		KEY_MICRO_VOLT, 4, 2900000,
		KEY_CURRENT, 4, 400,
	},
};

// sdhc1: vdd-io
static uint32_t ldo6[][11]=
{
	{
		LDOA_RES_TYPE, 6,
		KEY_SOFTWARE_ENABLE, 4, GENERIC_DISABLE,
		KEY_MICRO_VOLT, 4, 0,
		KEY_CURRENT, 4, 0,
	},

	{
		LDOA_RES_TYPE, 6,
		KEY_SOFTWARE_ENABLE, 4, GENERIC_ENABLE,
		KEY_MICRO_VOLT, 4, 1800000,
		KEY_CURRENT, 4, 60,
	},
};

// sdhc2: vdd
static uint32_t ldo18[][11]=
{
	{
		LDOA_RES_TYPE, 18,
		KEY_SOFTWARE_ENABLE, 4, GENERIC_DISABLE,
		KEY_MICRO_VOLT, 4, 0,
		KEY_CURRENT, 4, 0,
	},

	{
		LDOA_RES_TYPE, 18,
		KEY_SOFTWARE_ENABLE, 4, GENERIC_ENABLE,
		KEY_MICRO_VOLT, 4, 2950000,
		KEY_CURRENT, 4, 400,
	},
};

// sdhc2: vdd-io
static uint32_t ldo21[][11]=
{
	{
		LDOA_RES_TYPE, 21,
		KEY_SOFTWARE_ENABLE, 4, GENERIC_DISABLE,
		KEY_MICRO_VOLT, 4, 0,
		KEY_CURRENT, 4, 0,
	},

	{
		LDOA_RES_TYPE, 21,
		KEY_SOFTWARE_ENABLE, 4, GENERIC_ENABLE,
		KEY_MICRO_VOLT, 4, 2950000,
		KEY_CURRENT, 4, 50,
	},
};

int target_sdc_pre_init_actions(void) {
	// sdhci1: disable
	rpm_send_data(&ldo17[GENERIC_DISABLE][0], 36, RPM_REQUEST_TYPE);
	rpm_send_data(&ldo6[GENERIC_DISABLE][0], 36, RPM_REQUEST_TYPE);
	// sdhci2: disable
	rpm_send_data(&ldo18[GENERIC_DISABLE][0], 36, RPM_REQUEST_TYPE);
	rpm_send_data(&ldo21[GENERIC_DISABLE][0], 36, RPM_REQUEST_TYPE);
	udelay(10000 + 2000);

	// sdhci1: enable
	rpm_send_data(&ldo17[GENERIC_ENABLE][0], 36, RPM_REQUEST_TYPE);
	rpm_send_data(&ldo6[GENERIC_ENABLE][0], 36, RPM_REQUEST_TYPE);
	// sdhci2: enable
	rpm_send_data(&ldo18[GENERIC_ENABLE][0], 36, RPM_REQUEST_TYPE);
	rpm_send_data(&ldo21[GENERIC_ENABLE][0], 36, RPM_REQUEST_TYPE);
	udelay(10000 + 2000);

    return 0;
}
