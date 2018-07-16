#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/net.h>
#include <linux/types.h>
#include <linux/skbuff.h>
#include <linux/string.h>
#include <linux/netdevice.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/byteorder/generic.h> // To check the destination IP address, we need to convert it to string format


#define  DEVICE_NAME "hook_module"
#define  CLASS_NAME  "Yizhang"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yizhang Cao");
MODULE_DESCRIPTION("Packet Filtering hooks");

static ssize_t write_to_module(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops = {
   .write = write_to_module,
};

static int majorNumber;
// data from userspace
static char message[256] = {0};
static char IP_ADDR[100][16] = {"0.0.0.0"};
static char emptyArray[100][16] = {"0.0.0.0"};
// its size
static short size_of_message;
static struct class* driverClass  = NULL;
// driver device struct pointer
static struct device* driverDevice = NULL;

static struct nf_hook_ops local_out_nfho;
// Net filter used for local-out
struct sk_buff *sock_buff;
struct iphdr *ip_header;
// IP Header Structure

static int counter = 0;

unsigned int handle_inbound_packet(unsigned int hooknum, struct sk_buff **skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *))
{
  int i;
  sock_buff = skb;
  ip_header = (struct iphdr *)skb_network_header(sock_buff); // Grab network header using accessor
  if(!sock_buff){
    return NF_ACCEPT;
  }
  int size = 16;
  char* str[size];
  snprintf(str, size, "%pI4", &ip_header->daddr);
  printk("Entered %s \n\n", str);

  for(i = 0; IP_ADDR[i][0] != NULL; i++){
    if(!strcmp(str, IP_ADDR[i])){
      // Compare the inbound packet's IP to the restrited IP Array
        return NF_DROP;
    }
  }
  return NF_ACCEPT;

}

static ssize_t write_to_module(struct file *filep, const char *buffer, size_t len, loff_t *offset) {


   // append received string and its length
   sprintf(message, "%s", buffer, (int)len);
   // store length of message

     if (message != 0 && strcmp(message,"r") != 0){
       strcpy(IP_ADDR[counter], message);

       printk(KERN_INFO "mydriver: Update Module Restricted IP address to %s\n", IP_ADDR[counter]);
       counter++;
       // size_of_message = strlen(message);
       printk(KERN_INFO "mydriver: received %d characters from the user\n", (int)len);
     }
     if(strcmp(message,"r") == 0){
       memcpy(IP_ADDR, emptyArray, sizeof(emptyArray));
       counter = 0;
     }
   return len;
}

static int __init mydriver_init(void) {
  printk(KERN_INFO "mydriver: initializing\n");

  local_out_nfho.hook = handle_inbound_packet;
  local_out_nfho.hooknum = NF_INET_LOCAL_OUT; // Local-out hook
  local_out_nfho.pf = PF_INET; // IPv4 protocol hook
  local_out_nfho.priority = NF_IP_PRI_FIRST; // Hook to come first
  nf_register_hook(&local_out_nfho); // Registering the Local-out hook

  majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
  if (majorNumber<0) {
    printk(KERN_ALERT "mydriver: failed to allocate major number\n");
    return majorNumber;
  }
  printk(KERN_INFO "mydriver: registered with major number %d\n", majorNumber);
  // register device class
  driverClass = class_create(THIS_MODULE, CLASS_NAME);
  if (IS_ERR(driverClass)) {
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_ALERT "mydriver: failed to register device class\n");
    return PTR_ERR(driverClass);
  }
  printk(KERN_INFO "mydriver: device class registered\n");
   // register device driver
  driverDevice = device_create(driverClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
  if (IS_ERR(driverDevice)) {
// if error, cClean up
    class_destroy(driverClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_ALERT "mydriver: failed to create device\n"); return PTR_ERR(driverDevice);
  }

  printk(KERN_INFO "mydriver: device class created\n");
  return 0;
}

/* Cleanup */
static void __exit mydriver_exit(void) {
   device_destroy(driverClass, MKDEV(majorNumber, 0));
   // unregister device class
   class_unregister(driverClass);
   // remove device class
   class_destroy(driverClass);
   // unregister major number
   unregister_chrdev(majorNumber, DEVICE_NAME);
   nf_unregister_hook(&local_out_nfho); // Removing Local-out IPv4 Hook
   printk(KERN_INFO "mydriver: closed\n");
}

module_init(mydriver_init);
module_exit(mydriver_exit);
