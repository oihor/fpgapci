#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "fpgapci_user.h"

int main() {
    int *fd = open("/dev/fpgapci", O_RDWR);

    if (fd == -1) {
        printf("Open /dev/fpgapci failed! Error: %s\n", strerror(errno));
    }

    struct FpgaPciIoCtlRwData rw_struct = { 64, 8, 0xFFEA12340987CDFF };

    ioctl(fd, FPGA_PCI_IOCTL_WRITE, &rw_struct);

    //ioctl(fd, FPGA_PCI_READ, &rw_struct);

    return 0;
}
