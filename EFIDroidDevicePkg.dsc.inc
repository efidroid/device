[PcdsFixedAtBuild.common]
  gQcomTokenSpaceGuid.PcdMipiDsiRgbSwap|2 # DSI_RGB_SWAP_BGR

[LibraryClasses.common]
  KeypadDeviceImplLib|EFIDroidDevicePkg/Library/KeypadDeviceImplLib/KeypadDeviceImplLib.inf

  QcomTargetUartDmLib|EFIDroidDevicePkg/Library/QcomTargetUartDmLib/QcomTargetUartDmLib.inf
  QcomPlatformUartDmLib|EFIDroidDevicePkg/Library/QcomPlatformUartDmLib/QcomPlatformUartDmLib.inf

  QcomPlatformI2cQupGsbiLib|EFIDroidDevicePkg/Library/QcomPlatformI2cQupGsbiLib/QcomPlatformI2cQupGsbiLib.inf

[Components.common]
  EFIDroidPkg/Drivers/KeypadDxe/KeypadDxe.inf
  EFIDroidPkg/Drivers/GenericKeypadDeviceDxe/GenericKeypadDeviceDxe.inf

  #
  # LKL support
  #
  EFIDroidLKLPkg/LKL/LKL.inf {
    <LibraryClasses>
      NULL|EFIDroidDevices/generic/common/Library/LKLI2cQupLib/LKLI2cQupLib.inf
      NULL|EFIDroidDevices/generic/common/Library/LKLGpioTlmmIrqLib/LKLGpioTlmmIrqLib.inf
      NULL|EFIDroidDevicePkg/Library/LKLTouchScreenLib/LKLTouchScreenLib.inf
  }

  EFIDroidLKLPkg/LKLTS/LKLTS.inf
