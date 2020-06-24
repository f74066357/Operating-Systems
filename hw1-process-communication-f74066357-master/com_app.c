#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <linux/netlink.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#define NETLINK_TEST    30
#define MSG_LEN            256
#define MAX_PLOAD        1024

typedef struct _user_msg_info
{
    struct nlmsghdr hdr;
    char  msg[MSG_LEN];
} user_msg_info;
int main(int argc, char* argv[])
{
    //registration //Registration. id=[id], type=[type]
    char reg[50];
    char registration[15]="Registration.";
    char* s1=argv[1];
    char* s2=argv[2];
    char sp[1]=" ";
    char q[1]=",";
    char id[3]="id=";
    char type[6]="type=" ;
    strcpy(reg,registration);
    strncat(reg,sp,1);
    strncat(reg,id,3);
    strcat(reg,s1);
    strncat(reg,q,1);
    strncat(reg,sp,1);
    strncat(reg,type,5);
    strcat(reg,s2);
    int skfd;
    int ret;
    user_msg_info u_info;
    socklen_t len;
    struct nlmsghdr *nlh = NULL;
    struct sockaddr_nl saddr, daddr;

    char *umsg;
    umsg= reg;
    int port=atoi(argv[1]);

    /* 建立NETLINK socket */
    skfd = socket(AF_NETLINK, SOCK_RAW, NETLINK_TEST);
    memset(&saddr, 0, sizeof(saddr));
    saddr.nl_family = AF_NETLINK; //AF_NETLINK
    saddr.nl_pid = port;  //埠號(port ID)
    saddr.nl_groups = 0;

    //（綁定）
    if(bind(skfd, (struct sockaddr *)&saddr, sizeof(saddr)) != 0)
    {
        //perror("bind() error\n");
        printf("Fail\n");
        close(skfd);
        return -1;
    }
    memset(&daddr, 0, sizeof(daddr));
    daddr.nl_family = AF_NETLINK;
    daddr.nl_pid = 0; // to kernel
    daddr.nl_groups = 0;
    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PLOAD));
    memset(nlh, 0, sizeof(struct nlmsghdr));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PLOAD);
    nlh->nlmsg_flags = 0;
    nlh->nlmsg_type = 0;
    nlh->nlmsg_seq = 0;
    nlh->nlmsg_pid = saddr.nl_pid; //self port

    memcpy(NLMSG_DATA(nlh), umsg, MAX_PLOAD);//由src指向地址為起始地址的連續n個字節的數據複製到以destin指向地址為起始地址的空間內
//send
    ret = sendto(skfd, nlh, nlh->nlmsg_len, 0, (struct sockaddr *)&daddr, sizeof(struct sockaddr_nl));
    //printf("send kernel:%s\n", umsg);
    memset(&u_info, 0, sizeof(u_info));//用來對一段內存空間全部設置為某個字符，一般用在對定義的字符串進行初始化 把buffer所指內存區域的前count個字節設置成字符c
    len = sizeof(struct sockaddr_nl);
//recv
    ret = recvfrom(skfd, &u_info, sizeof(user_msg_info), 0, (struct sockaddr *)&daddr, &len);
    //printf("from kernel:%s\n", u_info.msg);
    umsg=0;
    char* ackfail;
    ackfail="Success";
    int success=strncmp(ackfail,u_info.msg,7);
    if(success!=0)
    {
        printf("Fail\n");
        close(skfd);
        free((void *)nlh);
        return -1;
    }
    else
    {
        printf("Success\n");
    }
    memset(&u_info,0,sizeof(u_info));
    len = sizeof(struct sockaddr_nl);
    char str[999999]= {'\0'};
    while(scanf("%[^\n]%*c",str))
    {
        //printf("Test:%s\n",u_info.msg);
        nlh=NULL;
        nlh=(struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PLOAD));
        memset(nlh, 0, sizeof(struct nlmsghdr));
        nlh->nlmsg_len = NLMSG_SPACE(MAX_PLOAD);
        nlh->nlmsg_flags = 0;
        nlh->nlmsg_type = 0;
        nlh->nlmsg_seq = 0;
        nlh->nlmsg_pid = saddr.nl_pid; //self port
        //printf("s:%s",str);
        int recv;
        recv = strncmp(str,"Recv",4);
        if(recv==0)
        {
            char rm[20]="";
            char sp[1]=" ";
            char end[1]="\0";
            strncpy(rm,str,4);
            strncat(rm,sp,1);
            strncat(rm,argv[1],strlen(argv[1]));
            strncat(rm,end,1);
            umsg=rm;
            memcpy(NLMSG_DATA(nlh),umsg, MAX_PLOAD);
            ret = sendto(skfd, nlh, nlh->nlmsg_len, 0, (struct sockaddr *)&daddr, sizeof(struct sockaddr_nl));
            memset(&u_info,0,sizeof(u_info));
            len = sizeof(struct sockaddr_nl);
            ret = recvfrom(skfd, &u_info, sizeof(user_msg_info), 0, (struct sockaddr *)&daddr, &len);
            printf("%s\n", u_info.msg);
        }
        else
        {
            char end[1]="\0";
            strncat(str,end,1);
            umsg=str;
            memcpy(NLMSG_DATA(nlh),umsg, MAX_PLOAD);
            ret = sendto(skfd, nlh, nlh->nlmsg_len, 0, (struct sockaddr *)&daddr, sizeof(struct sockaddr_nl));
            memset(&u_info,0,sizeof(u_info));
            len = sizeof(struct sockaddr_nl);
            //printf("Test3:%s\n",u_info.msg);
            ret = recvfrom(skfd, &u_info, sizeof(user_msg_info), 0, (struct sockaddr *)&daddr, &len);
            printf("%s\n", u_info.msg);
        }
        free((void *)nlh);
    }
    close(skfd);
    free((void *)nlh);
    return 0;
}