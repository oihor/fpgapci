#ifndef FPGA_PCI_CHARDEV_H
#define FPGA_PCI_CHARDEV_H

#include "fpgapci_driver.h"

int fpgapci_create_char_dev(struct fpgapci_driver *drv);
int fpgapci_destroy_char_dev(void);

#endif
