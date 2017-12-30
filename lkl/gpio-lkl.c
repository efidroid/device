#include <linux/err.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/bitops.h>
#include <linux/platform_device.h>
#include <linux/device/gpio_lkl.h>

struct internal_callback_data {
	struct lkl_gpio_callback *cb;
	int irq;
	void *user;
};

static struct internal_callback_data callbacks[32] = {0};

int lkl_gpio_register_gpio(struct lkl_gpio_callback *cb, int irq, void *user) {
	size_t i;

	if (!cb)
		return -1;

	for(i=0; i<ARRAY_SIZE(callbacks); i++) {
		if (callbacks[i].cb == NULL) {
			callbacks[i].cb = cb;
			callbacks[i].irq = irq;
			callbacks[i].user = user;
			return i;
		}
	}

	return -1;
}

void lkl_gpio_remove_gpio(int offset) {
	if (offset >= ARRAY_SIZE(callbacks))
		return;

	memset(&callbacks[offset].cb, 0, sizeof(struct internal_callback_data));
}

static int lkl_gpio_to_irq(struct gpio_chip *gc, unsigned offset)
{
	if (offset >= ARRAY_SIZE(callbacks))
		return -1;

	return callbacks[offset].irq;
}

static int lkl_gpio_get_direction(struct gpio_chip *gc, unsigned offset)
{
	if (offset >= ARRAY_SIZE(callbacks))
		return -1;

	if (!callbacks[offset].cb->get_direction)
		return -1;

	return callbacks[offset].cb->get_direction(callbacks[offset].user);
}

static int lkl_gpio_direction_input(struct gpio_chip *chip, unsigned offset)
{
	if (offset >= ARRAY_SIZE(callbacks))
		return -1;

	if (!callbacks[offset].cb->direction_input)
		return -1;

	return callbacks[offset].cb->direction_input(callbacks[offset].user);
}

static int lkl_gpio_direction_output(struct gpio_chip *chip, unsigned offset, int value)
{
	if (offset >= ARRAY_SIZE(callbacks))
		return -1;

	if (!callbacks[offset].cb->direction_output)
		return -1;

	return callbacks[offset].cb->direction_output(callbacks[offset].user, value);
}

static int lkl_gpio_get(struct gpio_chip *gc, unsigned offset)
{
	if (offset >= ARRAY_SIZE(callbacks))
		return -1;

	if (!callbacks[offset].cb->get)
		return -1;

	return callbacks[offset].cb->get(callbacks[offset].user);
}

static void lkl_gpio_set(struct gpio_chip *gc, unsigned offset,
				    int value)
{
	if (offset >= ARRAY_SIZE(callbacks))
		return;

	if (!callbacks[offset].cb->set)
		return;

	return callbacks[offset].cb->set(callbacks[offset].user, value);
}

static struct gpio_chip lkl_gpiochip = {
	.label		= "lkl",
	.base		= 0,
	.ngpio		= ARRAY_SIZE(callbacks),
	.to_irq		= lkl_gpio_to_irq,
	.get_direction	= lkl_gpio_get_direction,
	.direction_input = lkl_gpio_direction_input,
	.direction_output = lkl_gpio_direction_output,
	.get		= lkl_gpio_get,
	.set		= lkl_gpio_set,
};

static int lkl_gpio_probe(struct platform_device *pdev)
{
	return gpiochip_add_data(&lkl_gpiochip, NULL);
}

static struct platform_driver xtensa_gpio_driver = {
	.driver		= {
		.name		= "lkl-gpio",
	},
	.probe		= lkl_gpio_probe,
};

static int __init lkl_gpio_init(void)
{
	struct platform_device *pdev;

	pdev = platform_device_register_simple("lkl-gpio", 0, NULL, 0);
	if (IS_ERR(pdev))
		return PTR_ERR(pdev);

	return platform_driver_register(&xtensa_gpio_driver);
}
arch_initcall(lkl_gpio_init);

MODULE_AUTHOR("Michael Zimmermann <sigmaepsilon92@gmail.com>");
MODULE_DESCRIPTION("LKL GPIO driver");
MODULE_LICENSE("GPL");
