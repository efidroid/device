#include <PiDxe.h>
#include <Library/LKEnvLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/QcomPm8921Lib.h>
#include <Library/QcomGpioTlmmLib.h>

#include <lkl.h>
#include <lkl_host.h>

#define	PM_GPIO_VIN_S4			2 /* 1.8v */
#define RMI4_TS_I2C_INT_GPIO		6

#define PM8921_GPIO_INIT(_gpio, _dir, _buf, _val, _pull, _vin, _out_strength, \
			_func, _inv, _disable) \
{ \
	.gpio	= PM_GPIO(_gpio), \
	.config	= { \
		.direction	= _dir, \
		.output_buffer	= _buf, \
		.output_value	= _val, \
		.pull		= _pull, \
		.vin_sel	= _vin, \
		.out_strength	= _out_strength, \
		.function	= _func, \
		.inv_int_pol	= _inv, \
		.disable_pin	= _disable, \
	} \
}

#define PM8921_GPIO_OUTPUT(_gpio, _val, _strength) \
	PM8921_GPIO_INIT(_gpio, PM_GPIO_DIR_OUT, PM_GPIO_OUT_BUF_CMOS, _val, \
			PM_GPIO_PULL_NO, PM_GPIO_VIN_S4, \
			PM_GPIO_STRENGTH_##_strength, \
			PM_GPIO_FUNC_NORMAL, 0, 0)

#define PM8921_GPIO_INPUT(_gpio, _pull) \
	PM8921_GPIO_INIT(_gpio, PM_GPIO_DIR_IN, PM_GPIO_OUT_BUF_CMOS, 0, \
			_pull, PM_GPIO_VIN_S4, \
			PM_GPIO_STRENGTH_NO, \
			PM_GPIO_FUNC_NORMAL, 0, 0)

STATIC struct pm8xxx_gpio_init pm8921_gpios[] = {
	PM8921_GPIO_OUTPUT(5, 0, HIGH),			/* touchscreen power pin */
	PM8921_GPIO_OUTPUT(8, 0, HIGH),			/* touchscreen reset pin */
	PM8921_GPIO_INPUT(12, PM_GPIO_PULL_NO),		/* LCD DET ID */
};

STATIC struct pm8xxx_gpio_init pm8921_power_enable_gpios[] = {
	PM8921_GPIO_OUTPUT(5, 1, HIGH),			/* touchscreen power pin */
};

STATIC struct pm8xxx_gpio_init pm8921_power_disable_gpios[] = {
	PM8921_GPIO_OUTPUT(5, 0, HIGH),			/* touchscreen power pin */
};

int rmi_power_on(BOOLEAN on)
{
    int rc = 0;

    if(on) {
        rc = gPM8921->pm8921_gpio_config_array(pm8921_power_enable_gpios, ARRAY_SIZE(pm8921_power_enable_gpios));
        if(rc) return rc;
    }
    else {
        rc = gPM8921->pm8921_gpio_config_array(pm8921_power_disable_gpios, ARRAY_SIZE(pm8921_power_disable_gpios));
        if(rc) return rc;
    }

	mdelay(100);
	return rc;
}

int rmi_gpio_config(VOID *gpio_data, BOOLEAN configure)
{
	int rc = 0;

	if (configure) {
        rc = gPM8921->pm8921_gpio_config_array(pm8921_gpios, ARRAY_SIZE(pm8921_gpios));
        if(rc) return rc;

		rmi_power_on(TRUE);
	} else {
		rmi_power_on(FALSE);
	}

	return rc;
}

int lkl_rmi_get_attn_status(VOID) {
    return gGpioTlmm->Get(RMI4_TS_I2C_INT_GPIO);
}

RETURN_STATUS
EFIAPI
LKLTouchScreenLibConstructor (
  VOID
  )
{
  EFI_STATUS Status;

  gGpioTlmm->SetFunction(RMI4_TS_I2C_INT_GPIO, 0);
  gGpioTlmm->SetDriveStrength(RMI4_TS_I2C_INT_GPIO, 8);
  gGpioTlmm->SetPull(RMI4_TS_I2C_INT_GPIO, GPIO_PULL_UP);
  gGpioTlmm->DirectionInput(RMI4_TS_I2C_INT_GPIO);

  Status = EFI_SUCCESS;

  return Status;
}
