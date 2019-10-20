#include <linux/module.h>
#include <linux/pci.h>

#include "fpgapci_chardev.h"
#include "fpgapci_driver.h"
#include "fpgapci_log.h"

#define FPGA_PCI_VENDOR_ID_1    0x1173
#define FPGA_PCI_PRODUCT_ID_1   0xE002

static struct pci_device_id fpgapci_id_table[] = {
    {
        PCI_DEVICE(FPGA_PCI_VENDOR_ID_1, FPGA_PCI_PRODUCT_ID_1)
    },
    {
        0,
    }
};

MODULE_DEVICE_TABLE(pci, fpgapci_id_table);

static int fpgapci_probe(struct pci_dev *pdev, const struct pci_device_id *ent);
static void fpgapci_remove(struct pci_dev *pdev);

static struct pci_driver fpgapci_driver = {
    .name = DRIVER_NAME,
    .id_table = fpgapci_id_table,
    .probe = fpgapci_probe,
    .remove = fpgapci_remove
};

static int __init fpgapci_init(void)
{
    LOG_INFO("Loading %s - version %s\n", DRIVER_DESCRIPTION, DRIVER_VERSION);
    LOG_INFO("%s\n", DRIVER_COPYRIGHT);

    return pci_register_driver(&fpgapci_driver);
}

static void __exit fpgapci_exit(void)
{
    pci_unregister_driver(&fpgapci_driver);
}

int read_device_config(struct pci_dev *pdev) {
    u16 vendor, device, status_reg, command_reg;
    u32 addr_reg;

    pci_read_config_word(pdev, PCI_VENDOR_ID, &vendor);
    pci_read_config_word(pdev, PCI_DEVICE_ID, &device);

    LOG_INFO("Device vid: 0x%X  pid: 0x%X\n", vendor, device);

    pci_read_config_word(pdev, PCI_STATUS, &status_reg);

    LOG_INFO("Device status reg: 0x%X\n", status_reg);

    pci_read_config_dword(pdev, PCI_BASE_ADDRESS_0, &addr_reg);
    LOG_INFO("Device addr 0 reg: 0x%X\n", addr_reg);
    pci_read_config_dword(pdev, PCI_BASE_ADDRESS_1, &addr_reg);
    LOG_INFO("Device addr 1 reg: 0x%X\n", addr_reg);
    pci_read_config_dword(pdev, PCI_BASE_ADDRESS_2, &addr_reg);
    LOG_INFO("Device addr 2 reg: 0x%X\n", addr_reg);
    pci_read_config_dword(pdev, PCI_BASE_ADDRESS_3, &addr_reg);
    LOG_INFO("Device addr 3 reg: 0x%X\n", addr_reg);
    pci_read_config_dword(pdev, PCI_BASE_ADDRESS_4, &addr_reg);
    LOG_INFO("Device addr 4 reg: 0x%X\n", addr_reg);
    pci_read_config_dword(pdev, PCI_BASE_ADDRESS_5, &addr_reg);
    LOG_INFO("Device addr 5 reg: 0x%X\n", addr_reg);

    pci_read_config_word(pdev, PCI_COMMAND, &command_reg);

    if (command_reg | PCI_COMMAND_MEMORY) {
        LOG_INFO("Device supports memory access\n");
        return 0;
    }

    LOG_ERROR("Device doesn't supports memory access!");
    return -EIO;
}

void release_device(struct pci_dev *pdev) {
    pci_release_region(pdev, pci_select_bars(pdev, IORESOURCE_MEM));
    pci_disable_device(pdev);
}

