#include <stdio.h>
#include <string.h>

/*********  t.c file *********************/

int prints(char *s)
{
  if (s[0] == 0)
    return -1;

  int i = 0;
    while(s[i])
    {
      putchar(s[i]);
      i++;
    }
    return 1;
}

int gets(char s[64])
{
  char input;
  int i = 0;
  input = getchar();
  putchar(input);
  while(strcmp(input, '\r') != 0)
  {
   s[i] = input;
   i++;
  }

  return 1;
}
char ans[64];

main()
{
  while(1){
    prints("What's your name? ");
    gets(ans);  prints("\n\r");

    if (ans[0]==0){
      prints("return to assembly and hang\n\r");
      return;
    }
    prints("Welcome "); prints(ans); prints("\n\r");
  }
}
  
