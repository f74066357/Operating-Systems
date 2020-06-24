#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define BUFSIZE 300
#define THREAD 64
#define QUEUE  100
void* child(void *arg);
struct mypara
{
    int forClientSockfd;
    //char string[999];
    char path[999];
};
typedef enum
{
    immediate_shutdown = 1,
    graceful_shutdown  = 2
} threadpool_shutdown_t;
typedef struct
{
    void (*function)(void *);
    void *argument;
} threadpool_task_t;
struct threadpool_t
{
    pthread_mutex_t lock;
    pthread_cond_t notify;
    pthread_t *threads;
    threadpool_task_t *queue;
    int thread_count;
    int queue_size;
    int head;
    int tail;
    int count;
    int shutdown;
    int started;
};
void do_task(void *arg) ;
static void *threadpool_thread(void *threadpool);
int threadpool_free(threadpool_t *pool);
int tasks = 0, done = 0;
pthread_mutex_t lock;
void error (char *msg);
void* query(char* mes,char* inputBuffer,char* path);
int countOccurrences(char * str, char * toSearch);

int main(int argc, char *argv[]) //./server -r testdir -p 12345 -n 4
{
    char path[999];
    strcpy(path,argv[2]);
    //strncat(path,"\0",1);
    //printf("%s\n%d\n",argv[2],strlen(argv[2]));
    //printf("%s\n",path);
    char* port=argv[4];
    int portnum = atoi(port);
    char* threadnum=argv[6];
    int threadi=atoi(threadnum);
    //static char mes[999]= {};
    //pool
    threadpool_t *pool;
    pthread_mutex_init(&lock, NULL);
    assert((pool = threadpool_create(threadi,QUEUE, 0)) != NULL);
    //fprintf(stderr, "Pool started with %d threads and ""queue size of %d\n", threadi, QUEUE);
    //init_queue ();
    char buf [BUFSIZE];
    //socket的建立
    char inputBuffer[99999] = {};
    char message[] = {"Hi,this is server.\n"};
    int sockfd = 0,forClientSockfd = 0;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("Fail to create a socket.");
    }
    //socket的連線
    struct sockaddr_in serverInfo,clientInfo;
    int addrlen = sizeof(clientInfo);
    bzero(&serverInfo,sizeof(serverInfo));
    serverInfo.sin_family = PF_INET;
    serverInfo.sin_addr.s_addr = INADDR_ANY;
    serverInfo.sin_port = htons(portnum);
    bind(sockfd,(struct sockaddr *)&serverInfo,sizeof(serverInfo));
    listen(sockfd,5);
    //char s[999]="Query \"test\"s \"abc\"";
    while(1)
    {
        forClientSockfd = accept(sockfd,(struct sockaddr*) &clientInfo, &addrlen);
        struct mypara pstru;
        pstru.forClientSockfd=forClientSockfd;
        strncpy(pstru.path,path,strlen(path));
        assert(threadpool_add(pool, &do_task, &(pstru), 0) == 0);
    }
    return 0;
}
void do_task(void *arg)
{
    pthread_mutex_lock(&lock);
    struct mypara *pstru;
    pstru = arg;
    char path[99999]= {};
    strcpy(path,pstru->path);
    //printf("%s\n",path);
    // char s[999];//string
    //strcpy(s,pstru->string);
    //printf("%s\n",s);
    int socketnum=pstru->forClientSockfd;
    char s[99999] = {};
    //int *forClientSockfd = (int*)arg;
    recv(socketnum,s,sizeof(s),0);
    printf("%s\n",s);
    static char mes[999999]= {};
    static char forclient[99999]= {};
    char delim[] = "\"";
    char *token;
    int a=0;
    for(token = strtok(s, delim); token != NULL; token = strtok(NULL, delim))
    {
        a++;
        if(a%2==0)
        {
            //printf("%d\n",strlen(token));
            if(strlen(token)>128)
            {
                strcat(forclient,"Query string out of range use 128 byte to query\n");
                char over[128]= {};
                strncpy(over,token,128);
                query(mes,token,path);
                strcat(forclient,mes);
                strcpy(mes,"");
                /*
                query(mes,token,path);
                strcat(forclient,mes);
                strcpy(mes,"");
                */
                /*
                strcat(forclient,"Query string out of range,querry  only 128\n");
                /*
                char over[128]={};
                strncpy(over,token,128);
                query(mes,token,path);
                strcat(forclient,mes);
                strcpy(mes,"");
                */
                //printf("Query string out of range\n");
                //strcat(forclient,"String: \"");
                //strcat(forclient,token);
                //strcat(forclient,"\"\n");
                //strcat(forclient,"Query string out of range\n");
            }
            else
            {
                query(mes,token,path);
                //printf("client:%s\n",mes);
                strcat(forclient,mes);
                strcpy(mes,"");
            }

        }
    }
    send(socketnum,forclient,sizeof(forclient),0);
    strcpy(forclient,"");
    pthread_mutex_unlock(&lock);
}
void* query(char* mes,char* inputBuffer,char* path)
{
    //printf("%s\n",path);
    //grep -r 'test'  'testdir'
    char s[10]= {};
    char commanda[99999]="grep -r '";
    char command2[3]="' '";
    char command3[10]="> a.txt";
    char command4[2]="' ";
    strncat(commanda,inputBuffer,strlen(inputBuffer));//grep -r 'test
    strncat(commanda,command2,3);//grep -r 'test' '
    strncat(commanda,path,strlen(path));//grep -r 'te st' 'testdir
    strncat(commanda,command4,2);
    strncat(commanda,command3,8);
    //printf("%s\n",commanda);
    system(commanda);
    char *ch =NULL;
    char *bf =NULL;
    int len = 0;
    int count=0;
    char* delim;
    char history[999]= {};
    FILE *fpa=fopen( "a.txt", "r" ) ;
    FILE *fpc=fopen( "k.txt", "w" ) ;
    int loop=0;
    //getline(&ch, &len, fpa);
    //printf("%s\n",ch);
    while(getline(&ch, &len, fpa)!=-1)
    {
        loop++;
        //printf("ch:%s\n",ch);
        //find how many
        char cpy[999]= {};
        char t[999];
        //printf(":%s\n",mes);
        //printf("len:%d",strlen(ch)-1);
        strncpy(cpy,ch,strlen(ch)-1);
        char *token;
        char *cur;
        cur= cpy;
        delim= ":";
        token=strsep(&cur,delim);
        if(loop==1)
        {
            strcat(mes,"String: \"");
            strcat(mes,inputBuffer);
            strcat(mes,"\"\n");
            strcpy(t,"File: ");
            strncat(t,cpy,strlen(cpy));
            fprintf(fpc, t);
            strncat(mes,t,strlen(t));
            int c = countOccurrences(cur,inputBuffer);
            count=count+c;
            //printf("a%d\n",count);
            strcpy(history,cpy);
        }
        else
        {
            int compare=strcmp(cpy,history);
            if(compare==0)
            {
                //printf("H:%s\n",history);
                int c = countOccurrences(cur,inputBuffer);
                count=count+c;
                //printf("b%d\n",count);
                cur="";
                strcpy(history,cpy);
            }
            else
            {
                //printf("H:%s\n",history);
                sprintf(s,"%d",count);
                fprintf(fpc, ", Count: %s\n",s);
                strncat(mes,", Count: ",9);
                strncat(mes,s,strlen(s));
                strncat(mes,"\n",1);
                strcpy(t,"File: ");
                strncat(t,cpy,strlen(cpy));
                fprintf(fpc, t);
                strncat(mes,t,strlen(t));
                count=0;
                int c = countOccurrences(cur,inputBuffer);
                count=count+c;
                //printf("c%d\n",count);
                cur="";
                strcpy(history,cpy);
            }
        }
        strcpy(cpy,"");
    }
    sprintf(s,"%d",count);
    fprintf(fpc, ", Count: %s\n",s);
    strncat(mes,", Count: ",9);
    strncat(mes,s,strlen(s));
    strncat(mes,"\n",1);
    fclose(fpa);
    fclose(fpc);
    //printf("mes:%s",mes);
    //strncat(mes,"\0",1);
    char aa[]=",";
    int a=strncmp(mes,aa,1);
    //printf("%d\n",a);
    if(a==0)
    {
        //printf("a notfound");
        //char mn[]="Not found";
        strcpy(mes,"");
        strcat(mes,"String: \"");
        strcat(mes,inputBuffer);
        strcat(mes,"\"\n");
        strncat(mes,"Not found\n",10);
        return;
    }
    else
    {
        return;
    }
}
int countOccurrences(char * str, char * toSearch)
{
    int i, j, found, count;
    int stringLen, searchLen;
    stringLen = strlen(str);      // length of string
    searchLen = strlen(toSearch); // length of word to be searched
    count = 0;
    for(i=0; i <= stringLen-searchLen; i++)
    {
        /* Match word with string */
        found = 1;
        for(j=0; j<searchLen; j++)
        {
            if(str[i + j] != toSearch[j])
            {
                found = 0;
                break;
            }
        }
        if(found == 1)
        {
            count++;
        }
    }
    return count;
}

