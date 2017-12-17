#include <PiDxe.h>
#include <Library/LKEnvLib.h>
#include <Chipset/gsbi.h>
#include <Chipset/smem.h>
#include <Target/board.h>
#include <Library/QcomClockLib.h>
#include <Library/QcomBoardLib.h>
#include <Library/QcomGpioTlmmLib.h>

#define MSM_BOOT_UART_SWITCH_GPIO_MITWOA	33
#define MSM_BOOT_UART_SWITCH_GPIO_MITWO		62

#define LINUX_MACHTYPE_8064_MITWO   4180

/* Configure UART clock - based on the gsbi id */
VOID LibQcomPlatformUartDmClockConfig(UINT8 id)
{
  CHAR8 gsbi_uart_clk_id[64];
  CHAR8 gsbi_p_clk_id[64];

  snprintf(gsbi_uart_clk_id, 64,"gsbi%u_uart_clk", id);
  gClock->clk_get_set_enable(gsbi_uart_clk_id, 1843200, 1);

  snprintf(gsbi_p_clk_id, 64,"gsbi%u_pclk", id);
  gClock->clk_get_set_enable(gsbi_p_clk_id, 0, 1);
}

/* Configure gpio for uart - based on gsbi id */
VOID LibQcomPlatformUartDmGpioConfig(UINT8 id)
{
	UINTN target_id = gBoard->board_target_id();

	// enable UART switch
	if(target_id == LINUX_MACHTYPE_8960_CDP)
	{
		gGpioTlmm->gpio_tlmm_config(MSM_BOOT_UART_SWITCH_GPIO_MITWOA, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_16MA, GPIO_ENABLE);
		gGpioTlmm->gpio_set(MSM_BOOT_UART_SWITCH_GPIO_MITWOA, 2);

	} else if (target_id==LINUX_MACHTYPE_8064_MTP || target_id==LINUX_MACHTYPE_8064_MITWO) {
		gGpioTlmm->gpio_tlmm_config(MSM_BOOT_UART_SWITCH_GPIO_MITWO, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_16MA, GPIO_ENABLE);
		gGpioTlmm->gpio_set(MSM_BOOT_UART_SWITCH_GPIO_MITWO, 2);
	}

	if (gBoard->board_platform_id() == MPQ8064 || gBoard->board_platform_id() == MSM8260AB)
	{
		switch (id) {

		case GSBI_ID_5:
			/* configure rx gpio */
			gGpioTlmm->gpio_tlmm_config(23, 2, GPIO_INPUT, GPIO_NO_PULL,
							 GPIO_8MA, GPIO_DISABLE);
			/* configure tx gpio */
			gGpioTlmm->gpio_tlmm_config(22, 2, GPIO_OUTPUT, GPIO_NO_PULL,
							 GPIO_8MA, GPIO_DISABLE);
			break;

		default:
			ASSERT(0);
		}
	}
	else if((gBoard->board_platform_id() == APQ8064) ||
			(gBoard->board_platform_id() == APQ8064AA) ||
			(gBoard->board_platform_id() == APQ8064AB))
	{
		switch (id) {

		case GSBI_ID_1:
			/* configure rx gpio */
			gGpioTlmm->gpio_tlmm_config(19, 1, GPIO_INPUT, GPIO_NO_PULL,
							 GPIO_8MA, GPIO_DISABLE);
			/* configure tx gpio */
			gGpioTlmm->gpio_tlmm_config(18, 1, GPIO_OUTPUT, GPIO_NO_PULL,
							 GPIO_8MA, GPIO_DISABLE);
			break;


		case GSBI_ID_7:
			/* configure rx gpio */
			gGpioTlmm->gpio_tlmm_config(83, 1, GPIO_INPUT, GPIO_NO_PULL,
							 GPIO_8MA, GPIO_DISABLE);
			/* configure tx gpio */
			gGpioTlmm->gpio_tlmm_config(82, 2, GPIO_OUTPUT, GPIO_NO_PULL,
							 GPIO_8MA, GPIO_DISABLE);
			break;

		default:
			ASSERT(0);
		}
	}
	else
	{
		switch (id) {

		case GSBI_ID_3:
			/* configure rx gpio */
			gGpioTlmm->gpio_tlmm_config(15, 1, GPIO_INPUT, GPIO_NO_PULL,
							 GPIO_8MA, GPIO_DISABLE);
			/* configure tx gpio */
			gGpioTlmm->gpio_tlmm_config(14, 1, GPIO_OUTPUT, GPIO_NO_PULL,
							 GPIO_8MA, GPIO_DISABLE);
			break;

		case GSBI_ID_5:
			/* configure rx gpio */
			gGpioTlmm->gpio_tlmm_config(23, 1, GPIO_INPUT, GPIO_NO_PULL,
							 GPIO_8MA, GPIO_DISABLE);
			/* configure tx gpio */
			gGpioTlmm->gpio_tlmm_config(22, 1, GPIO_OUTPUT, GPIO_NO_PULL,
							 GPIO_8MA, GPIO_DISABLE);
			break;

		case GSBI_ID_8:
			/* configure rx gpio */
			gGpioTlmm->gpio_tlmm_config(35, 1, GPIO_INPUT, GPIO_NO_PULL,
							 GPIO_8MA, GPIO_DISABLE);
			/* configure tx gpio */
			gGpioTlmm->gpio_tlmm_config(34, 1, GPIO_OUTPUT, GPIO_NO_PULL,
							 GPIO_8MA, GPIO_DISABLE);
			break;

		default:
			ASSERT(0);
		}
	}
}
