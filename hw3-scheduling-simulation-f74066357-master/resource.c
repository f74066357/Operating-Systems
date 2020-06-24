#include "resource.h"

status_type get_resource(resource_type id)
{
    status_type result;
    if(res[id]==-1) //resource not occupied
    {
        res[id]=running;//printf("%d\n",RESOURCE_1);0
        /*If a task got a resource and its current priority is lower
        than the ceiling priority of the resource, the priority of
        the task would raise to the ceiling priority of the resource.
        */
        if(temp_priority[running]<res_priority[id])
        {
            temp_priority[running]=res_priority[id];
            //printf("%d %d\n",running,temp_priority[running]);
        }
        result = STATUS_OK;
    }
    else
    {
        /*resource with <id> is already occupied, the above
        operations are ignored -> return STATUS_ERROR
        */
        result = STATUS_ERROR;
        return result;
    }
    return result;
}

status_type release_resource(resource_type id)
{
    status_type result = STATUS_OK;
    /*
    released a resource, the priority of the task
    would be reset to the priority before getting that resource
    */
    if(res[id]==running)
    {
        res[id]=-1;//release
        if(temp_priority[running]>task_const[running].static_priority)
        {
            temp_priority[running]=task_const[running].static_priority;
        }
        status_type result = STATUS_OK;
    }
    else
    {
        /*
        release a resource that is not occupied by the calling task,
        the above operations are ignored -> return STATUS_ERROR
        */
        status_type result = STATUS_ERROR;
        return result;
    }
    //Reschedule if needed.
    swapcontext(&ctx[running],&schedule_ctx);
    return result;
}