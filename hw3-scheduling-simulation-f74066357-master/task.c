#include "task.h"
status_type activate_task(task_type id)
{
    status_type result;
    //task with <id> is transferred from suspended state into ready state.
    //task with <id> is not in suspended state, the activation is ignored -> return STATUS_ERROR
    int position=-1;
    for(int k=0; k<TASKS_COUNT; k++)
    {
        if(ready[k]==id)
        {
            position=k;
            break;
        }
    }
    if((position!=-1)||(id==running))//exist or already running
    {
        result = STATUS_ERROR;
        return result;
    }
    else
    {
        //add to ready state
        for(int j=0; j<TASKS_COUNT; j++)
        {
            if(ready[j]==-1)
            {
                ready[j]=id;
                break;
            }
        }
        result = STATUS_OK;
    }
    //Reschedule if needed.
    swapcontext(&ctx[running],&schedule_ctx);
    return result;
}

status_type terminate_task(void)
{
    status_type result;
    for(int j=0; j<RESOURCES_COUNT; j++)
    {
        //still occupies any resource, the termination is ignored -> Return STATUS_ERROR
        if(res[j]==running)
        {
            result = STATUS_ERROR;
            return result;
        }
    }
    //task is transferred from running state into suspended state
    makecontext(&ctx[running],task_const[running].entry,0);
    temp_priority[running]=task_const[running].static_priority;
    running=-1;
    setcontext(&schedule_ctx);
    result = STATUS_OK;
    return result;
}