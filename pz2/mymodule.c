#include <linux/init.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/time.h>

#define DEVICE_NAME "mydevice"
#define SUCCESS 0
#define BUF_SIZE 10

static int Major;		/* Major number assigned to our device driver */
static int Device_Open = 0;

typedef struct {
    char msg[50];
    struct timespec ts;
} msg;

typedef union {
    int msg_num;
    msg message;
}msgr;

#define CAP_BUFFER _IOR('a','a', int *)
#define MESSAGE_GET _IOWR('a','b', msgr *)

typedef struct {
    unsigned int begin;
    unsigned int end;
    int capacity;
    msg buffer[BUF_SIZE];
}circle_buffer;

static circle_buffer buffer;

static void put_in_buffer(const char *buff, size_t len){
    if (len > 49){
        len = 49;
    }
    if(copy_from_user(buffer.buffer[buffer.end].msg, buff, len)){
        return;
    }
    buffer.buffer[buffer.end].msg[len] ='\0';
    getnstimeofday(&buffer.buffer[buffer.end].ts);

    if (buffer.capacity < BUF_SIZE)
        buffer.capacity++;

    buffer.end = (buffer.end + 1) % BUF_SIZE;
    if (buffer.end == buffer.begin)
        buffer.begin = (buffer.begin + 1) % BUF_SIZE;
}

static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t * off){
    printk(KERN_ALERT "Writing msg in buffer.\n");
    put_in_buffer(buff, len);
    return len;
}

static long device_ioctl (struct file *file, unsigned int cmd, unsigned long arg){
    msgr req;
    int num;
    printk(KERN_INFO "IOCTL = %i\n", cmd);
    switch(cmd) {
        case CAP_BUFFER:
            printk(KERN_INFO "Send cap to user: %d (%ld bytes)\n", buffer.capacity, sizeof(buffer.capacity));
            copy_to_user((int*) arg, &buffer.capacity, sizeof(buffer.capacity));
            break;
        case MESSAGE_GET:
            copy_from_user(&req, (int*) arg, sizeof(req));
            num = (buffer.begin + req.msg_num) % BUF_SIZE;
            printk(KERN_INFO "request to send msg with num %d\n", num);
            req.message = buffer.buffer[num];
            copy_to_user( (int*) arg, &req, sizeof(req));
            break;
    }
    return 0;
}

static int device_open(struct inode *inode, struct file *file){
    if (Device_Open)
        return -EBUSY;

    printk(KERN_ALERT "Dev open\n");
    Device_Open++;
    try_module_get(THIS_MODULE);
    return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file){
    Device_Open--;
    printk(KERN_ALERT "Dev close\n");
    module_put(THIS_MODULE);
    return 0;
}

static struct file_operations fops = {
        // we dont have read bsc we may not have it
        .write = device_write,
        .open = device_open,
        .release = device_release,
        .unlocked_ioctl = device_ioctl
};

static int mod_init(void){
    // it is 1st task
    printk(KERN_ALERT "Hello !!!\n");
    buffer.capacity = 0;
    buffer.begin = 0;
    buffer.end = 0;

    // task 2..3
    Major = register_chrdev(0, DEVICE_NAME, &fops);

    if (Major < 0) {
        printk(KERN_ALERT "Registering char device failed with %d\n", Major);
        return Major;
    }

    printk(KERN_INFO "I was assigned major number %d.\n", Major);
    return 0;
}

static void mod_exit(void){
    // it is 1st task
    printk(KERN_ALERT "Bye !!!\n");
    // task 2..3
    unregister_chrdev(Major, DEVICE_NAME);
}

module_init(mod_init);
module_exit(mod_exit);
