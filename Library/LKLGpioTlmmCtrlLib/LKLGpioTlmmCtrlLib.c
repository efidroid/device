#include <PiDxe.h>
#include <Library/LKEnvLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/QcomGpioTlmmLib.h>
#include <Protocol/QcomGpioTlmmInterrupt.h>
#include <lkl.h>
#include <lkl_host.h>
#include <linux/device/gpio_lkl.h>

static int tlmm_gpio_get_direction(void *user) {
  int gpio = (int)user;
  return gGpioTlmm->GetDirection(gpio);
}

static int tlmm_gpio_direction_input(void *user) {
  int gpio = (int)user;
  return gGpioTlmm->DirectionInput(gpio);
}

static int tlmm_gpio_direction_output(void *user, int value) {
  int gpio = (int)user;
  return gGpioTlmm->DirectionOutput(gpio, value);
}

static int tlmm_gpio_get(void *user) {
  int gpio = (int)user;
  return gGpioTlmm->Get(gpio);
}

static void tlmm_gpio_set(void *user, int value) {
  int gpio = (int)user;
  gGpioTlmm->Set(gpio, value);
}

struct lkl_gpio_callback tlmm_gpio_callback = {
	.get_direction = tlmm_gpio_get_direction,
	.direction_input = tlmm_gpio_direction_input,
	.direction_output = tlmm_gpio_direction_output,
	.get = tlmm_gpio_get,
	.set = tlmm_gpio_set,
};
