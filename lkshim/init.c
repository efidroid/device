#include <stdint.h>
#include <platform/gpio.h>
#include <pm8x41.h>
#include <platform/timer.h>

int target_volume_down(void)
{
	static uint8_t first_time = 0;
	uint8_t status = 0;
	struct pm8x41_gpio gpio;

	if (!first_time) {
		/* Configure the GPIO */
		gpio.direction = PM_GPIO_DIR_IN;
		gpio.function  = 0;
		gpio.pull      = PM_GPIO_PULL_UP_30;
		gpio.vin_sel   = 2;

		pm8x41_gpio_config(2, &gpio);

		/* Wait for the pmic gpio config to take effect */
		udelay(10000);

		first_time = 1;
	}

	/* Get status of P_GPIO_5 */
	pm8x41_gpio_get(2, &status);

	return !status; /* active low */
}
