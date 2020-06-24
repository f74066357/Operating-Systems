#include "typedefine.h"
#include "resource.h"
#include "task.h"
#include <stdio.h>
#include <ucontext.h>
ucontext_t schedule_ctx;
ucontext_t ctx[TASKS_COUNT];
int res[RESOURCES_COUNT]= {0};
int res_priority[RESOURCES_COUNT]= {0};
int running=-1;
char st[TASKS_COUNT][8192];
char sch_st[8192];
int ready[TASKS_COUNT];
int temp_priority[TASKS_COUNT];

/****************************/
void schedule();
int main()
{
    //printf("%d\n",running);
    getcontext(&schedule_ctx);
    schedule_ctx.uc_link = &schedule_ctx;
    schedule_ctx.uc_stack.ss_sp = sch_st;
    schedule_ctx.uc_stack.ss_size = sizeof sch_st;
    makecontext (&schedule_ctx,schedule, 0);
    //resource set
    for(int j=0; j<RESOURCES_COUNT; j++)
    {
        res[j]=-1;//-1:沒被占用 1:被占用
        res_priority[j]=resources_priority[j];
    }
    for(int j=0; j<TASKS_COUNT; j++)
    {
        // initialize context
        getcontext(&ctx[j]);
        ctx[j].uc_link = &schedule_ctx; //运行完成后，会跳至sch_ctx指向的context继续运行
        ctx[j].uc_stack.ss_sp = st[j]; // 设置新的堆栈
        ctx[j].uc_stack.ss_size = sizeof st[j];
        makecontext(&ctx[j],task_const[j].entry,0);
        temp_priority[j]=task_const[j].static_priority;
        ready[j]=-1;
    }
    //auto start tesk set
    int i;
    for(i=0; i<AUTO_START_TASKS_COUNT; i++)
    {
        ready[i]=auto_start_tasks_list[i];
    }
    setcontext(&schedule_ctx);
    while (1)
        ;
    return 0;
}
void schedule(void)
{
    /*
    for(int i=0;i<TASKS_COUNT;i++){
        printf("%d ",ready[i]);
    }
    */
    int highest=0;
    int l=0;
    int p=0;
    for(int i=0; i<TASKS_COUNT; i++)
    {
        int k=ready[i];
        //printf("%d %d\n",k,temp_priority[k]);
        if(temp_priority[k]>highest)
        {
            highest=temp_priority[k];//piority
            l=i;//location
            p=k;;//task id
        }
    }
    if(running!=-1)//reschedule
    {
        if(temp_priority[running]<temp_priority[p]) //swap
        {
            //delete highest
            for(int i=l; i<TASKS_COUNT-1; i++)
            {
                ready[i]=ready[i+1];
            }
            //insert running
            for(int i=TASKS_COUNT-1; i>0; i--)
            {
                ready[i]=ready[i-1];
            }
            ready[0]=running;
            int temp=running;
            running=p;
            setcontext(&ctx[p]);
        }
        else
        {
            //swapcontext(&schedule_ctx,&ctx[running]);
            setcontext(&ctx[running]);
        }
    }
    else//沒有東西正在run
    {
        for(int i=l; i<TASKS_COUNT-1; i++)
        {
            ready[i]=ready[i+1];
        }
        ready[TASKS_COUNT-1]=-1;
        running=p;
        setcontext(&ctx[p]);
    }
}