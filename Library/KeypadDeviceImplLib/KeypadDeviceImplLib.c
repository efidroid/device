#include <PiDxe.h>

#include <Library/LKEnvLib.h>
#include <Library/QcomPm8921Lib.h>
#include <Library/KeypadDeviceImplLib.h>
#include <Library/KeypadDeviceHelperLib.h>
#include <Protocol/KeypadDevice.h>

STATIC KEY_CONTEXT KeyContextPower;
STATIC KEY_CONTEXT KeyContextVolumeUp;
STATIC KEY_CONTEXT KeyContextVolumeDown;

/* pm8921 GPIO configuration for APQ8064 keypad */
static struct pm8xxx_gpio_init pm8921_keypad_gpios_apq[] = {
  /* keys GPIOs */
  PM8XXX_GPIO_INPUT(PM_GPIO(1), PM_GPIO_PULL_UP_31_5),
  PM8XXX_GPIO_INPUT(PM_GPIO(2), PM_GPIO_PULL_UP_31_5),
  PM8XXX_GPIO_OUTPUT(PM_GPIO(9), 0),
  PM8XXX_GPIO_OUTPUT(PM_GPIO(10), 0),
};

EFI_STATUS EFIAPI KeypadDeviceImplReset (KEYPAD_DEVICE_PROTOCOL *This)
{
  gPM8921->pm8921_gpio_config_array(pm8921_keypad_gpios_apq, ARRAY_SIZE(pm8921_keypad_gpios_apq));

  LibKeyInitializeKeyContext(&KeyContextPower);
  KeyContextPower.KeyData.Key.UnicodeChar = CHAR_CARRIAGE_RETURN;

  LibKeyInitializeKeyContext(&KeyContextVolumeUp);
  KeyContextVolumeUp.KeyData.Key.ScanCode = SCAN_UP;

  LibKeyInitializeKeyContext(&KeyContextVolumeDown);
  KeyContextVolumeDown.KeyData.Key.ScanCode = SCAN_DOWN;

  return EFI_SUCCESS;
}

EFI_STATUS KeypadDeviceImplGetKeys (KEYPAD_DEVICE_PROTOCOL *This, KEYPAD_RETURN_API *KeypadReturnApi, UINT64 Delta)
{
  UINT8 GpioStatus;
  INTN RC;

  RC = gPM8921->pm8921_pwrkey_status(&GpioStatus);
  LibKeyUpdateKeyStatus(&KeyContextPower, KeypadReturnApi, (RC==0 && GpioStatus), Delta);

  RC = gPM8921->pm8921_gpio_get(PM_GPIO(1), &GpioStatus);
  LibKeyUpdateKeyStatus(&KeyContextVolumeUp, KeypadReturnApi, (RC==0 && GpioStatus), Delta);

  RC = gPM8921->pm8921_gpio_get(PM_GPIO(2), &GpioStatus);
  LibKeyUpdateKeyStatus(&KeyContextVolumeDown, KeypadReturnApi, (RC==0 && GpioStatus), Delta);

  return EFI_SUCCESS;
}

