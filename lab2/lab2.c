/*+
 * Module:  lab2.c
 *Authored by: Adam Bayley and Samantha Hawco
 *
-*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>

static struct task_struct * firstTask, *theTask;

int cnt;

int my_read_proc(char * page, char **start, off_t fpos, int blen, int * eof,void * data){

    int numChars = 0;
    int size_of_page = (PAGE_SIZE >> 10);   
    numChars = sprintf(page, "\tPID\tUID\tVSZ\tRSS\n"); //displays heading
    if (fpos == 0){
        //numChars = sprintf(page, "Hello"); , numChars += sprintf(page + numChars, " World\n"); <-- 2 lines from earlier part

        theTask = &init_task;
        //loop until pid is not zero, go through tasks
        while (theTask->pid == 0) {
            theTask = theTask->next_task;
        }
        firstTask = theTask;
        //display the information for the first task
        numChars += sprintf(page + numChars, "\t%4d\t%4d", firstTask->pid, firstTask->uid);
        
        if (firstTask->mm == NULL) {        //if NULL print zeros instead
            numChars += sprintf(page + numChars, "%4d\t%4d\n", 0,0);
        }
        else {                              //display VSZ and RSS information
            numChars += sprintf(page + numChars, "\t%4d\t%4d\n", firstTask->mm->total_vm * size_of_page, firstTask->mm->rss * size_of_page);
        }

        //go to next task where pid is not zero
        do {
            theTask = theTask->next_task;
        } while (theTask->pid == 0);

    }
    else {
        //if the current task is the same as the first task, set variables and return
        if (theTask == firstTask){
            *eof = 0;
            *start = page;
            return 0;
        }
        //same procedure as in the if 
        numChars = sprintf(page, "\t%4d\t%4d", theTask->pid, theTask->uid);
        if (theTask->mm == NULL) {
            numChars += sprintf(page + numChars, "\t%4d\t%4d\n", 0,0);
        } else {
            numChars += sprintf(page + numChars, "\t%4d\t%4d\n", theTask->mm->total_vm * size_of_page, theTask->mm->rss * size_of_page);
        }

        //go to next task where pid is not zero
        do {
            theTask = theTask->next_task;
        } while (theTask->pid == 0);

    }
    *eof = 1;
    *start = page;
    return numChars;
}

int init_module(){
    //create proc_entry
    struct proc_dir_entry * proc_entry;
    proc_entry = create_proc_entry("lab2", 0444, NULL);

    //determine if creation was successful or not
    if (proc_entry == NULL) {   //create failed
        return -1;
    }
    else {  //did not fail
        proc_entry->read_proc = my_read_proc;
        return 0;
    }
}

void cleanup_module()
{
    remove_proc_entry("lab2", NULL);
}
