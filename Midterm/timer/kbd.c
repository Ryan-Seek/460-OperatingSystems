#include "keymap2"

#define KCNTL 0x00
#define KSTAT 0x04
#define KDATA 0x08
#define KCLK  0x0C
#define KISTA 0x10

#define CAPSLOCK         0x3A
#define LSHIFT           0x2A
#define RSHIFT           0x36
#define CONTROL          0x1D

typedef volatile struct kbd{
  char *base;
  char buf[128];
  int head, tail, data, room;
}KBD;

volatile KBD kbd;

// state variables
int shift;		// left and right shift key state 
int control;		// control key state 
int capslock;		// caps lock key state 
int release;
int counter;

int kbd_init()
{
  KBD *kp = &kbd;
  kp->base = (char *)0x10006000;
  *(kp->base + KCNTL) = 0x10; // bit4=Enable bit0=INT on
  *(kp->base + KCLK)  = 8;
  kp->head = kp->tail = 0;
  kp->data = 0; kp->room = 128;

  shift   = 0;
  control = 0;
  capslock = 0;
  release = 0;
}

void kbd_handler()
{
  unsigned char code, c;
  int i;
  KBD *kp = &kbd;

  code = *(kp->base + KDATA);


  if (code == 0x12)
    shift = 1;

  if (code == 0x14)
    control = 1;

  if(code == 0x58)
    {
      counter = counter + 1;
      if(counter == 2)
        capslock = 1;
    }


  if (code == 0xF0)
  {
     release = 1; 
     return;
  }


  if (release == 1 && code == 0x12)
  {
    release = 0; 
    shift = 0;
    return;
  }
  else if(release == 1 && code == 0x14)
  {
    release = 0;
    control = 0;
    return;
  }
  else if(release == 1 && code == 0x58)
  {
    if(counter == 4)
    {
      counter = 0;
      release = 0;
      capslock = 0;
      return;
    }
  }
  else if(release)
  {
    release = 0;
    return;
  }


  if(capslock == 1)
  {
    c = utab[code];
    printf("%c", c);
    return;
  }

  if (control == 1 && code == 0x21)
  {
    printf("Control-c key\n");
    c = '\n';
    return;
  }

  if (control == 1 && code == 0x23)
  {
    printf("Control-d key\n");
    c = 4;
    return;
  }

  if(shift)
    c = utab[code];
  else
    c = ltab[code];

  if(code != 0x0)
   printf("%c", c);



  if (kp->data == 128) 
    return;

  kp->buf[kp->head++] = c;
  kp->head %= 128;

  kp->data++;
}

int kgetc()
{
  char c;
  KBD *kp = &kbd;

  while(kp->data == 0);

  lock();
    c = kp->buf[kp->tail++];
    kp->tail %= 128;
    kp->data--; kp->room++;
  unlock();
  return c;
}