/*
 * LKL master mode driver
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

struct lkl_i2c_dev {
	struct device *dev;
	struct i2c_adapter adapter;
	void *handle;
};

void* lkl_platform_i2c_init(int id);
int lkl_platform_i2c_xfer(void *handle, struct i2c_msg msgs[], int num);
void lkl_platform_i2c_iterate(void (*cb)(u8));

static int lkl_i2c_xfer(struct i2c_adapter *adap, struct i2c_msg msgs[],
				int num)
{
	struct lkl_i2c_dev *i2c_dev = i2c_get_adapdata(adap);
	return lkl_platform_i2c_xfer(i2c_dev->handle, msgs, num);
}

static u32 lkl_i2c_func(struct i2c_adapter *adap)
{
	return I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL;
}

static const struct i2c_algorithm lkl_i2c_algo = {
	.master_xfer	= lkl_i2c_xfer,
	.functionality	= lkl_i2c_func,
};

static int lkl_i2c_probe(struct platform_device *pdev)
{
	struct lkl_i2c_dev *i2c_dev;
	struct i2c_adapter *adap;
	void *handle;

	handle = lkl_platform_i2c_init(pdev->id);
	if (handle==NULL)
		return -1;

	i2c_dev = devm_kzalloc(&pdev->dev, sizeof(*i2c_dev), GFP_KERNEL);
	if (!i2c_dev)
		return -ENOMEM;
	platform_set_drvdata(pdev, i2c_dev);
	i2c_dev->dev = &pdev->dev;
	i2c_dev->handle = handle;

	adap = &i2c_dev->adapter;
	i2c_set_adapdata(adap, i2c_dev);
	adap->owner = THIS_MODULE;
	adap->class = I2C_CLASS_DEPRECATED;
	strlcpy(adap->name, "lkl I2C adapter", sizeof(adap->name));
	adap->algo = &lkl_i2c_algo;
	adap->nr = pdev->id;
	adap->dev.parent = &pdev->dev;
	adap->dev.of_node = pdev->dev.of_node;

	return i2c_add_numbered_adapter(adap);
}

static int lkl_i2c_remove(struct platform_device *pdev)
{
	struct lkl_i2c_dev *i2c_dev = platform_get_drvdata(pdev);

	i2c_del_adapter(&i2c_dev->adapter);

	return 0;
}

static struct platform_driver lkl_i2c_driver = {
	.probe		= lkl_i2c_probe,
	.remove		= lkl_i2c_remove,
	.driver		= {
		.name	= "i2c-lkl",
	},
};
module_platform_driver(lkl_i2c_driver);

static void iterate_callback(u8 id) {
	struct platform_device *dev;

	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (!dev)
		return;

	dev->name = "i2c-lkl";
	dev->id = id;

	platform_device_register(dev);
}

static int lkl_i2c_init(void) {
	lkl_platform_i2c_iterate(iterate_callback);
	return 0;
}
module_init(lkl_i2c_init);

MODULE_AUTHOR("Michael Zimmermann <sigmaepsilon92@gmail.com>");
MODULE_DESCRIPTION("LKL I2C bus adapter");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:i2c-lkl");
