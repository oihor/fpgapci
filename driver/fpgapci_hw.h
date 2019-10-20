#ifndef FPGA_PCI_HW_H
#define FPGA_PCI_HW_H

#include "fpgapci_driver.h"

int fpgapci_ioctl_read_data(struct fpgapci_driver *drv, unsigned long arg);
int fpgapci_ioctl_write_data(struct fpgapci_driver *drv, unsigned long arg);

#endif

