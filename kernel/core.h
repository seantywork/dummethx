#ifndef _DUMMETH_X_H_ 
#define _DUMMETH_X_H_ 



#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

#include <linux/sched.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/errno.h>  /* error codes */
#include <linux/types.h>  /* size_t */
#include <linux/interrupt.h> 

#include <linux/in.h>
#include <linux/netdevice.h>   /* struct device, and other headers */
#include <linux/etherdevice.h> /* eth_type_trans */
#include <linux/ip.h>          /* struct iphdr */
#include <linux/tcp.h>         /* struct tcphdr */
#include <linux/skbuff.h>
#include <linux/version.h> 

#include <linux/in6.h>
#include <asm/checksum.h>

#define DRV_NAME	"dummethx"
#define DRV_COUNT   2


#define DUMMETHX_RX_INTR 0x0001
#define DUMMETHX_TX_INTR 0x0002

/* Default timeout period */
#define DUMMETHX_TIMEOUT 5   /* In jiffies */



struct dummethx_packet {
	struct dummethx_packet *next;
	struct net_device *dev;
	int	datalen;
	u8 data[ETH_DATA_LEN];
};

struct dummethx_priv {
    struct net_device_stats stats;
    int status;
    struct dummethx_packet *ppool;
    struct dummethx_packet *rx_queue; /* List of incoming packets */
    int rx_int_enabled;
    int tx_packetlen;
    u8 *tx_packetdata;
    struct sk_buff *skb;
    spinlock_t lock;
	struct net_device *dev;
	struct napi_struct napi;
};


extern struct net_device *dummethx_devs[DRV_COUNT];
extern struct dummethx_priv *dummethx_privs[DRV_COUNT];
extern int setup_ptr;

extern const struct net_device_ops dummethx_netdev_ops;

extern const struct header_ops dummethx_header_ops;

extern int lockup;

extern int timeout;

extern int use_napi;

extern int pool_size;

extern void (*dummethx_interrupt)(int, void *, struct pt_regs *);

/* util functions */

void dummethx_setup_pool(struct net_device *dev);

void dummethx_teardown_pool(struct net_device *dev);

struct dummethx_packet *dummethx_get_tx_buffer(struct net_device *dev);

void dummethx_release_buffer(struct dummethx_packet *pkt);

void dummethx_enqueue_buf(struct net_device *dev, struct dummethx_packet *pkt);

struct dummethx_packet *dummethx_dequeue_buf(struct net_device *dev);


/* rx, interrupt functions */

void dummethx_rx_ints(struct net_device *dev, int enable);

void dummethx_rx(struct net_device *dev, struct dummethx_packet *pkt);

int dummethx_poll(struct napi_struct *napi, int budget);

void dummethx_regular_interrupt(int irq, void *dev_id, struct pt_regs *regs);

void dummethx_napi_interrupt(int irq, void *dev_id, struct pt_regs *regs);

/* device specific hw tx functions */

void dummethx_hw_tx(char *buf, int len, struct net_device *dev);


/* header and etc */

int dummethx_rebuild_header(struct sk_buff *skb);

int dummethx_header(struct sk_buff *skb, struct net_device *dev,
                unsigned short type, const void *daddr, const void *saddr,
                unsigned len);

int dummethx_change_mtu(struct net_device *dev, int new_mtu);


/* netdev */

int dummethx_open(struct net_device *dev);

int dummethx_stop(struct net_device *dev);

int dummethx_set_config(struct net_device *dev, struct ifmap *map);

netdev_tx_t dummethx_xmit(struct sk_buff *skb, struct net_device *dev);

int dummethx_do_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd);

//int dummethx_get_stats(struct net_device *dev);

struct net_device_stats* dummethx_get_stats(struct net_device *dev);

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,6,0)

void dummethx_tx_timeout(struct net_device *dev);

#else 

void dummethx_tx_timeout(struct net_device *dev, unsigned int txqueue);

#endif 


/* module entry */

void dummethx_setup(struct net_device *dev);

#endif