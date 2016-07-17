EMMC_BOOT := 1
DISPLAY_2NDSTAGE := 1

# it's not nice to rely on the original values here but it's the best we can do right now
DEFINES := $(filter-out ABOOT_FORCE_RAMDISK_ADDR=0x08800000,$(DEFINES))
DEFINES := $(filter-out ABOOT_FORCE_TAGS_ADDR=0x09000000,$(DEFINES))

DEFINES += ABOOT_FORCE_RAMDISK_ADDR=0x02000000
DEFINES += ABOOT_FORCE_TAGS_ADDR=0x01e00000

