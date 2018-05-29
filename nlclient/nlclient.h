#ifndef _NLCLIENT_H
#define _NLCLIENT_H
#define log(fmt,arg...) \
	printf("[nlclient] "fmt,##arg)

#define NETLINK_TEST_MODULE         17      /* 抓包 netlink 协议 */

typedef enum netlink_msg_type {
    NLKERNEL_GET = NLMSG_MIN_TYPE +1,        /* value : 17 */
    NLKERNEL_SET,                            /* value : 18 */

    NLKERNEL_END,
}NETLINK_MSG_TYPE;


#endif

