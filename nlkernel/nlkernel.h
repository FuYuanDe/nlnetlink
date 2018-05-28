#ifndef __NLKERNEL_H__
#define __NLKERNEL_H__
#define log(fmt, arg...)  printk(KERN_INFO"[bfd] %s:%d "fmt, __FUNCTION__, __LINE__, ##arg)

#ifndef NIPQUAD
#define NIPQUAD(addr) \
	((unsigned char *)&addr)[0], \
	((unsigned char *)&addr)[1], \
	((unsigned char *)&addr)[2], \
	((unsigned char *)&addr)[3]
#endif

#define NETLINK_TEST_MODULE         17      /* 抓包 netlink 协议 */

typedef enum netlink_msg_type {
    NLKERNEL_GET = NLMSG_MIN_TYPE +1,        /* value : 17 */
    NLKERNEL_SET,                            /* value : 18 */

    NLKERNEL_END,
}NETLINK_MSG_TYPE;

#endif