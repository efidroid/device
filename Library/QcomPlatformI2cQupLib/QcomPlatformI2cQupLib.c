#include <PiDxe.h>
#include <Library/LKEnvLib.h>
#include <Chipset/gsbi.h>
#include <Platform/iomap.h>
#include <Library/QcomClockLib.h>
#include <Library/QcomGpioTlmmLib.h>
#include <Library/QcomPlatformI2cQupLib.h>
#include <Library/MemoryAllocationLib.h>

#define GSBI_BASE1(id)           (0x16000000 + ((id-1) * 0x100000))
#define QUP_BASE1(id)            (GSBI_BASE1(id) + 0x80000)

#define SPS_GSBI_BASE(id)           (0x12440000 + ((id-1) * 0x40000))
#define SPS_QUP_BASE(id)            (SPS_GSBI_BASE(id) + 0x20000)

#define GIC_SPI_START 32
#define GSBI_QUP_IRQ(id)       ((id) <= 8 ? (GIC_SPI_START + 145 + 2*((id))) : \
                                            (GIC_SPI_START + 187 + 2*((id)-8)))

STATIC
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

STATIC
VOID
EFIAPI
LibQcomPlatformI2cQupGsbiGpioConfig (
  UINT8 Id
  )
{
  switch (Id) {
  case GSBI_ID_1:
    gGpioTlmm->SetFunction(20, 1);
    gGpioTlmm->SetDriveStrength(20, 8);
    gGpioTlmm->SetPull(20, GPIO_PULL_NONE);
    gGpioTlmm->DirectionInput(20);

    gGpioTlmm->SetFunction(21, 1);
    gGpioTlmm->SetDriveStrength(21, 8);
    gGpioTlmm->SetPull(21, GPIO_PULL_NONE);
    gGpioTlmm->DirectionInput(21);
    break;

  case GSBI_ID_3:
    gGpioTlmm->SetFunction(9, 1);
    gGpioTlmm->SetDriveStrength(9, 8);
    gGpioTlmm->SetPull(9, GPIO_PULL_NONE);
    gGpioTlmm->DirectionInput(9);

    gGpioTlmm->SetFunction(8, 1);
    gGpioTlmm->SetDriveStrength(8, 8);
    gGpioTlmm->SetPull(8, GPIO_PULL_NONE);
    gGpioTlmm->DirectionInput(8);
    break;

  default:
    DEBUG((EFI_D_ERROR, "gpio_config_i2c(%hhu)\n", Id));
  }
}

STATIC
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

STATIC
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

STATIC
UINTN
EFIAPI
LibQcomPlatformI2cQupGetGsbiQupIrq (
  UINT8 Id
  )
{
  return GSBI_QUP_IRQ(Id);
}

STATIC
EFI_STATUS
RegisterGsbiI2cDevice (
  REGISTER_I2C_DEVICE RegisterI2cDevice,
  UINTN DeviceId,
  UINT8 gsbi_id,
  UINTN clk_freq,
  UINTN src_clk_rate
  )
{
  EFI_STATUS         Status;
  struct qup_i2c_dev *dev;

  dev = AllocateZeroPool(sizeof(struct qup_i2c_dev));
  if (!dev) {
    DEBUG((DEBUG_ERROR, "Can't allocate memory for i2c device\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  dev->id = DeviceId;
  dev->qup_irq = LibQcomPlatformI2cQupGetGsbiQupIrq(gsbi_id);
  dev->qup_base = LibQcomPlatformI2cQupGetGsbiQupBase(gsbi_id);
	dev->gsbi_base = LibQcomPlatformI2cQupGetGsbiBase(gsbi_id);
  dev->gsbi_number = gsbi_id;

  /* Initialize the GPIO for GSBIn as i2c */
  LibQcomPlatformI2cQupGsbiGpioConfig(gsbi_id);

  /* Configure the GSBI Protocol Code for i2c */
  writel((GSBI_PROTOCOL_CODE_I2C <<
    GSBI_CTRL_REG_PROTOCOL_CODE_S), GSBI_CTRL_REG(dev->gsbi_base));

  LibQcomPlatformI2cQupGsbiClockConfig(gsbi_id, src_clk_rate);

  dev->device_type = I2C_QUP_DEVICE_TYPE_GSBI;
  Status = RegisterI2cDevice(dev, clk_freq, src_clk_rate);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
LibQcomPlatformI2cQupAddBusses (
  REGISTER_I2C_DEVICE RegisterI2cDevice
  )
{
  RegisterGsbiI2cDevice (RegisterI2cDevice, 1, GSBI_ID_1, 100000, 24000000);
  RegisterGsbiI2cDevice (RegisterI2cDevice, 3, GSBI_ID_3, 100000, 24000000);

  return EFI_SUCCESS;
}
