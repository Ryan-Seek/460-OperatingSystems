#include "defines.h"
#include "timer.c"
#include "string.c"
#include "vid.c"
#include "interrupts.c"

int color;


void copy_vectors(void){
  extern u32 vectors_start, vectors_end;
  u32 *vectors_src = & vectors_start; 
  u32 *vectors_dst = (u32*) 0;
  while ( vectors_src < & vectors_end)
    *vectors_dst ++ = *vectors_src ++;
  } 
 
 
 
 void timer_handler(); 
 TIMER * tp[4];

 void IRQ_handler()
{
  int vicstatus = VIC_STATUS;
  if (vicstatus & (1 << 4))
  {
    if (*(tp[0]->base + TVALUE) == 0)
      timer_handler(0);
    if (*(tp[1]->base + TVALUE) == 0)
      timer_handler(1);
  }
      
  if ( vicstatus & (1 << 5))
  {
  if (*(tp[2]->base + TVALUE) == 0)
    timer_handler(2);
  if (*(tp[3]->base + TVALUE) == 0)
    timer_handler(3);
  } 
} 

int main() 

{
int i;
color = RED;
fbuf_init(); 
printf("main starts\n");
VIC_INTENABLE = 0;
VIC_INTENABLE |= (1 << 4);
VIC_INTENABLE |= (1 << 5); 
timer_init();

for (i = 0; i < 4; i++)
{
  tp[i] =&timer[i];
  timer_start( i);
}
printf("Enter while( 1)   loop,   handle   timer   interrupts \n"); 

}

