#include <linux/init.h> /* Необходим для макросов */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/usb.h>

#define DRIVER_AUTHOR "opa"
#define DRIVER_DESC "USB beep module driver"
#define DEVICE_VENDOR_ID 0x0483
#define DEVICE_PRODUCT_ID 0xDEAD

#define TOTAL_BUFFER_SIZE 64 * 100
#define BEEP_MINOR_BASE 1

static atomic_t DeviceFree = ATOMIC_INIT(1);

static struct usb_device_id usbbeep_table[] = {
    {USB_DEVICE(DEVICE_VENDOR_ID, DEVICE_PRODUCT_ID)},
    {} /* Terminating entry */
};
MODULE_DEVICE_TABLE(usb, usbbeep_table);

static int usbbeep_probe(struct usb_interface* interface,
                         const struct usb_device_id* id);
void usbbeep_disconnect(struct usb_interface* interface);
static int usbbeep_open(struct inode* inode, struct file* file);
static int usbbeep_release(struct inode* inode, struct file* file);
static ssize_t usbbeep_write(struct file* file, const char __user* user_buf,
                             size_t count, loff_t* ppos);

struct usbbeep {
    /* One structure for each connected device */
    struct usb_device* udev;
    struct usb_interface* interface;
    unsigned char minor;

    struct usb_endpoint_descriptor* bulk_out_ep;
    unsigned int bulk_out_packet_size;
    unsigned char* videobuffer;
    struct mutex io_mutex;
};

struct usbbeep* dev;

struct usb_driver usbbeep_driver = {
    //.owner = THIS_MODULE,
    .name = "usbbeep",
    .probe = usbbeep_probe,
    .disconnect = usbbeep_disconnect,
    .id_table = usbbeep_table,
};

module_usb_driver(usbbeep_driver);

static struct file_operations usbbeep_fops = {
    .owner = THIS_MODULE,
    .write = usbbeep_write,
    .open = usbbeep_open,
    .release = usbbeep_release,
};

static struct usb_class_driver usbbeep_class = {
    .name = "beep%d",
    .fops = &usbbeep_fops,
    .minor_base = BEEP_MINOR_BASE,
};

static int usbbeep_probe(struct usb_interface* interface,
                         const struct usb_device_id* id) {

    // struct usbbeep* dev;
    struct usb_host_interface* iface_desc;
    struct usb_endpoint_descriptor* endpoint;
    int i;
    int retval = -ENODEV;

    dev = kzalloc(sizeof(*dev), GFP_KERNEL);

    if (!dev) {
        dev_err(&interface->dev, "Out of memory\n");
        retval = -ENOMEM;
        return retval;
    }

    dev->udev = interface_to_usbdev(interface);
    mutex_init(&dev->io_mutex);

    dev->interface = interface;
    // get interface description (current)
    iface_desc = interface->cur_altsetting;

    // get endpoints data
    for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i) {
        endpoint = &iface_desc->endpoint[i].desc;
        if (usb_endpoint_is_bulk_out(endpoint)) {
            dev->bulk_out_ep = endpoint;
            dev->bulk_out_packet_size = le16_to_cpu(endpoint->wMaxPacketSize);
            break;
        }
    }
    if (!dev->bulk_out_ep) {
        dev_err(&interface->dev, "Can not find bulk-out endpoint!\n");
        retval = -EIO;
        kfree(dev);
        return retval;
    }

    dev->videobuffer = kmalloc(TOTAL_BUFFER_SIZE, GFP_KERNEL);

    if (!dev->videobuffer) {
        dev_err(&interface->dev, "Out of memory\n");
        retval = -ENOMEM;
        kfree(dev);
        return retval;
    }
    // throuh kernel callbacks
    usb_set_intfdata(interface, dev);
    // registration device in /dev/beep
    retval = usb_register_dev(interface, &usbbeep_class);
    if (retval) {
        dev_err(&interface->dev, "Not able to get a minor for this device.");
        usb_set_intfdata(interface, NULL);
        kfree(dev->videobuffer);
        return retval;
    }
    dev->minor = interface->minor;
    dev_info(&interface->dev, "USB beep driver%d\n",
             dev->minor - BEEP_MINOR_BASE);

    printk(KERN_INFO "USB BEEP CONNECTED!\n");
    return retval;
}

