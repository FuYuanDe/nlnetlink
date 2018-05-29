/*
 *  Description : 应用层netlink编程
 *  Date        ：20180529
 *  Author      ：mason
 *  Mail        : mrsonko@126.com
 *
 */
#include <linux/netlink.h>
#include <linux/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "nlclient.h"
void main() {
    int nlfd;
    int *value;
    int opt, arg;
    unsigned int len;
	struct iovec iov[1];
	struct msghdr msg;
	struct sockaddr_nl src, dst;  
	struct nlmsghdr *nlh, *nlh1, *nlh2 = NULL;

    // 创建netlink套接字
    nlfd = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_TEST_MODULE);
    if (nlfd == -1) {
        log("create netlink socket fail\n");
        return;
    }
    
    memset(&src, 0, sizeof(struct sockaddr_nl));
    memset(&dst, 0, sizeof(struct sockaddr_nl));
    memset(&msg, 0, sizeof(struct msghdr)); 

    // 设置本地地址
    src.nl_family = AF_NETLINK; 
    src.nl_pid = getpid(); 
    src.nl_groups = 0;  

    // 设置内核netlink地址
    dst.nl_family = AF_NETLINK;
    dst.nl_pid = 0;    
    dst.nl_groups = 0;

    // 绑定本地地址
    bind(nlfd, (struct sockaddr*)&src, sizeof(struct sockaddr_nl));

    // 申请netlink消息头域
    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(int));
    while(1) {
        log("input option :\n"
            "1 : set \n"
            "2 : get \n"
            "default : quit\n");
        scanf("%d", &opt);
        switch (opt) {
            // set 
            case 1 :     
                log("input value to set:");
                scanf("%d", &arg);        
                memset(nlh, 0, NLMSG_SPACE(sizeof(int)));
                nlh->nlmsg_len = NLMSG_SPACE(sizeof(int));

                // 设置netlink 应用层的pid
                nlh->nlmsg_pid = getpid();

                // 设置消息类型
                nlh->nlmsg_type = NLKERNEL_SET;

                // 设置标志位
                nlh->nlmsg_flags = NLM_F_REQUEST;

                // 填充发送消息结构体
                iov[0].iov_base = (void *)nlh;
                iov[0].iov_len = nlh->nlmsg_len;
                value = (int *)NLMSG_DATA(nlh);
                *value = arg;
                msg.msg_name = (void *)&dst;
                msg.msg_namelen = sizeof(struct sockaddr_nl);
                msg.msg_iov = &iov[0];
                msg.msg_iovlen = 1;

                // 发送netlink 消息给内核
                sendmsg(nlfd, &msg, 0);
                log("send set msg to kernel success \n\n");
                break;
                
            // get
            case 2 :  
                memset(nlh, 0, NLMSG_SPACE(int));
                nlh->nlmsg_len = NLMSG_SPACE(int);
                nlh->nlmsg_pid = getpid();

                // 设置netlink消息类型
                nlh->nlmsg_type = NLKERNEL_GET;
                nlh->nlmsg_flags = NLM_F_REQUEST;
                iov[0].iov_base = (void *)nlh;
                iov[0].iov_len = nlh->nlmsg_len;
                msg.msg_name = (void *)&dst;
                msg.msg_namelen = sizeof(struct sockaddr_nl);
                msg.msg_iov = &iov[0];
                msg.msg_iovlen = 1;

                // 发送消息
                log("send get msg to kernel success \n");
                sendmsg(nlfd, &msg, 0);

                memset(nlh, 0, NLMSG_SPACE(sizeof(int)));
                nlh->nlmsg_len = NLMSG_SPACE(sizeof(int));
                iov[0].iov_base = (void *)nlh;
                iov[0].iov_len = nlh->nlmsg_len;
                msg.msg_iov = &iov[0];
                msg.msg_iovlen = 1;    

                // 接收netlink内核端消息
                len = recvmsg(nlfd, &msg, 0);
                value = (int *)NLMSG_DATA(nlh);
                log("kernel return : %d \r\n\n", *value);
                
            break;
            default :
                goto end;
        }
    }

 end:         
    close(nlfd);
    if (nlh)
        free(nlh);          

    return ; 
}
