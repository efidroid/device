#include <Base.h>
#include <Library/LKEnvLib.h>
#include <Library/QcomSmemLib.h>
#include <Library/QcomBoardLib.h>
#include <Library/QcomClockLib.h>
#include <Library/QcomGpioTlmmLib.h>
#include <Chipset/baseband.h>
#include <Target/board.h>
#include <Library/QcomTargetUartDmLib.h>

#define LINUX_MACHTYPE_8064_MITWO   4180

RETURN_STATUS LibQcomTargetUartCallSecConstructors(VOID)
{
  SmemImplLibInitialize ();
  BoardImplLibInitialize ();
  ClockImplLibInitialize ();
  GpioTlmmImplLibInitialize ();

  return RETURN_SUCCESS;
}

RETURN_STATUS LibQcomTargetGetUartDmConfig(UINT8 *Id, UINTN *GsbiBase, UINTN *UartDmBase)
{
	unsigned target_id = gBoard->board_target_id();

	switch (target_id) {
	case LINUX_MACHTYPE_8960_SIM:
	case LINUX_MACHTYPE_8960_RUMI3:
	case LINUX_MACHTYPE_8960_CDP:
	case LINUX_MACHTYPE_8960_MTP:
	case LINUX_MACHTYPE_8960_FLUID:
	case LINUX_MACHTYPE_8960_APQ:
	case LINUX_MACHTYPE_8960_LIQUID:

		if(gBoard->board_baseband() == BASEBAND_SGLTE || gBoard->board_baseband() == BASEBAND_SGLTE2)
		{
			*Id = 8;
			*GsbiBase = 0x1A000000;
			*UartDmBase = 0x1A040000;
		}
		else
		{
			*Id = 5;
			*GsbiBase = 0x16400000;
			*UartDmBase = 0x16440000;
		}
		break;

	case LINUX_MACHTYPE_8930_CDP:
	case LINUX_MACHTYPE_8930_MTP:
	case LINUX_MACHTYPE_8930_FLUID:
		*Id = 5;
		*GsbiBase = 0x16400000;
		*UartDmBase = 0x16440000;
		break;

	case LINUX_MACHTYPE_8064_SIM:
	case LINUX_MACHTYPE_8064_RUMI3:
		*Id = 3;
		*GsbiBase = 0x16400000;
		*UartDmBase = 0x16440000;
		break;

	case LINUX_MACHTYPE_8064_CDP:
	case LINUX_MACHTYPE_8064_MTP:
	case LINUX_MACHTYPE_8064_LIQUID:
	case LINUX_MACHTYPE_8064_MITWO:
		*Id = 7;
		*GsbiBase = 0x16600000;
		*UartDmBase = 0x16640000;
		break;

	case LINUX_MACHTYPE_8064_MPQ_CDP:
	case LINUX_MACHTYPE_8064_MPQ_HRD:
	case LINUX_MACHTYPE_8064_MPQ_DTV:
		*Id = 5;
		*GsbiBase = 0x1A200000;
		*UartDmBase = 0x1A240000;
		break;

	case LINUX_MACHTYPE_8627_CDP:
	case LINUX_MACHTYPE_8627_MTP:
		*Id = 5;
		*GsbiBase = 0x16400000;
		*UartDmBase = 0x16440000;
		break;

	default:
		DEBUG((EFI_D_ERROR, "uart gsbi not defined for target: %d\n", target_id));
		return RETURN_UNSUPPORTED;
	}

	return RETURN_SUCCESS;
}
