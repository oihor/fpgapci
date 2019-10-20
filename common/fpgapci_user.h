#ifndef FPGA_PCI_USER_H
#define FPGA_PCI_USER_H

#define FPGA_PCI_IOCTL_WRITE  0x42
#define FPGA_PCI_IOCTL_READ   0x43

struct FpgaPciIoCtlRwData {
    uint32_t width;
    uint32_t addr;
    uint64_t val;
};

#endif

