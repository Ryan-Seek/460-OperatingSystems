extern PROC *running;
extern PROC *readyQueue;

typedef struct semaphore{
  int value;
  PROC *queue;
}SEMAPHORE;

int P(struct semaphore *s)
{
  int SR = int_off();
  s->value--;

  if(s->value < 0)
  {
    running->status = BLOCK;
    enqueue(&s->queue, running);
    int_on(SR);
    tswitch();
  }
  int_on(SR);
}

int V(struct semaphore *s)
{
  PROC *p;
  int cpsr;
  int SR = int_off();
  s->value++;

  if (s->value <= 0)
  {
    p = dequeue(&s->queue);
    p->status = READY;
    enqueue(&readyQueue, p);
    printf("V up task%d pi=%d; running pri = %d\n", p->pid, p->priority, running->priority);
    scheduler();
  }
  int_on(SR);
}
