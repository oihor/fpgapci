#include <linux/uaccess.h>

#include "fpgapci_user.h"

#include "fpgapci_hw.h"
#include "fpgapci_log.h"

int fpgapci_ioctl_read_data(struct fpgapci_driver *drv, unsigned long arg) {
    int err = 0;
    void *mem_addr;
    struct FpgaPciIoCtlRwData rw_struct;

    LOG_INFO("read_data\n");

    err = copy_from_user(&rw_struct, (void*) (arg), sizeof(rw_struct));
    if (err) {
        return err;
    }

    mem_addr = drv->hwmem + rw_struct.addr;

    switch (rw_struct.width) {
    case 8:
        rw_struct.val = ioread8(mem_addr);
        break;
    case 16:
        rw_struct.val = ioread16(mem_addr);
        break;
    case 32:
        rw_struct.val = ioread32(mem_addr);
        break;
    case 64:
        rw_struct.val = readq(mem_addr);
        break;
    }

    err = copy_to_user((void*) (arg), &rw_struct, sizeof(rw_struct));
    if (err) {
        return err;
    }

    return 0;
}

int fpgapci_ioctl_write_data(struct fpgapci_driver *drv, unsigned long arg) {
    int err = 0;
    void *mem_addr;
    struct FpgaPciIoCtlRwData rw_struct;

    LOG_INFO("write_data\n");

    err = copy_from_user(&rw_struct, (void*) (arg), sizeof(rw_struct));
    if (err) {
        return err;
    }

    mem_addr = drv->hwmem + rw_struct.addr;

    switch (rw_struct.width) {
    case 8:
        iowrite8(rw_struct.val, mem_addr);
        break;
    case 16:
        iowrite16(rw_struct.val, mem_addr);
        break;
    case 32:
        iowrite32(rw_struct.val, mem_addr);
        break;
    case 64:
        writeq(rw_struct.val, mem_addr);
        break;
    }

    return 0;
}

