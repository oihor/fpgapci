#ifndef FPGA_PCI_DRIVER_H
#define FPGA_PCI_DRIVER_H

#include <linux/types.h>

struct fpgapci_driver {
    u8 __iomem *hwmem;
};

#endif

