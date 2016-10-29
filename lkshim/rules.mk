LOCAL_DIR := $(GET_LOCAL_DIR)

#ifeq ($(WITH_KERNEL_UEFIAPI),1)
#	SHIMOBJS += msm8974.o
#endif

SHIMOBJS += \
	$(LOCAL_DIR)/init.o
