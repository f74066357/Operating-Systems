#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/string.h>
#include "com_kmodule.h"
#define NETLINK_TEST     30
#define MSG_LEN            256
//#define USER_PORT        100

struct sock *nlsk = NULL;
extern struct net init_net;
int port=0;
struct mailbox *mail;
//send
int send_usrmsg(char *pbuf, uint16_t len)
{
    struct sk_buff *nl_skb;
    struct nlmsghdr *nlh;
    int ret;
    /* 建立sk_buff 空間 */
    nl_skb = nlmsg_new(len, GFP_ATOMIC);
    if(!nl_skb)
    {
        printk("netlink alloc failure\n");
        return -1;
    }
    /* 設定netlink訊息頭部 */
    nlh = nlmsg_put(nl_skb, 0, 0, NETLINK_TEST, len, 0);
    if(nlh == NULL)
    {
        printk("nlmsg_put failaure \n");
        nlmsg_free(nl_skb);
        return -1;
    }
    /* 拷貝資料傳送 */
    memcpy(nlmsg_data(nlh), pbuf, len);
    ret = netlink_unicast(nlsk, nl_skb,port, MSG_DONTWAIT);
    return ret;
}
//recv
static void netlink_rcv_msg(struct sk_buff *skb)
{
    struct nlmsghdr *nlh = NULL;
    char *umsg = NULL;
    char *kmsg = NULL;

    if(skb->len >= nlmsg_total_size(0))
    {
        nlh = nlmsg_hdr(skb);
        umsg = NLMSG_DATA(nlh);
        if(umsg)
        {
            kmsg="Fail";//default
            port=nlh->nlmsg_pid;
            printk("k: %s\n",umsg);
            char* delim;
            delim="\r";
            char *token, *cur = umsg;
            token = strsep(&cur, delim);
            int send;
            int recv;
            int regi;
            send = strncmp(umsg,"Send",4);
            recv = strncmp(umsg,"Recv",4);
            regi = strncmp(umsg,"Regi",4);
            if(send==0&&recv!=0) //if send//Send [id] [message data]
            {
                printk("it is a send");
                char* ids;
                int id=0;
                char* content;
                char* delim;
                delim=" ";
                char* end;
                char *token, *cur = umsg;
                token = strsep(&cur, delim);
                token = strsep(&cur, delim);
                //printk("%s\n",token);//y id
                ids=token;
                id=simple_strtol(ids,NULL,10);
                printk("%d\n",id);
                //printk("%s\n",cur);//y content
                content=cur;
                if(id>1000||id<0)
                {
                    kmsg="Fail";
                }
                else
                {
                    if(strlen(content)>255)
                    {
                        printk("out of range");
                        kmsg="Fail.Message out of range";
                    }
                    else
                    {
                        if(mail[id].type=='0') //unqueued
                        {
                            if( mail[id].msg_data_count==0)
                            {
                                printk("unqueued");
                                struct  msg_data *message;
                                message=kmalloc(sizeof(struct msg_data),GFP_KERNEL);
                                mail[id].msg_data_head=message;
                                strcpy(mail[id].msg_data_head->buf,content);
                                //mail[id].msg_data_head->next=NULL;
                                mail[id].msg_data_count++;
                                kmsg="Success";
                            }
                            else
                            {
                                strcpy(mail[id].msg_data_head->buf,content);
                                mail[id].msg_data_count++;
                                kmsg="Success";
                            }
                        }
                        else if(mail[id].type=='1') //queued
                        {
                            printk("queued");
                            if(mail[id].msg_data_count==0)
                            {
                                struct  msg_data *message;
                                message=kmalloc(sizeof(struct msg_data),GFP_KERNEL);
                                mail[id].msg_data_head=message;
                                strcpy(mail[id].msg_data_head->buf,content);
                                mail[id].msg_data_head->next=NULL;
                                mail[id].msg_data_tail=mail[id].msg_data_head;
                                mail[id].msg_data_count++;
                                kmsg="Success";
                            }
                            else if((mail[id].msg_data_count==1)||mail[id].msg_data_count==2)
                            {
                                struct  msg_data *message;
                                message=kmalloc(sizeof(struct msg_data),GFP_KERNEL);
                                strcpy(message->buf,content);
                                mail[id].msg_data_tail->next=message;
                                mail[id].msg_data_tail=message;
                                mail[id].msg_data_count++;
                                kmsg="Success";
                            }
                            else //message data is lost
                            {
                                //do nothing
                                kmsg="Fail";
                            }
                            //printk()
                        }
                        else //z
                        {
                            printk("here");
                            kmsg="Fail";
                        }
                        //printk("%s\n",mail[id].msg_data_head->buf);
                        //printk("len:%d\n",strlen(content));
                    }
                }
            }
            else if(recv==0&&send!=0) //if recv//Recv [id]
            {
                printk("it is a recv");
                char s[20];
                strncpy(s,umsg,20);
                char* ids;
                int id=0;
                char* delim;
                delim= " ";
                char *token, *cur = s;
                token = strsep(&cur, delim);
                delim= "\n";
                token = strsep(&cur, delim);
                ids=token;
                printk("%s\n",token);//y id
                id=simple_strtol(ids,NULL,10);
                printk("%d\n",id);//y id
                if(mail[id].type=='0') //unqueued
                {
                    printk("%d\n",mail[id].msg_data_count);
                    if(mail[id].msg_data_count==0)
                    {
                        printk("cccc");
                        kmsg="Fail";
                    }
                    else
                    {
                        printk("hoho");
                        kmsg=mail[id].msg_data_head->buf;
                    }

                }
                else if(mail[id].type=='1') //queued
                {

                    //empty-> does not provide any message data
                    if(mail[id].msg_data_head==NULL)
                    {
                        kmsg="Fail";
                    }
                    else //not empty-> provides the application with the oldest message data
                    {
                        //and removes this message data from the queue
                        if(mail[id].msg_data_count!=0)
                        {
                            kmsg=mail[id].msg_data_head->buf;
                            printk("%s\n",mail[id].msg_data_head->buf);
                            mail[id].msg_data_head=mail[id].msg_data_head->next;
                            mail[id].msg_data_count--;
                            printk("%d\n",mail[id].msg_data_count);
                        }
                        else
                        {
                            kmsg="Fail";
                        }
                    }
                }
                else //z
                {
                    kmsg="Fail";
                }

            }
            else if(recv!=0&&send!=0&&regi==0)//if registration//Registration. id=2, type=unqueued
            {
                printk("it is a registration");
                char s[40];
                char* ids;
                int id=0;
                char* qtype;
                char* delim;
                delim= "=";
                strncpy(s,umsg,40);
                char *token, *cur = s;
                token = strsep(&cur, delim);
                delim= ",";
                token = strsep(&cur, delim);
                ids=token;
                delim= "=";
                token = strsep(&cur, delim);
                qtype=cur;
                id=simple_strtol(ids,NULL,10);
                printk("%d\n",id);
                if((id>1000)||(id<1))
                {
                    printk("um....");
                    kmsg="Fail";
                }
                else
                {
                    //allocate a mailbox
                    if((mail[id].type!='z'))//or id has been used
                    {
                        kmsg="Fail";
                    }
                    else
                    {
                        int q=strncmp(qtype,"queued",6);
                        if(q==0)//queued
                        {
                            mail[id].type='1';
                            //mail[id].msg_data_count=0;
                            //mail[id].msg_data_head=NULL;
                            //mail[id].msg_data_tail=NULL;
                            //printk("type:%c\n",mail[id].type);
                            kmsg="Success";
                        }
                        else//unqueued
                        {
                            mail[id].type='0';
                            //mail[id].msg_data_count=0;
                            //mail[id].msg_data_head=NULL;
                            //mail[id].msg_data_tail=NULL;
                            printk("type:%c\n",mail[id].type);
                            kmsg="Success";
                        }
                    }
                }
            }
            else
            {
                printk("ummmm?");
                kmsg="Fail";
            }
            send_usrmsg(kmsg, strlen(kmsg));
            printk("kernel send:%s",kmsg);
        }
    }
}
struct netlink_kernel_cfg cfg =
{
    .input  = netlink_rcv_msg, /* set recv callback */
};
static int __init com_kmodule_init(void)
{
    /* create netlink socket */
    nlsk = (struct sock *)netlink_kernel_create(&init_net, NETLINK_TEST, &cfg);
    if(nlsk == NULL)
    {
        printk("netlink_kernel_create error !\n");
        return -1;
    }
    printk(KERN_INFO "Enter module. Hello world!\n");
    mail=kmalloc(1001*sizeof(struct mailbox),GFP_KERNEL);
    int i;
    for(i=0; i<1001; i++)
    {
        mail[i].type='z';
        mail[i].msg_data_count=0;
        mail[i].msg_data_head=NULL;
        mail[i].msg_data_tail=NULL;
        //mail[i].msg_data_head=(struct  msg_data*)kmalloc(sizeof(struct msg_data),GFP_KERNEL);
        //mail[i].msg_data_tail=(struct  msg_data*)kmalloc(sizeof(struct msg_data),GFP_KERNEL);
    }
    printk("type:%c\n",mail[i].type);
    printk("com_kmodule_init\n");
    return 0;
}
static void __exit com_kmodule_exit(void)
{
    if (nlsk)
    {
        netlink_kernel_release(nlsk); /* release ..*/
        nlsk = NULL;
    }
    printk("com_kmodule_exit!\n");
    printk(KERN_INFO "Exit module. Bye~\n");
}
module_init(com_kmodule_init);
module_exit(com_kmodule_exit);