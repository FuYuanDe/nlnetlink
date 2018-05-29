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
    unsigned int len;
	struct iovec iov[1];
	struct msghdr msg;
	struct sockaddr_nl src, dst;  
	struct nlmsghdr *nlh, *nlh1, *nlh2 = NULL;

    
    nlfd = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_TEST_MODULE);
    if (nlfd == -1) {
        log("create netlink socket fail\n");
        return;
    }
    
    memset(&src, 0, sizeof(struct sockaddr_nl));
    memset(&dst, 0, sizeof(struct sockaddr_nl));
    memset(&msg, 0, sizeof(struct msghdr));   
    src.nl_family = AF_NETLINK;
    src.nl_pid = getpid(); 
    src.nl_groups = 0;  
    dst.nl_family = AF_NETLINK;
    dst.nl_pid = 0;    
    dst.nl_groups = 0;

    bind(nlfd, (struct sockaddr*)&src, sizeof(struct sockaddr_nl));
    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(int));
    memset(nlh, 0, NLMSG_SPACE(int));

    nlh->nlmsg_len = NLMSG_SPACE(int);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_type = NLKERNEL_GET;
    nlh->nlmsg_flags = NLM_F_REQUEST;
    iov[0].iov_base = (void *)nlh;
    iov[0].iov_len = nlh->nlmsg_len;
    msg.msg_name = (void *)&dst;
    msg.msg_namelen = sizeof(struct sockaddr_nl);
    msg.msg_iov = &iov[0];
    msg.msg_iovlen = 1;
    log("send netlink_get msg to kernel \r\n");
    sendmsg(nlfd, &msg, 0);

    nlh1 = (struct nlmsghdr *)malloc(NLMSG_SPACE(sizeof(int)));        
    memset(nlh1, 0, NLMSG_SPACE(sizeof(int)));
    nlh1->nlmsg_len = NLMSG_SPACE(sizeof(int));
    iov[0].iov_base = (void *)nlh1;
    iov[0].iov_len = nlh1->nlmsg_len;
    msg.msg_iov = &iov[0];
    msg.msg_iovlen = 1;    

    len = recvmsg(nlfd, &msg, 0);
    value = (int *)NLMSG_DATA(nlh1);
    log("receive kernel msg : %d \r\n", *value);

    nlh2 = (struct nlmsghdr *)malloc(NLMSG_SPACE(sizeof(int)));        
    memset(nlh2, 0, NLMSG_SPACE(sizeof(int)));
    nlh2->nlmsg_len = NLMSG_SPACE(sizeof(int));
    nlh2->nlmsg_pid = getpid();
    nlh2->nlmsg_type = NLKERNEL_SET;
    nlh2->nlmsg_flags = NLM_F_REQUEST;
    iov[0].iov_base = (void *)nlh2;
    iov[0].iov_len = nlh2->nlmsg_len;
    value = (int *)NLMSG_DATA(nlh2);
    *value = 30;
    msg.msg_name = (void *)&dst;
    msg.msg_namelen = sizeof(struct sockaddr_nl);
    msg.msg_iov = &iov[0];
    msg.msg_iovlen = 1;
    log("send netlink_set msg to kernel \r\n");
    sendmsg(nlfd, &msg, 0);    

    memset(nlh1, 0, NLMSG_SPACE(sizeof(int)));
    nlh1->nlmsg_len = NLMSG_SPACE(sizeof(int));
    nlh1->nlmsg_pid = getpid();
    nlh1->nlmsg_type = NLKERNEL_GET;
    nlh1->nlmsg_flags = NLM_F_REQUEST;
    iov[0].iov_base = (void *)nlh1;
    iov[0].iov_len = nlh1->nlmsg_len;
    msg.msg_iov = &iov[0];
    msg.msg_iovlen = 1;    
    msg.msg_name = (void *)&dst;
    msg.msg_namelen = sizeof(struct sockaddr_nl);
    log("send netlink_get msg to kernel \r\n");
    sendmsg(nlfd, &msg, 0);

    memset(nlh1, 0, NLMSG_SPACE(sizeof(int)));
    nlh1->nlmsg_len = NLMSG_SPACE(sizeof(int));
    iov[0].iov_base = (void *)nlh1;
    iov[0].iov_len = nlh1->nlmsg_len;
    msg.msg_iov = &iov[0];
    msg.msg_iovlen = 1;  

    len = recvmsg(nlfd, &msg, 0);
    value = (int *)NLMSG_DATA(nlh1);
    log("receive kernel msg : %d \r\n", *value);


    close(nlfd);
    if (nlh)
        free(nlh);
    if (nlh1)
        free(nlh1);
    if (nlh2)
        free(nlh2);
        

    return ;
    
}
