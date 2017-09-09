#include <PiDxe.h>
#include <Library/LKEnvLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/QcomGpioTlmmInterrupt.h>
#include <lkl.h>
#include <lkl_host.h>

#define MAX_NR_GPIO 300

STATIC QCOM_GPIO_TLMM_INTERRUPT_PROTOCOL *gGpioTlmmInterrupt = NULL;

STATIC volatile UINTN mGpiosPending[MAX_NR_GPIO] = {0};
STATIC EFI_EVENT mIrqEventNotify;
int lkl_gpio_tlmm_impl_irq_map[MAX_NR_GPIO] = {0};
int lkl_gpio_tlmm_impl_irq_map_size = MAX_NR_GPIO;

STATIC inline UINTN Gpio2Irq(UINTN Gpio) {
  UINTN Irq;

  for (Irq=0; Irq<lkl_gpio_tlmm_impl_irq_map_size; Irq++) {
    if (lkl_gpio_tlmm_impl_irq_map[Irq] == Gpio)
      return Irq;
  }

  return 0;
}

STATIC
VOID
EFIAPI
IrqCallbackNotify (
    IN  EFI_EVENT   Event,
    IN  VOID        *Context
)
{
  UINTN Gpio;
  UINTN Irq;
  UINTN Index;
  UINTN Count;
  EFI_TPL OldTpl;

  for (Gpio=0; Gpio<ARRAY_SIZE(mGpiosPending); Gpio++) {
    // atomically read and clear pending count
    OldTpl = gBS->RaiseTPL(TPL_HIGH_LEVEL);
    Count = mGpiosPending[Gpio];
    mGpiosPending[Gpio] = 0;
    gBS->RestoreTPL(OldTpl);

    // nothing to do
    if(Count==0)
      continue;

    // get lkl irq
    Irq = Gpio2Irq(Gpio);
    if (Irq == 0)
      continue;

    // trigger irq Count times
    for (Index=0; Index<Count; Index++) {
      lkl_trigger_irq(Irq);
    }
  }
}

STATIC VOID custom_gpio_handler(UINTN id) {
  EFI_TPL OldTpl = gBS->RaiseTPL(TPL_HIGH_LEVEL);

  // increase pending counter
  ASSERT(id<ARRAY_SIZE(mGpiosPending));
  mGpiosPending[id]++;

  // signal irq event
  gBS->SignalEvent(mIrqEventNotify);

  gBS->RestoreTPL(OldTpl);
}

void lkl_gpio_tlmm_impl_irq_setup(int gpio) {
    gGpioTlmmInterrupt->IrqSetCustomHandler(gpio, custom_gpio_handler);
}

void lkl_gpio_tlmm_impl_irq_ack(int gpio) {
    gGpioTlmmInterrupt->IrqAck(gpio);
}

void lkl_gpio_tlmm_impl_irq_mask(int gpio) {
    gGpioTlmmInterrupt->IrqMask(gpio);
}

void lkl_gpio_tlmm_impl_irq_unmask(int gpio) {
    gGpioTlmmInterrupt->IrqUnMask(gpio);
}

int lkl_gpio_tlmm_impl_irq_set_type(int gpio, unsigned int type) {
    return gGpioTlmmInterrupt->IrqSetType(gpio, type)!=EFI_SUCCESS;
}

RETURN_STATUS
EFIAPI
LKLGpioTlmmIrqLibConstructor (
  VOID
  )
{
  EFI_STATUS Status;

  Status = gBS->LocateProtocol (&gQcomGpioTlmmInterruptProtocolGuid, NULL, (VOID **)&gGpioTlmmInterrupt);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->CreateEvent (EVT_NOTIFY_SIGNAL, TPL_CALLBACK, IrqCallbackNotify, NULL, &mIrqEventNotify);
  ASSERT_EFI_ERROR (Status);

  return Status;
}