void usbbeep_disconnect(struct usb_interface* interface) {

    struct usbbeep* dev;
    int minor;

    dev = usb_get_intfdata(interface);
    minor = dev->minor;
    usb_set_intfdata(interface, NULL); // to NULL
    usb_deregister_dev(interface, &usbbeep_class);

    mutex_lock(&dev->io_mutex);
    dev->interface = NULL;
    mutex_unlock(&dev->io_mutex);

    kfree(dev->videobuffer);
    kfree(dev);
    dev_info(&interface->dev, "BEEP STM32-based module%d disconnected\n",
             minor - BEEP_MINOR_BASE);
    printk(KERN_INFO "USB BEEP DISCONNECTED!\n");
}

static int usbbeep_open(struct inode* inode, struct file* file) {
    /* open syscall */
    struct usbbeep* dev;
    struct usb_interface* interface;
    int retval = 0;
    int subminor;
    // file->private_data = NULL;

    if (!atomic_dec_and_test(&DeviceFree)) {
        atomic_inc(&DeviceFree);
        return -EBUSY;
    }

    subminor = iminor(inode);
    interface = usb_find_interface(&usbbeep_driver, subminor);
    if (!interface) {
        printk(KERN_ERR "usblcd driver error, can't find device for minor %d\n",
               subminor);
        retval = -ENODEV;
        return retval;
    }

    dev = usb_get_intfdata(interface);
    if (!dev) {
        retval = -ENODEV;
        return retval;
    }

    mutex_lock(&dev->io_mutex);

    if (!dev->interface) {
        retval = -ENODEV;
        mutex_unlock(&dev->io_mutex);
        return retval;
    }

    file->private_data = dev;
    //dev_info(&interface->dev, "usbbeep: opened successfuly");

    mutex_unlock(&dev->io_mutex);
    //printk(KERN_INFO "USB BEEP OPENED!\n");
    return 0;
}

static int usbbeep_release(struct inode* inode, struct file* file) {
    /* close syscall */
    //printk(KERN_INFO "USB BEEP released!\n");
    atomic_inc(&DeviceFree);
    file->private_data = NULL;
    return 0;
}

static ssize_t usbbeep_write(struct file* file, const char __user* user_buf,
                             size_t count, loff_t* ppos) {
    /* write syscall */
    struct usbbeep* dev;
    struct usb_interface* interface;
    int retval = -ENODEV;

    int usbSent = 0;
    int writtenCount = count;

    dev = file->private_data;
    //////////  MUTEX   ///////////////////
    mutex_lock(&dev->io_mutex);

    interface = dev->interface;
    if (!interface) {
        printk(KERN_ERR "usbbeep driver error, no device found\n");
        retval = -ENODEV;
        mutex_unlock(&dev->io_mutex);
        return retval;
    }

    if (copy_from_user(dev->videobuffer, user_buf, writtenCount)) {
        retval = -EFAULT;
        mutex_unlock(&dev->io_mutex);
        return retval;
    }
    retval = usb_bulk_msg(dev->udev, usb_sndbulkpipe(dev->udev, 1),
                          dev->videobuffer, writtenCount, &usbSent, 1000);
    if (!retval) {
        retval = writtenCount;
        ppos += retval;
    } else {
        retval = -EIO;
        mutex_unlock(&dev->io_mutex);
        return retval;
    }
    //////////  MUTEX   ///////////////////
    mutex_unlock(&dev->io_mutex);
    return retval;
}

// static int __init beepdriver_init(void) {
//    printk(KERN_INFO "Hello, beep!\n");
//    return usb_register(&usbbeep_driver);
//}
//
// static void __exit beepdriver_exit(void) {
//    printk(KERN_INFO "Goodbye, beep!\n");
//    usb_deregister(&usbbeep_driver);
//}
//
// module_init(beepdriver_init);
// module_exit(beepdriver_exit);

MODULE_LICENSE("GPL");

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
