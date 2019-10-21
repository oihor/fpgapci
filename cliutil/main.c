#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "fpgapci_user.h"

// r/w 8/16/32/64 hexaddr [hexval]

int main(int argc, char *argv[]) {

    struct FpgaPciIoCtlRwData rw_struct = { 0, 0, 0 };
    uint32_t ioctl_op = 0;
    bool read = false;
    bool write = false;

    if(argc != 4 && argc != 5) {
        printf("Usage: fpgapci_test_util r/w 8/16/32/64 hexaddr [hexval]\n");
        return EXIT_FAILURE;
    }

    char *rwstr = argv[1];
    if(strcmp(rwstr, "r") == 0) {
        read = true;
        ioctl_op = FPGA_PCI_IOCTL_READ;
    } else if(strcmp(rwstr, "w") == 0) {
        write= true;
        ioctl_op = FPGA_PCI_IOCTL_WRITE;
    } else {
        printf("Usage: fpgapci_test_util r/w 8/16/32/64 hexaddr [hexval]\n");
        printf("Provide r or w. Provided %s\n", rwstr);
        return EXIT_FAILURE;
    }

    if(read && argc != 4) {
        printf("Usage for read: fpgapci_test_util r/w 8/16/32/64 hexaddr\n");
        printf("Provide 3 args\n");
        return EXIT_FAILURE;
    }

    if(write && argc != 5) {
        printf("Usage for write: fpgapci_test_util r/w 8/16/32/64 hexaddr hexval\n");
        printf("Provide 4 args\n");
        return EXIT_FAILURE;
    }

    char *widthstr = argv[2];
    rw_struct.width = strtoul(widthstr, NULL, 10);
    switch(rw_struct.width) {
    case 8:
    case 16:
    case 32:
    case 64:
        break;
    default:
        printf("Usage for write: fpgapci_test_util r/w 8/16/32/64 hexaddr hexval\n");
        printf("Provide correct width 8/16/32/64. Provided %s\n", widthstr);
        return EXIT_FAILURE;
    }

    char *hexaddrstr = argv[3];
    rw_struct.addr = strtoul(hexaddrstr, NULL, 16);
    if(errno != 0) {
        printf("Usage for write: fpgapci_test_util r/w 8/16/32/64 hexaddr hexval\n");
        printf("Provide correct hex addr. Provided %s\n", hexaddrstr);
        return EXIT_FAILURE;
    }

    if(write) {
        char *hexvalstr = argv[3];
        rw_struct.val = strtoull(hexvalstr, NULL, 16);
        if(errno != 0) {
            printf("Usage for write: fpgapci_test_util r/w 8/16/32/64 hexaddr hexval\n");
            printf("Provide correct hex val. Provided %s\n", hexvalstr);
            return EXIT_FAILURE;
        }
    }

    int *fd = open("/dev/fpgapci", O_RDWR);

    if (fd == -1) {
        printf("Open /dev/fpgapci failed! Error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    int err = ioctl(fd, ioctl_op, &rw_struct);
    if(err == -1) {
        printf("ioctl to /dev/fpgapci failed! Error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    if(read) {
        printf("Read: %#0*lx\n", rw_struct.width / 4 + 2, rw_struct.val);
    }

    return EXIT_SUCCESS;
}
