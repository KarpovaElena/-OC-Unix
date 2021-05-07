#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h> /* printk */
#include <linux/slab.h> /* kmalloc */
#include <linux/types.h>
#include <linux/netdevice.h> /* struct device */
#include <linux/etherdevice.h>
#include <linux/skbuff.h>

MODULE_LICENSE("GPL");

struct net_device *netdev;
typedef struct {
    
}driver_data;

int driver_stop( struct net_device *dev )
{
    
    netif_stop_queue( netdev );
    printk("Driver stopped");
    return 0; //SUCCESS
}

int driver_open( struct net_device *dev )
{
    
    memcpy(dev->dev_addr,"ASDDDD",ETH_ALEN); // setting mac to device, 6bytes
    netif_start_queue( netdev );
    printk("Driver opened");
    return 0; //SUCCESS
}

int driver_hard_start_xmit( struct sk_buff *skb, struct net_device *dev )
{
    // dev->trans_start = jiffies; dont working in kernel
    netif_trans_update(dev); // set cur time
    dev->stats.tx_packets += 1; // recv 1 pkg
    dev->stats.tx_bytes += skb->len; // add pkg size
    dev_kfree_skb( skb );
    printk("Got pkg with size: %d", skb->len);
    return 0; //SUCCESS
}

static const struct net_device_ops netdev_ops = {
        .ndo_open = driver_open,
        .ndo_stop = driver_stop,
        .ndo_start_xmit = driver_hard_start_xmit,
};

static void mod_exit( void )
{
    unregister_netdev( netdev );
    free_netdev( netdev );
    printk("Exited from module, clear resources");
}

int mod_init (void)
{
    int t;
    netdev = alloc_etherdev(sizeof(driver_data));
    netdev->netdev_ops = &netdev_ops;
    strcpy (netdev->name, "MyDriver");
    if ((t = register_netdev( netdev ))){
        printk("Error while registering net device %d", t);
        return t;
    }
    printk("Netdriver registred");
    return 0;
}

module_init(mod_init);
module_exit(mod_exit);