static int fpgapci_probe(struct pci_dev *pdev, const struct pci_device_id *ent) {
    int bar, err;
    unsigned long mmio_start, mmio_end, mmio_len;
    struct fpgapci_driver *drv_priv;

    //////////////
//    void *mem_addr;
//    uint32_t val32;
//    uint64_t val64;
//    uint32_t offset;
//    uint32_t val32q[4] = { 0xFF123456, 0x7890ABFF, 0xFFEA1234, 0x0987CDFF };
    //0xFF123456
    //0x7890ABFF
    //0xFFEA1234
    //0x0987CDFF
    //////////////

    if (read_device_config(pdev) < 0) {
        return -EIO;
    }

    bar = pci_select_bars(pdev, IORESOURCE_IO);

    LOG_INFO("Device available MEM BAR are 0x%x\n", bar);

    err = pci_enable_device_mem(pdev);

    LOG_INFO("after pci_enable_device_mem, err: %i\n", err);

    if (err) {
        LOG_INFO("Failed to enable FPGA PCI device memory, err: %i\n", err);
        LOG_ERROR("Failed to enable FPGA PCI device memory, err: %i\n", err);
        return err;
    }

    err = pci_request_region(pdev, bar, DRIVER_NAME);

    LOG_INFO("after pci_request_region, err: %i\n", err);

    if (err) {
        LOG_INFO("Failed to request region, err: %i\n", err);
        LOG_ERROR("Failed to request region, err: %i\n", err);
        pci_disable_device(pdev);
        return err;
    }

    mmio_start = pci_resource_start(pdev, 0);
    mmio_end = pci_resource_end(pdev, 0);
    mmio_len = pci_resource_len(pdev, 0);

    LOG_INFO(
        "FPGA PCI device resource 0: start at 0x%lx and end at 0x%lx with length %lu\n",
        mmio_start, mmio_end, mmio_len
    );

    drv_priv = kzalloc(sizeof(struct fpgapci_driver), GFP_KERNEL);

    if (!drv_priv) {
        LOG_INFO("Failed to allocate drv_priv\n");
        LOG_ERROR("Failed to allocate drv_priv\n");
        release_device(pdev);
        return -ENOMEM;
    }

    //drv_priv->hwmem = ioremap(mmio_start, mmio_len);
    drv_priv->hwmem = pci_iomap(pdev, 0, mmio_len);

    if (!drv_priv->hwmem) {
        LOG_INFO("Failed to ioremap\n");
        LOG_ERROR("Failed to ioremap\n");
        release_device(pdev);
        return -EIO;
    }

    LOG_INFO("FPGA PCI device mapped resource 0x%lx to 0x%p\n", mmio_start, drv_priv->hwmem);

    fpgapci_create_char_dev(drv_priv);

    pci_set_drvdata(pdev, drv_priv);

    ///////////////
    //offset = 8;
    //val32 = 0xFFEACDFF;
    //val32 = 0xFFCDEAFF;
    //val64 = 0xFFEA12340987CDFF;
    //mem_addr = drv_priv->hwmem + offset;
    //val = ioread32(mem_addr);
    //iowrite32(val32, mem_addr);
    //writeq(val64, mem_addr);
    //writesq(mem_addr, val64q, 2);
    //iowrite32_rep(mem_addr, val32q, 4);
    //iowrite32(0xFF123456, mem_addr);
    //iowrite32(0x7890ABFF, mem_addr + 4);

    //val32 = ioread32(mem_addr);
    //val64 = readq(mem_addr);

    return 0;
}

static void fpgapci_remove(struct pci_dev *pdev) {
    int bar;
    struct fpgapci_driver *drv_priv = pci_get_drvdata(pdev);

    fpgapci_destroy_char_dev();

    if (drv_priv) {
        if (drv_priv->hwmem) {
            //iounmap(drv_priv->hwmem);
            pci_iounmap(pdev, drv_priv->hwmem);
        }

        kfree(drv_priv);
    }

    bar = pci_select_bars(pdev, IORESOURCE_IO);
    LOG_INFO("Device release MEM BAR are 0x%x\n", bar);

    pci_release_region(pdev, bar);
    LOG_INFO("after pci_release_region\n");

    //err = pci_disable_device_mem(pdev);
    //LOG_INFO("after pci_disable_device_mem, err: %i\n", err);

    release_device(pdev);

    LOG_INFO("Unloaded %s\n", DRIVER_DESCRIPTION);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("");
MODULE_DESCRIPTION(DRIVER_DESCRIPTION);
MODULE_VERSION(DRIVER_VERSION);

module_init( fpgapci_init);
module_exit( fpgapci_exit);