threadpool_t *threadpool_create(int thread_count, int queue_size, int flags)
{
    threadpool_t *pool;
    int i;
    (void) flags;

    if(thread_count <= 0 || thread_count > MAX_THREADS || queue_size <= 0 || queue_size > MAX_QUEUE)
    {
        return NULL;
    }

    if((pool = (threadpool_t *)malloc(sizeof(threadpool_t))) == NULL)
    {
        goto err;
    }

    /* Initialize */
    pool->thread_count = 0;
    pool->queue_size = queue_size;
    pool->head = pool->tail = pool->count = 0;
    pool->shutdown = pool->started = 0;

    /* Allocate thread and task queue */
    pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * thread_count);
    pool->queue = (threadpool_task_t *)malloc
                  (sizeof(threadpool_task_t) * queue_size);

    /* Initialize mutex and conditional variable first */
    if((pthread_mutex_init(&(pool->lock), NULL) != 0) ||
            (pthread_cond_init(&(pool->notify), NULL) != 0) ||
            (pool->threads == NULL) ||
            (pool->queue == NULL))
    {
        goto err;
    }

    /* Start worker threads */
    for(i = 0; i < thread_count; i++)
    {
        if(pthread_create(&(pool->threads[i]), NULL,threadpool_thread, (void*)pool) != 0)
        {
            threadpool_destroy(pool, 0);
            return NULL;
        }
        pool->thread_count++;
        pool->started++;
    }

    return pool;

