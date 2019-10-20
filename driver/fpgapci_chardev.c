#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>

#include "fpgapci_user.h"

#include "fpgapci_chardev.h"
#include "fpgapci_hw.h"

#define DEV_MAJOR 19

static int fpgapcidev_open(struct inode *inode, struct file *file);
static int fpgapcidev_release(struct inode *inode, struct file *file);
static long fpgapcidev_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static ssize_t fpgapcidev_read(struct file *file, char __user *buf, size_t count, loff_t *offset);
static ssize_t fpgapcidev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset);

static const struct file_operations fpgapcidev_fops = {
    .owner = THIS_MODULE,
    .open = fpgapcidev_open,
    .release = fpgapcidev_release,
    .unlocked_ioctl = fpgapcidev_ioctl,
    .read = fpgapcidev_read,
    .write = fpgapcidev_write
};

struct fpgapci_priv {
    struct fpgapci_driver *drv;
};

static int dev_major = 0;
static struct class *fpgapciclass = NULL;
static struct device *fpgapcidev;
static struct cdev cdev;
static struct fpgapci_driver *drv_access = NULL;

static int fpgapci_uevent(struct device *dev, struct kobj_uevent_env *env) {
    add_uevent_var(env, "DEVMODE=%#o", 0666);

    return 0;
}

int fpgapci_create_char_dev(struct fpgapci_driver *drv) {
    int err;
    dev_t dev;

    err = alloc_chrdev_region(&dev, 0, 1, "fpgapci");

    dev_major = MAJOR(dev);

    fpgapciclass = class_create(THIS_MODULE, "fpgapci-dev");

    fpgapciclass->dev_uevent = fpgapci_uevent;

    cdev_init(&cdev, &fpgapcidev_fops);
    cdev.owner = THIS_MODULE;
    cdev_add(&cdev, MKDEV(dev_major, 0), 1);

    fpgapcidev = device_create(fpgapciclass, NULL, MKDEV(dev_major, 0), NULL, "fpgapci");

    drv_access = drv;

    return 0;
}

int fpgapci_destroy_char_dev(void) {
    device_destroy(fpgapciclass, MKDEV(dev_major, 0));

    class_unregister(fpgapciclass);
    class_destroy(fpgapciclass);
    unregister_chrdev_region(MKDEV(dev_major, 0), MINORMASK);

    return 0;
}

static int fpgapcidev_open(struct inode *inode, struct file *file) {
    struct fpgapci_priv *priv;

    priv = kzalloc(sizeof(struct fpgapci_priv), GFP_KERNEL);
    priv->drv = drv_access;

    file->private_data = priv;

    return 0;
}

static int fpgapcidev_release(struct inode *inode, struct file *file) {
    struct fpgapci_priv *priv = file->private_data;

    kfree(priv);
    priv = NULL;

    return 0;
}

static long fpgapcidev_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    struct fpgapci_priv *drv = file->private_data;

    switch (cmd) {
    case FPGA_PCI_IOCTL_READ:
        return fpgapci_ioctl_read_data(drv->drv, arg);

    case FPGA_PCI_IOCTL_WRITE:
        return fpgapci_ioctl_write_data(drv->drv, arg);

    default:
        return -EINVAL;
    };

    return 0;
}

static ssize_t fpgapcidev_read(struct file *file, char __user *buf, size_t count, loff_t *offset) {
    return 0;
}

static ssize_t fpgapcidev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset) {
    return 0;
}

