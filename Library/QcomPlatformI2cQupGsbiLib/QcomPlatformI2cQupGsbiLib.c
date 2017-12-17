#include <PiDxe.h>
#include <Library/LKEnvLib.h>
#include <Chipset/gsbi.h>
#include <Platform/iomap.h>
#include <Library/QcomClockLib.h>
#include <Library/QcomGpioTlmmLib.h>
#include <Library/QcomPlatformI2cQupGsbiLib.h>

#define GSBI_BASE1(id)           (0x16000000 + ((id-1) * 0x100000))
#define QUP_BASE1(id)            (GSBI_BASE1(id) + 0x80000)

#define SPS_GSBI_BASE(id)           (0x12440000 + ((id-1) * 0x40000))
#define SPS_QUP_BASE(id)            (SPS_GSBI_BASE(id) + 0x20000)

#define GIC_SPI_START 32
#define GSBI_QUP_IRQ(id)       ((id) <= 8 ? (GIC_SPI_START + 145 + 2*((id))) : \
                                            (GIC_SPI_START + 187 + 2*((id)-8)))

VOID
EFIAPI
LibQcomPlatformI2cQupGsbiClockConfig (
  UINT8 Id,
  UINTN Frequency
  )
{
  char gsbi_qup_clk_id[64];
  char gsbi_p_clk_id[64];

  snprintf(gsbi_qup_clk_id, 64,"gsbi%u_qup_clk", Id);
  gClock->clk_get_set_enable(gsbi_qup_clk_id, 24000000, 1);

  snprintf(gsbi_p_clk_id, 64,"gsbi%u_pclk", Id);
  gClock->clk_get_set_enable(gsbi_p_clk_id, 0, 1);
}

VOID
EFIAPI
LibQcomPlatformI2cQupGsbiGpioConfig (
  UINT8 Id
  )
{
  switch (Id) {
  case GSBI_ID_1:
    gGpioTlmm->gpio_tlmm_config(20, 1, GPIO_OUTPUT, GPIO_NO_PULL,
                     GPIO_2MA, GPIO_DISABLE);
    gGpioTlmm->gpio_tlmm_config(21, 1, GPIO_OUTPUT, GPIO_NO_PULL,
                     GPIO_2MA, GPIO_DISABLE);
    break;

  case GSBI_ID_3:
    gGpioTlmm->gpio_tlmm_config(9, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_8MA,
                GPIO_ENABLE);
    gGpioTlmm->gpio_tlmm_config(8, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_8MA,
                GPIO_ENABLE);
    break;
  default:
    DEBUG((EFI_D_ERROR, "gpio_config_i2c(%hhu)\n", Id));
  }
}

UINTN
EFIAPI
LibQcomPlatformI2cQupGetGsbiBase (
  UINT8 Id
  )
{
  if(Id < 10)
    return GSBI_BASE1(Id);
  else
    return SPS_GSBI_BASE(Id);
}

UINTN
EFIAPI
LibQcomPlatformI2cQupGetGsbiQupBase (
  UINT8 Id
  )
{
  if(Id < 10)
    return QUP_BASE1(Id);
  else
    return SPS_QUP_BASE(Id);
}

UINTN
EFIAPI
LibQcomPlatformI2cQupGetGsbiQupIrq (
  UINT8 Id
  )
{
  return GSBI_QUP_IRQ(Id);
}

EFI_STATUS
EFIAPI
LibQcomPlatformI2cQupAddGsbiBusses (
  REGISTER_GSBI_I2C_DEVICE RegisterI2cDevice
  )
{
  RegisterI2cDevice (1, GSBI_ID_1, 100000, 24000000);
  RegisterI2cDevice (3, GSBI_ID_3, 100000, 24000000);

  return EFI_SUCCESS;
}
