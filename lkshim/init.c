#include <kernel/thread.h>
#include <pm8x41.h>

/* Return 1 if vol_down pressed */
uint32_t target_volume_down(void)
{
    uint8_t status = 0;
	struct pm8x41_gpio gpio;

	/* Configure the GPIO */
	gpio.direction = PM_GPIO_DIR_IN;
	gpio.function  = 0;
	gpio.pull      = PM_GPIO_PULL_UP_30;
	gpio.vin_sel   = 2;

	pm8x41_gpio_config(2, &gpio);

	/* Wait for the pmic gpio config to take effect */
	thread_sleep(1);

	/* Get status of P_GPIO_2 */
	pm8x41_gpio_get(2, &status);

	return !status; /* active low */
}
