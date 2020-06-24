#include "memory_manager.h"
int main(int argc, char *argv[])
{
    list * memH = NULL;
    second * mem2H = NULL;
    list * mem3H = NULL;
    list * diskH = NULL;
    int policy=-1;
    int M=0;
    int N=0;
    int loop=0;
    int len = 0;
    int hit=0;
    int miss=0;
    int startpoint=0;
    char ch[100] = {};

    while(scanf("%[^\n]%*c",ch)!=EOF)
    {
        loop++;
        if(loop==1) //policy
        {
            if(ch[8]=='F')
            {
                policy=0;
                //printf("%s\n","F");
            }
            else if(ch[8]=='E')
            {
                policy=1;
                //printf("%s\n","E");
            }
            else if(ch[8]=='S')
            {
                policy=2;
                //printf("%s\n","S");
            }
            else
            {
                printf("%s\n","Wrong policy format");
                break;
            }
        }
        else if(loop==2) //M
        {
            char *token;
            char *cur;
            char cpy2[20]= {};
            cur= ch;
            token=strsep(&cur,":");
            strncpy(cpy2,cur,strlen(cur));
            cur= cpy2;
            token=strsep(&cur," ");
            M = atoi(cur);//str to int
            //printf("M:%d\n",M);
            diskH = malloc(sizeof(list));
            if (diskH == NULL)
            {
                return 1;
            }
            diskH->index = 0;
            diskH->val = -1;
            diskH->next = NULL;
            for(int i=1; i<M; i++)
            {
                push(diskH,i,-1);
            }
            //print_list(diskH);
        }
        else if(loop==3) //N
        {
            char *token;
            char *cur;
            char cpy2[20]= {};
            cur= ch;
            token=strsep(&cur,":");
            strncpy(cpy2,cur,strlen(cur));
            cur= cpy2;
            token=strsep(&cur," ");
            N = atoi(cur);//str to int
            //printf("N:%d\n",N);
            if(policy==0)
            {
                memH = malloc(sizeof(list));
                if (memH == NULL)
                {
                    return 1;
                }
                memH->index = 0;
                memH->val = -1;
                memH->next = NULL;
                for(int i=1; i<N; i++)
                {
                    push(memH,i,-1);
                }
                //print_list(memH);
            }
            else if(policy==1) //E
            {
                mem2H = malloc(sizeof(second));
                if (mem2H == NULL)
                {
                    return 1;
                }
                mem2H->index = 0;
                mem2H->val = -1;
                mem2H->ref = 0;
                mem2H->dir = 0;
                mem2H->next = NULL;
                for(int i=1; i<N; i++)
                {
                    push2(mem2H,i,-1,0,0);
                }
                //print_list2(mem2H);
            }
            else //S policy==2
            {
                //printf("%d\n",N);
                if(N%2==0)
                {
                    inactiven=(N/2);
                    activen=(N/2);
                }
                else
                {
                    inactiven=(N+1)/2;
                    activen=(N-1)/2;
                }
                //frame
                mem3H = malloc(sizeof(list));
                if (mem3H == NULL)
                {
                    return 1;
                }
                mem3H->index = 0;
                mem3H->val = -1;
                mem3H->next = NULL;
                for(int i=1; i<N; i++)
                {
                    push(mem3H,i,-1);
                }
                //print_list(mem3H);
                //建active list
                for(int i=0; i<activen; i++)
                {
                    active[i]=-1;
                }
                //建inactive list
                for(int i=0; i<inactiven; i++)
                {
                    inactive[i]=-1;
                }
                //ref
                for(int i=0; i<200; i++)
                {
                    ref[i]=-1;
                }
            }
        }
        else if(loop==4)
        {
        }
        else
        {
            if(policy==0) //FIFO
            {
                //printf("%s ","FIFO");
                //printf("head:%d\n",head->val);
                char *token;
                char *cur;
                char cpy2[20]= {};
                cur= ch;
                token=strsep(&cur," ");
                //printf("%s ",cpy);
                strncpy(cpy2,cur,strlen(cur));
                cur= cpy2;
                token=strsep(&cur,"\n");
                int k=atoi(cpy2);
                //printf("k:%d\n",k);
                int a=search(memH,k);
                if(a!=-1) //hit
                {
                    hit++;
                    //printf("Hit, %d=>%d\n",k,a);
                    printf("Hit, %d=>%d\n",k,a);
                }
                else //miss
                {
                    miss++;
                    //找找看有沒有空位
                    int empty=search(memH,-1);
                    if(empty!=-1) //有空位
                    {
                        list * current = memH;
                        for(int i=0; i<empty; i++)
                        {
                            current = current->next;
                        }
                        current->val=k;
                        printf("Miss, %d, %d>>%d, %d<<%d\n",current->index,-1,-1,k,-1);
                    }
                    else //踢人(head)
                    {
                        int p=search(diskH,-1);
                        list* cur = diskH;
                        for(int i=0; i<p; i++)
                        {
                            cur = cur->next;
                        }
                        int evi=memH->val;
                        int des=cur->index;
                        cur->val=memH->val;//head的value 放進disk
                        int exist=search(diskH,k);
                        int sour=-1;
                        if(exist!=-1)
                        {
                            list* cur2 = diskH;
                            for(int i=0; i<exist; i++)
                            {
                                cur2 = cur2->next;
                            }
                            sour=cur2->index;
                            cur2->val=-1;//disk 裡面有A 先把該index的value改-1
                        }
                        int pfi=memH->index;
                        memH->val=k;
                        push(memH,memH->index,memH->val);
                        list* temp=memH;
                        memH=memH->next;
                        free(temp);
                        printf("Miss, %d, %d>>%d, %d<<%d\n",pfi,evi,des,k,sour);
                    }
                }
            }
            else if(policy==1) //E
            {
                //printf("%s ","Enhance");
                char *token;
                char *cur;
                char cpy2[20]= {};
                cur= ch;
                token=strsep(&cur," ");
                //printf("%s ",cpy);
                strncpy(cpy2,cur,strlen(cur));
                cur= cpy2;
                token=strsep(&cur,"\n");
                int k=atoi(cpy2);
                //printf("k:%d\n",k);
                //search //write
                int a=search2(mem2H,k);
                //print_list2(mem2H);
                //printf("startpoint:%d\n",startpoint);
                if(a!=-1) //hit
                {
                    hit++;
                    printf("Hit, %d=>%d\n",k,a);
                    second * current = mem2H;
                    for(int i=0; i<a; i++)
                    {
                        current = current->next;
                    }
                    current->ref=1;
                    if(current->next!=NULL)
                    {
                        startpoint=a+1;
                    }
                    else
                    {
                        startpoint=0;
                    }
                    if(strcmp(ch,"Write")==0) //改dir bit 為1
                    {
                        current->dir=1;
                    }
                    else
                    {
                        //current->dir=0;
                    }
                }
                else
                {
                    miss++;
                    int empty=search2(mem2H,-1);//還有位置嗎?
                    if(empty!=-1)
                    {
                        second * current = mem2H;
                        for(int i=0; i<empty; i++)
                        {
                            current = current->next;
                        }
                        current->val=k;
                        current->ref=1;
                        if(current->next!=NULL)
                        {
                            startpoint=empty+1;
                        }
                        else
                        {
                            startpoint=0;
                        }
                        if(strcmp(ch,"Write")==0) //改dir bit 為1
                        {
                            current->dir=1;
                        }
                        else
                        {
                            //current->dir=0;
                        }
                        printf("Miss, %d, %d>>%d, %d<<%d\n",current->index,-1,-1,k,-1);
                    }
                    else //踢人
                    {
                        int z1=searchpoint(mem2H,0,0,startpoint);
                        if(z1!=-1) //優先踢(0,0)
                        {
                            //printf("%s\n","00-1");
                            int cmp=strcmp(ch,"Write");
                            //first(0,0)
                            second * current=mem2H;
                            for(int i=0; i<z1; i++)
                            {
                                current = current->next;
                            }
                            int evi=current->val;
                            int pfi=current->index;
                            current->val=k;
                            current->ref=1;
                            if(current->next!=NULL)
                            {
                                startpoint=z1+1;
                            }
                            else
                            {
                                startpoint=0;
                            }
                            if(cmp==0) //改dir bit 為1
                            {
                                current->dir=1;
                            }
                            else
                            {
                                current->dir=0;
                            }

                            int p=search(diskH,-1);
                            list* cur = diskH;
                            for(int i=0; i<p; i++)
                            {
                                cur = cur->next;
                            }
                            int des=cur->index;
                            cur->val=evi;

                            int exist=search(diskH,k);
                            int sour=-1;
                            if(exist!=-1)
                            {
                                list* cur2 = diskH;
                                for(int i=0; i<exist; i++)
                                {
                                    cur2 = cur2->next;
                                }
                                sour=cur2->index;
                                cur2->val=-1;//disk 裡面有A 先把該index的value改-1
                            }
                            printf("Miss, %d, %d>>%d, %d<<%d\n",pfi,evi,des,k,sour);
                        }
                        else
                        {
                            int z2=searchpoint(mem2H,0,1,startpoint);
                            if(z2!=-1)
                            {
                                //printf("%s\n","01-1");
                                int cmp=strcmp(ch,"Write");
                                //first(0,1)
                                ref2zero(mem2H,startpoint,z2);//ref
                                second* current=mem2H;
                                for(int i=0; i<z2; i++)
                                {
                                    current = current->next;
                                }
                                int evi=current->val;
                                int pfi=current->index;
                                current->val=k;
                                current->ref=1;
                                if(current->next!=NULL)
                                {
                                    startpoint=z2+1;
                                }
                                else
                                {
                                    startpoint=0;
                                }
                                if(cmp==0) //改dir bit 為1
                                {
                                    current->dir=1;
                                }
                                else
                                {
                                    current->dir=0;
                                }

                                int p=search(diskH,-1);
                                list* cur = diskH;
                                for(int i=0; i<p; i++)
                                {
                                    cur = cur->next;
                                }
                                int des=cur->index;
                                cur->val=evi;

                                int exist=search(diskH,k);
                                int sour=-1;
                                if(exist!=-1)
                                {
                                    list* cur2 = diskH;
                                    for(int i=0; i<exist; i++)
                                    {
                                        cur2 = cur2->next;
                                    }
                                    sour=cur2->index;
                                    cur2->val=-1;//disk 裡面有A 先把該index的value改-1
                                }
                                printf("Miss, %d, %d>>%d, %d<<%d\n",pfi,evi,des,k,sour);
                            }
                            else
                            {
                                second * current=mem2H;
                                while(current!=NULL)
                                {
                                    current->ref=0;
                                    current = current->next;
                                }
                                int z1=searchpoint(mem2H,0,0,startpoint);
                                if(z1!=-1) //踢(0,0)
                                {
                                    //printf("%s\n","00-2");
                                    int cmp=strcmp(ch,"Write");
                                    //first(0,0)
                                    second * current=mem2H;
                                    for(int i=0; i<z1; i++)
                                    {
                                        current = current->next;
                                    }
                                    int evi=current->val;
                                    int pfi=current->index;
                                    current->val=k;
                                    current->ref=1;
                                    if(current->next!=NULL)
                                    {
                                        startpoint=z1+1;
                                    }
                                    else
                                    {
                                        startpoint=0;
                                    }
                                    if(cmp==0) //改dir bit 為1
                                    {
                                        current->dir=1;
                                    }
                                    else
                                    {
                                        current->dir=0;
                                    }

                                    int p=search(diskH,-1);
                                    list* cur = diskH;
                                    for(int i=0; i<p; i++)
                                    {
                                        cur = cur->next;
                                    }
                                    int des=cur->index;
                                    cur->val=evi;
                                    int exist=search(diskH,k);
                                    int sour=-1;
                                    if(exist!=-1)
                                    {
                                        list* cur2 = diskH;
                                        for(int i=0; i<exist; i++)
                                        {
                                            cur2 = cur2->next;
                                        }
                                        sour=cur2->index;
                                        cur2->val=-1;//disk 裡面有A 先把該index的value改-1
                                    }
                                    printf("Miss, %d, %d>>%d, %d<<%d\n",pfi,evi,des,k,sour);
                                }
                                else //踢(0,1)
                                {
                                    //printf("%s\n","01-2");
                                    int z2=searchpoint(mem2H,0,1,startpoint);
                                    int cmp=strcmp(ch,"Write");
                                    //first(0,1)
                                    second * current=mem2H;
                                    //print_list2(mem2H);
                                    for(int i=0; i<z2; i++)
                                    {
                                        current = current->next;
                                    }
                                    int evi=current->val;
                                    int pfi=current->index;
                                    current->val=k;
                                    current->ref=1;
                                    if(current->next!=NULL)
                                    {
                                        startpoint=z2+1;
                                    }
                                    else
                                    {
                                        startpoint=0;
                                    }
                                    if(cmp==0) //改dir bit 為1
                                    {
                                        current->dir=1;
                                    }
                                    else
                                    {
                                        current->dir=0;
                                    }

                                    int p=search(diskH,-1);
                                    list* cur = diskH;
                                    for(int i=0; i<p; i++)
                                    {
                                        cur = cur->next;
                                    }
                                    int des=cur->index;
                                    cur->val=evi;

                                    int exist=search(diskH,k);
                                    int sour=-1;
                                    if(exist!=-1)
                                    {
                                        list* cur2 = diskH;
                                        for(int i=0; i<exist; i++)
                                        {
                                            cur2 = cur2->next;
                                        }
                                        sour=cur2->index;
                                        cur2->val=-1;//disk 裡面有A 先把該index的value改-1
                                    }
                                    printf("Miss, %d, %d>>%d, %d<<%d\n",pfi,evi,des,k,sour);
                                }
                            }
                        }
                    }
                }
            }
            else //policy==2
            {
                char *token;
                char *cur;
                char cpy2[20]= {};
                cur= ch;
                token=strsep(&cur," ");
                //printf("%s ",ch);
                strncpy(cpy2,cur,strlen(cur));
                cur= cpy2;
                token=strsep(&cur,"\n");
                int k=atoi(cpy2);
                //printf("k:%d\n",k);
                //
                //print_list(mem3H);
                int a=search(mem3H,k);
                //printf("%d\n",a);
                if(a!=-1) //hit
                {
                    hit++;
                    int in_in=searcharr(inactive,inactiven,k);//search 在inactive裡面?
                    if(in_in!=-1) //在inactive裡面
                    {
                        int num=inactive[in_in];
                        if(ref[num]==0)
                        {
                            ref[num]=1;
                            movetohead(inactive,in_in);
                        }
                        else
                        {
                            if(activen!=0)
                            {
                                ref[num]=0;
                                //update
                                int act_empty=searcharr(active,activen,-1);
                                if(act_empty!=-1)
                                {
                                    //刪inactive
                                    deleten(inactive,inactiven,in_in);
                                    //放進active head
                                    active[act_empty]=num;
                                    movetohead(active,act_empty);
                                }
                                else //Search the tail of the active list for evicted page
                                {
                                    while(1)
                                    {
                                        int tmp_num=active[activen-1];
                                        if(ref[tmp_num]==0) //refill the page to inactive list head
                                        {
                                            inactive[in_in]=tmp_num;
                                            movetohead(inactive,in_in);
                                            active[activen-1]=num;
                                            movetohead(active,(activen-1));
                                            break;
                                        }
                                        else //set ref 0 search again
                                        {
                                            ref[tmp_num]=0;
                                            movetohead(active,(activen-1));
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else //在active裡面
                    {
                        int in_act=searcharr(active,activen,k);
                        int num=active[in_act];
                        if(ref[num]==0)
                        {
                            ref[num]=1;
                            movetohead(active,in_act);
                        }
                        else
                        {
                            movetohead(active,in_act);
                        }
                    }
                    //寫進frame
                    printf("Hit, %d=>%d\n",k,a);
                }
                else //miss
                {
                    miss++;
                    int in_empty=searcharr(inactive,inactiven,-1);
                    if(in_empty!=-1) //直接放
                    {
                        inactive[in_empty]=k;
                        movetohead(inactive,in_empty);
                        ref[k]=1;
                        int frame_empty=search(mem3H,-1);
                        list * current = mem3H;
                        for(int i=0; i<frame_empty; i++)
                        {
                            current = current->next;
                        }
                        current->val=k;
                        printf("Miss, %d, %d>>%d, %d<<%d\n",current->index,-1,-1,k,-1);
                    }
                    else //Search the tail of the inactive list for evicted page
                    {
                        int swap=0;
                        while(1)
                        {
                            int tmp_num=inactive[inactiven-1];
                            if(ref[tmp_num]==0) //swap out
                            {
                                swap=inactive[inactiven-1];
                                //printf("swap:%d\n",swap);
                                inactive[inactiven-1]=k;
                                movetohead(inactive,(inactiven-1));
                                break;
                            }
                            else //set ref 0 search again
                            {
                                ref[tmp_num]=0;
                                movetohead(inactive,(inactiven-1));
                            }
                        }

                        int location=search(mem3H,swap);
                        list * current = mem3H;
                        for(int i=0; i<location; i++)
                        {
                            current = current->next;
                        }
                        current->val=k;
                        ref[k]=1;
                        //放進disk
                        int disk_empty=search(diskH,-1);
                        list* cur = diskH;
                        for(int i=0; i<disk_empty; i++)
                        {
                            cur = cur->next;
                        }
                        cur->val=swap;
                        int des=cur->index;
                        int exist=search(diskH,k);
                        int sour=-1;
                        if(exist!=-1)
                        {
                            list* cur2 = diskH;
                            for(int i=0; i<exist; i++)
                            {
                                cur2 = cur2->next;
                            }
                            sour=cur2->index;
                            cur2->val=-1;//disk 裡面有A 先把該index的value改-1
                        }
                        int pfi=location;
                        int evi=swap;
                        //printf("%d\n",location);
                        printf("Miss, %d, %d>>%d, %d<<%d\n",pfi,evi,des,k,sour);
                    }
                }
            }
        }
    }
    float rate=(float)miss/(float)(hit+miss);
    printf("Page Fault Rate: %.3f\n",rate);
    return 0;
}

void print_list(list * head)
{
    list * current = head;
    while (current != NULL)
    {
        printf("%d %d\n", current->index,current->val);
        current = current->next;
    }
}
void print_list2(second* head)
{
    second * current = head;
    while (current != NULL)
    {
        printf("%d %d %d %d\n", current->index,current->val,current->ref,current->dir);
        current = current->next;
    }
    printf("\n");
}
void push(list* head, int index,int val)
{
    list * current = head;
    while (current->next != NULL)
    {
        current = current->next;
    }
    current->next = malloc(sizeof(list));
    current->next->index=index;
    current->next->val = val;
    current->next->next = NULL;
}
void push2(second* head, int index,int val,int ref,int dir)
{
    second * current =head;
    while (current->next != NULL)
    {
        current = current->next;
    }
    current->next = malloc(sizeof(second));
    current->next->index=index;
    current->next->val = val;
    current->next->ref = ref;
    current->next->dir = dir;
    current->next->next = NULL;
}
int search(list* head, int x)
{
    list* current = head;
    while (current != NULL)
    {
        if (current->val == x)
            return current->index;
        current = current->next;
    }
    return -1;
}
int search2(second* head, int x)
{
    second* current = head;
    while (current != NULL)
    {
        if (current->val == x)
            return current->index;
        current = current->next;
    }
    return -1;
}
int searchpoint(second* head, int ref,int dir,int start)
{
    second* current = head;
    for(int i=0; i<start; i++)
    {
        current = current->next;
    }
    while (current != NULL)
    {
        if (current->ref == ref && current->dir == dir)
            return current->index;
        current = current->next;
    }
    current = head;
    for(int i=0; i<start; i++)
    {
        if (current->ref == ref && current->dir == dir)
            return current->index;
        current = current->next;
    }
    return -1;
}
void ref2zero(second* head,int startpoint,int z)
{
    if(z<startpoint)
    {
        second * current=head;
        for(int i=0; i<startpoint; i++)
        {
            current = current->next;
        }
        while (current != NULL)
        {
            current->ref=0;
            current = current->next;
        }
        current=head;
        for(int i=0; i<z; i++)
        {
            current->ref=0;
            current = current->next;
        }
    }
    else
    {
        second * current=head;
        for(int i=0; i<startpoint; i++)
        {
            current = current->next;
        }
        for(int i=startpoint; i<z; i++)
        {
            current->ref=0;
            current = current->next;
        }
    }
}
int searcharr (int  arr[],int n,int k)
{
    int i=0;
    while(i<n)
    {
        if(arr[i]==k)
        {
            return i;
        }
        i++;
    }
    return -1;
}
void printarr(int arr[],int n)
{
    for(int i=0; i<n; i++)
    {
        printf("%d ",arr[i]);
    }
    printf("\n");
}
void movetohead(int arr[],int k)
{
    int tmp=arr[k];
    for(int i=k; i>0; i--)
    {
        arr[i]=arr[i-1];
    }
    arr[0]=tmp;
}
void deleten(int arr[],int n,int k)
{
    for(int i=k; i<n-1; i++)
    {
        arr[i]=arr[i+1];
    }
    arr[n-1]=-1;
}