err:
    if(pool)
    {
        threadpool_free(pool);
    }
    return NULL;
}

int threadpool_add(threadpool_t *pool, void (*function)(void *),void *argument, int flags)
{
    int err = 0;
    int next;
    (void) flags;

    if(pool == NULL || function == NULL)
    {
        return threadpool_invalid;
    }

    if(pthread_mutex_lock(&(pool->lock)) != 0)
    {
        return threadpool_lock_failure;
    }

    next = (pool->tail + 1) % pool->queue_size;

    do
    {
        /* Are we full ? */
        if(pool->count == pool->queue_size)
        {
            err = threadpool_queue_full;
            break;
        }

        /* Are we shutting down ? */
        if(pool->shutdown)
        {
            err = threadpool_shutdown;
            break;
        }

        /* Add task to queue */
        pool->queue[pool->tail].function = function;
        pool->queue[pool->tail].argument = argument;
        pool->tail = next;
        pool->count += 1;

        /* pthread_cond_broadcast */
        if(pthread_cond_signal(&(pool->notify)) != 0)
        {
            err = threadpool_lock_failure;
            break;
        }
    }
    while(0);

    if(pthread_mutex_unlock(&pool->lock) != 0)
    {
        err = threadpool_lock_failure;
    }

    return err;
}

int threadpool_destroy(threadpool_t *pool, int flags)
{
    int i, err = 0;

    if(pool == NULL)
    {
        return threadpool_invalid;
    }

    if(pthread_mutex_lock(&(pool->lock)) != 0)
    {
        return threadpool_lock_failure;
    }

    do
    {
        /* Already shutting down */
        if(pool->shutdown)
        {
            err = threadpool_shutdown;
            break;
        }

        pool->shutdown = (flags & threadpool_graceful) ?
                         graceful_shutdown : immediate_shutdown;

        /* Wake up all worker threads */
        if((pthread_cond_broadcast(&(pool->notify)) != 0) ||
                (pthread_mutex_unlock(&(pool->lock)) != 0))
        {
            err = threadpool_lock_failure;
            break;
        }

        /* Join all worker thread */
        for(i = 0; i < pool->thread_count; i++)
        {
            if(pthread_join(pool->threads[i], NULL) != 0)
            {
                err = threadpool_thread_failure;
            }
        }
    }
    while(0);

    /* Only if everything went well do we deallocate the pool */
    if(!err)
    {
        threadpool_free(pool);
    }
    return err;
}

int threadpool_free(threadpool_t *pool)
{
    if(pool == NULL || pool->started > 0)
    {
        return -1;
    }

    /* Did we manage to allocate ? */
    if(pool->threads)
    {
        free(pool->threads);
        free(pool->queue);

        /* Because we allocate pool->threads after initializing the
           mutex and condition variable, we're sure they're
           initialized. Let's lock the mutex just in case. */
        pthread_mutex_lock(&(pool->lock));
        pthread_mutex_destroy(&(pool->lock));
        pthread_cond_destroy(&(pool->notify));
    }
    free(pool);
    return 0;
}
static void *threadpool_thread(void *threadpool)
{
    threadpool_t *pool = (threadpool_t *)threadpool;
    threadpool_task_t task;
    for(;;)
    {
        /* Lock must be taken to wait on conditional variable */
        pthread_mutex_lock(&(pool->lock));

        /* Wait on condition variable, check for spurious wakeups.
           When returning from pthread_cond_wait(), we own the lock. */
        while((pool->count == 0) && (!pool->shutdown))
        {
            pthread_cond_wait(&(pool->notify), &(pool->lock));
        }

        if((pool->shutdown == immediate_shutdown) ||((pool->shutdown == graceful_shutdown) &&(pool->count == 0)))
        {
            break;
        }

        /* Grab our task */
        task.function = pool->queue[pool->head].function;
        task.argument = pool->queue[pool->head].argument;
        pool->head = (pool->head + 1) % pool->queue_size;
        pool->count -= 1;

        /* Unlock */
        pthread_mutex_unlock(&(pool->lock));

        /* Get to work */
        (*(task.function))(task.argument);
    }
    pool->started--;
    pthread_mutex_unlock(&(pool->lock));
    pthread_exit(NULL);
    return(NULL);
}