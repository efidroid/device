#include <dev/pm8921.h>

#define BITS_IN_ELEMENT(x) (sizeof(x) * 8)
#define KEYMAP_INDEX(row, col) (row)* BITS_IN_ELEMENT(unsigned int) + (col)

unsigned int apq8064_pm8921_keys_gpiomap[] = {
	[KEYMAP_INDEX(0, 0)] = PM_GPIO(1),	/* Volume key on the device/CDP */
	[KEYMAP_INDEX(0, 1)] = PM_GPIO(2),	/* Volume key on the device/CDP */
};
