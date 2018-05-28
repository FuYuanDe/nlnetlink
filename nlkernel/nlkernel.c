/*
 *  Description : 内核netlink编程
 *  Date        ：20180528
 *  Author      ：mason
 *  Mail        : mrsonko@126.com
 *
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/netlink.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <net/ip.h>
#include "nlkernel.h"


static int number;
static struct sock *nlsock = NULL;  // netlink 套接字


// netlink消息回复，用于向应用层传递数据
static void netlink_sendto_userapp(unsigned int dst_pid) {
    struct sk_buff *skb = NULL;
    struct nlmsghdr *nlh = NULL;
    int datalen;
    int *pnum;
    datalen = NLMSG_SPACE(sizeof(int));

    skb = alloc_skb(datalen, GFP_KERNEL);
    if(!skb)
    {
        log("alloc skb error.\r\n");
        return ;
    }
    // 数据初始化
    nlh = nlmsg_put(skb, 0, 0, 0, sizeof(int), 0);
    nlh->nlmsg_pid = 0;
    nlh->nlmsg_type = NLKERNEL_GET;
    
    pnum = (int *)NLMSG_DATA(nlh);
    *pnum = number;    
    netlink_unicast(nlsock, skb, dst_pid, MSG_DONTWAIT);
    log("netlink send done \r\n");
    
    return;    
}


/* netlink消息处理函数 */
static int netlink_kernel_rcv_msg(struct sk_buff *skb, struct nlmsghdr *nlh)
{
    int *value = NULL;
    switch(nlh->nlmsg_type){
        // 设置
        case NLKERNEL_SET :
            log("kernel receive netlink set msg!\r\n");
            value = (int *)NLMSG_DATA(nlh);            
            number = *value;
            break;
        // 获取
        case NLKERNEL_GET :
            log("kernel receive netlink get msg!\r\n");
            netlink_sendto_userapp(nlh->nlmsg_pid);
            break;
        default:
            log("unrecognized netlink message type : %u \r\n",nlh->nlmsg_type);
            break;
    }   
    return 0;
}   


static void netlink_kernel_rcv(struct sk_buff *skb)
{
    int res;
	res = netlink_rcv_skb(skb, &netlink_kernel_rcv_msg);
    return;
}


// 模块入口函数
static int __init nlkernel_init(void) {
    log("nlkernel init \r\n");
    // 注册netlink协议
	nlsock = netlink_kernel_create(&init_net, NETLINK_TEST_MODULE, 0, netlink_kernel_rcv, NULL, THIS_MODULE);
    if (!nlsock) {
        log("netlink module init fail \r\n");
        return -1;
    }
    return 0;
}


// 模块退出函数
static void __exit nlkernel_exit(void) {
    // 注销netlink协议
	if(nlsock)
	{
        netlink_kernel_release(nlsock);
        nlsock = NULL;
	}
    log("nlkernel exit \r\n");
    return ;
}


module_init(nlkernel_init)
module_exit(nlkernel_exit)
MODULE_AUTHOR("mason");
MODULE_DESCRIPTION("netlink kernel test");
MODULE_LICENSE("GPL");

