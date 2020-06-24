#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
void* child(void *arg);
struct mypara
{
    char str[999];
    int  portnum;
    char localhost[999];
};
int main(int argc, char *argv[])
{
    char localhost[999]= {};
    strcpy(localhost,argv[2]);
    char* port=argv[4];
    int portnum = atoi(port);
    char str[99999]= {};
    while(scanf("%[^\n]%*c",str))//Query "test" "abc"
    {
        pthread_t t;
        int cmp1,cmp2=100;
        cmp1=strncmp("Query \"",str,7);
        int x=strlen(str);
        char temp;
        temp=str[x-1];
        if(temp=='"')
        {
            cmp2=0;
        }
        if(cmp1!=0||cmp2!=0)
        {
            printf("the strings format is not correct\n");
        }
        else
        {
            struct mypara pstru;
            pstru.portnum=portnum;
            strcpy(pstru.str,str);
            strcpy(pstru.localhost,&localhost);
            pthread_create(&t, NULL, child,&(pstru));
        }
        //pthread_join(t, NULL);
    }
    return 0;
}

void* child(void *arg)
{
    struct mypara *pstru;
    pstru = arg;
    //printf("%s\n",pstru->str);
    int a=0;
    int recvnum=0;
    char check[999] ;
    char delim[] = "\"";
    char *token;
    strcpy(check,pstru->str);
    for(token = strtok(check, delim); token != NULL; token = strtok(NULL, delim))
    {
        a++;
        if(a%2==0)
        {
            recvnum++;
        }
    }
    //printf("%d\n",recvnum);

    //socket的建立
    int sockfd = 0;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    //printf("%d\n",sockfd);
    if (sockfd == -1)
    {
        printf("Fail to create a socket.");
    }
    //socket的連線
    struct sockaddr_in info;
    bzero(&info,sizeof(info));
    info.sin_family = PF_INET;
    //localhost test
    info.sin_addr.s_addr = inet_addr(pstru->localhost);
    info.sin_port = htons(pstru->portnum);
    int err = connect(sockfd,(struct sockaddr *)&info,sizeof(info));
    if(err==-1)
    {
        printf("Connection error");
    }
    //Send a message to server
    char message[999] ;
    strcpy(message,pstru->str);
    char receiveMessage[999] = {};
    int q;
    send(sockfd,message,sizeof(message),0);

    int k;
    for (k=0; k<recvnum; k++)
    {
        //printf("hehe\n");
        recv(sockfd,receiveMessage,sizeof(receiveMessage),0);
        printf("%s",receiveMessage);
        //printf("nono\n");
    }
    //printf("close Socket\n");
    //close(sockfd);
    pthread_exit(NULL); // 離開子執行緒
}