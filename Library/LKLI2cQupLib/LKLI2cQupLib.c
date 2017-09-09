#include <PiDxe.h>
#include <Library/LKEnvLib.h>
#include <Library/QcomI2cQupLib.h>

void* lkl_platform_i2c_init(int id) {
    return gI2cQup->GetDevice(id);
}

int lkl_platform_i2c_xfer(void *handle, struct i2c_msg msgs[], int num) {
    return gI2cQup->Transfer(handle, msgs, num);
}

void lkl_platform_i2c_iterate(void (*cb)(UINT8)) {
    gI2cQup->Iterate(cb);
}
