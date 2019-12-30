#define DR   0x00
#define FR   0x18
#define RXFE 0x10
#define RXFF 0x40
#define TXFF 0x20

typedef volatile struct uart{
  char *base;
  int n;
}UART;

UART uart[4];

int uart_init()
{
  int i; UART *up;

  for (i=0; i<4; i++)	//uart0 to uart2 are adjacent
  {
    up = &uart[i];
    up->base = (char *)(0x101F1000 + i*0x1000);
    up->n = i;
  }
  //uart[3].base = (char *)(0x10009000);
}

//input a char from UART pointed by up
int ugetc(UART *up)
{
 // printf("BEGINNING OF UGETC\n");
  while (*(up->base + FR) & RXFE);	//loop if UFR is RXFE ???????//stuck on line 30???????
 // printf("IN UGETC\n");
  return *(up->base + DR);			//return a char in UDR
}

//output a char to UART pointed by up
int uputc(UART *up, char c)
{
//  printf("IN UPUTC\n");
  while(*(up->base + FR) & TXFF);	//loop if UFR is TXFF
  *(up->base + DR) = c;				//white char to data register
}

//input a string of chars
int ugets(UART *up, char *s)	
{
  while ((*s = ugetc(up)) != '\r'){
//    printf("IN UGETS MIDDLE\n");
    uputc(up, *s);
    s++;
  }
 // printf("END OF UGETS\n");
  *s = 0;
}

//output a string of chars
int uprints(UART *up, char *s)
{
  while(*s)
    uputc(up, *s++);
}
