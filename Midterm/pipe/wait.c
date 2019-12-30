int ksleep(int event)
{
  int sr = int_off();
    printf("proc %d going to sleep on event=%x\n", running->pid, event);

    running->event = event;
    running->status = SLEEP;

    enqueue(&sleepList, running);
    tswitch();

  int_on(sr);
}

int kwakeup(int event)
{
  int sr = int_off();
  PROC *temp, *proc;

  temp = 0;

  while( proc =dequeue(&sleepList))
  {
    if(proc->event == event)
    {
      printf("wakeup proc #%d on event #%d\n", proc->pid, event);
      proc->status = READY;
      enqueue(&readyQueue, proc);
    }

    else
    {
      enqueue(&temp, proc);
    }
    
  }
  
  sleepList = temp;

  int_on(sr);
}


int kexit(int exitCode)
{
  int i, wakeup;
  PROC *p;

  wakeup = sendChild();  // give children to P1

  running->exitCode = exitCode;
  running->status = ZOMBIE;
 
  kwakeup((int)running->parent);
  tswitch();
}

int kwait(int *status)
{
  int i, found = 0;
  PROC *p;

  if (!running->child){
    printf("wait error: no child\n");
    return -1;
  }

  while(1){
    // if can find a ZOMBIE child
    {
      for(i=1;i<NPROC;i++)
      {
        p=&proc[i];
        if(p->status != FREE && p->ppid == running->ppid)
        {
          found = 1;
          if(p->status == ZOMBIE)
          {
            *status = p->exitCode;
            p->status = FREE;
            enqueue(&freeList, p);
            return(p->pid);
          }
        }
      }
      if(!found)
      {
        printf("No child found and proc 1 did not die\n");
        return -1;
      }
    }

    ksleep((int)running);
  }
}

int sendChild()
{
  PROC *p = 0;
  int wakeupP1=0;
  /* send children (dead or alive) to P1's orphanage */
  for (int i = 1; i < NPROC; i++)
  {
    p = &proc[i]; //set temp to proc[i] to see if a child of running proc
    if (p->status != FREE && p->ppid == running->pid)
    {
      p->ppid = 1;
      p->parent = &proc[1];
      wakeupP1++;
    }
  }

  return wakeupP1;
}

