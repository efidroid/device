[FV.FvMainDevice]
FvNameGuid         = E882890E-1540-41BD-8210-50BA75C85F58
BlockSize          = 0x40
NumBlocks          = 0         # This FV gets compressed so make it just big enough
FvAlignment        = 16        # FV alignment and FV attributes setting.
ERASE_POLARITY     = 1
MEMORY_MAPPED      = TRUE
STICKY_WRITE       = TRUE
LOCK_CAP           = TRUE
LOCK_STATUS        = TRUE
WRITE_DISABLED_CAP = TRUE
WRITE_ENABLED_CAP  = TRUE
WRITE_STATUS       = TRUE
WRITE_LOCK_CAP     = TRUE
WRITE_LOCK_STATUS  = TRUE
READ_DISABLED_CAP  = TRUE
READ_ENABLED_CAP   = TRUE
READ_STATUS        = TRUE
READ_LOCK_CAP      = TRUE
READ_LOCK_STATUS   = TRUE

  INF EFIDroidPkg/Drivers/KeypadDxe/KeypadDxe.inf
  INF EFIDroidPkg/Drivers/GenericKeypadDeviceDxe/GenericKeypadDeviceDxe.inf

  INF EFIDroidLKLPkg/LKLTS/LKLTS.inf
