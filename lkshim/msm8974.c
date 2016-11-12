#include <pm8x41.h>
#include <platform/timer.h>
#include <dev/newkeys.h>
#include <err.h>

static uint32_t target_camera_snapshot(void)
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

        pm8x41_gpio_config(1, &gpio);

        /* Wait for the pmic gpio config to take effect */
        udelay(10000);

        first_time = 1;
    }

    /* Get status of P_GPIO_5 */
    pm8x41_gpio_get(1, &status);

    return !status; /* active low */
}

static int event_source_poll(newkey_event_source_t *source)
{
    newkeys_set_report_key(source, KEY_BACK, target_camera_snapshot());

    return NO_ERROR;
}

static newkey_event_source_t event_source = {
    .poll = event_source_poll
};

void uefiapi_platform_init_late(void)
{
    newkeys_add_source(&event_source);
}
