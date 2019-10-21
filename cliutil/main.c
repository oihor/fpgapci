#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "fpgapci_user.h"

// r/w 8/16/32/64 addr [val]

int main(int argc, char *argv[]) {

    if(argc != 4 && argc != 5) {
        printf("Usage: fpgapci_test_util r/w 8/16/32/64 addr [val]\n");
        return EXIT_FAILURE;
    }

    int *fd = open("/dev/fpgapci", O_RDWR);

    if (fd == -1) {
        printf("Open /dev/fpgapci failed! Error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    struct FpgaPciIoCtlRwData rw_struct = { 64, 8, 0xFFEA12340987CDFF };

    ioctl(fd, FPGA_PCI_IOCTL_WRITE, &rw_struct);

    //ioctl(fd, FPGA_PCI_READ, &rw_struct);

    return EXIT_SUCCESS;
}
