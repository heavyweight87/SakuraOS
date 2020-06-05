##===================================================================================================================
##
##  SwitchToTask.s -- This will perform the actual task switch
##
## -----------------------------------------------------------------------------------------------------------------
##
##     Date      Tracker  Version  Pgmr  Description
##  -----------  -------  -------  ----  --------------------------------------------------------------------------
##  2019-Sep-22  Initial   step01  ADCL  Initial version
##  2019-Sep-26  Step 01   step01  ADCL  Fix a stack symmetry issue
##  2019-Oct-05  Step 3    step03  ADCL  Add the call to `UpdateTimeUsed()`
##  2019-Oct-05  Step 4    step04  ADCL  Create a process state
##  2019-Oct-24  Step 5    step05  ADCL  Add rudamentary scheduler lock
##  2019-Oct-25  Step 6    step06  ADCL  Add the ability to block/unblock
##  2019-Nov-01  Step 8    step08  ADCL  Add checks for postponed task changes
##  2019-Nov-05  Step 9    step09  ADCL  Add sleeping to the process repetoire
##  2019-Nov-20  Step12    step12  ADCL  Add process termination
##
##===================================================================================================================


##
## -- things that need to be seen outside this source
##    -----------------------------------------------
        .global     SwitchToTask        ## void SwitchToTask(PCB_t *task) __attributes__((cdecl));
        .global     GetCR3              ## unsigned int GetCR3(void);


##
## -- thing that need to be see inside this source but are defined outside
##    --------------------------------------------------------------------
        .extern     currentPCB
        .extern     UpdateTimeUsed
        .extern     AddReady
        .extern     postponeTaskCounter
        .extern     taskChangePostponedFlag


##
## -- Some offsets into the PCB sructure
##    ----------------------------------
.equ    TOS,        0
.equ    VAS,        4
.equ    STATE,      12


##
## -- These are the different states
##    ------------------------------
.equ    RUNNING,    0
.equ    READY,      1
.equ    PAUSED,     2
.equ        SLEEPING, 3
.equ        TERMINATED, 4

##
## -- Perform the actual task switch
##   
##    Some notes about this function:
##    * cdecl ensures that `eax`, `ecx`, and `edx` are saved on the stack
##    * `eip` was also saved by the `call` instruction
##    * `cr3` does not need to be saved -- once set it cannot be changed
##    * segment registers do not need to be saves -- kernel segments to kernel segments
##    
##    IRQs are required to be disabled prior to calling and restored on exit
##
##    !!!! The caller must have called LockScheduler() and must call !!!!
##    !!!! UnlockScheduler() before and after the call to Schedule() !!!!
##    ---------------------------------------------------------------------------------
SwitchToTask: 
        cmpl        $0,postponeTaskCounter          ## are we OK to change tasks?
        je          SwitchToTask.swap
        movl        $1,taskChangePostponedFlag          ## note we are ready to postpone tasks
        ret

SwitchToTask.swap: 
        pushl       %ebx
        pushl       %esi
        pushl       %edi
        pushl       %ebp

        movl        currentPCB,%edi         ## `edi` = previous tasks PCB

        cmpl        $0,STATE(%edi)          ## is the task RUNNING?
        jne         SwitchToTask.noReady    ## if not, no need to ready it

        pushl       %edi                    ## push the current process onto the stack
        call        AddReady                ## add it to the ready list
        addl        $4,%esp                 ## drop the value from the stack

SwitchToTask.noReady: 
        call        UpdateTimeUsed          ## update the time used field before swapping

        movl        %esp,TOS(%edi)          ## save the top of the stack


        ## -- load the next task's state


        movl        ((4+1)*4)(%esp),%esi    ## `esi` = next tasks PCB
        movl        %esi,currentPCB         ## save the current process PCB

        movl        TOS(%esi),%esp          ## load the next process's stack
        movl        VAS(%esi),%eax          ## load the next process's virtual address space
        movl        $0,STATE(%esi)          ## make the current task running

        ## -- fix tss.esp0 here

        movl        %cr3, %ecx              ## get the current task cr3

        cmpl        %eax,%ecx               ## are the virtual address spaces the same
        jmp         SwitchToTask.sameVAS
        movl        %eax, %cr3              ## update to the new virtual address space

SwitchToTask.sameVAS: 
        popl        %ebp
        popl        %edi
        popl        %esi
        popl        %ebx

        ret                                 ## this is the next task's `eip`


##
## -- get the current `cr3` value
##    ---------------------------
GetCR3: 
        movl        %cr3, %eax
        ret
