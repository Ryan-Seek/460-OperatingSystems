#include "ucode.c"

char *newargs[10];
char *iargs[10];
int icount;
int parseCount;


int finddelim(char *line, char delim)
{
    char *cp;
    cp = line;
    int ret = 0;
    while(*cp && *cp != delim)
    {
        *cp++;
        ret++;
    }
    return ret;
}

void tokenize2(char *line, char delim)
{
  char *cp;
  cp = line;
  icount = 0;
  
  while (*cp != 0){
       while (*cp == delim) *cp++ = 0;        
       if (*cp != 0)
           iargs[icount++] = cp;         
       while (*cp != delim && *cp != 0) cp++;                  
       if (*cp != 0)   
           *cp = 0;                   
       else 
            break; 
       cp++;
  }
  iargs[icount] = 0;
}

void correctInput2(int num)
{
    char *cp;
    cp = iargs[num];
    while(*cp == ' ')
    {
        *cp++;
    }
    iargs[num] = cp;
}

int do_indirects(int n)
{
    int len = strlen(newargs[n]);
    int i =0, check, fd;
    char hold[64];
    strcpy(hold, newargs[n]);
    check = finddelim(hold, '<');
    if(check == len)
    {
        check = finddelim(hold, '>');

        if(check == len)
            return exec(newargs[n]);

        else if(newargs[n][check+1] == '>')
        {
            memset(hold, 0, 64);
            strcpy(hold, newargs[n]);
            tokenize2(hold, '>');

            for(i = 0; i < icount; i++)
                correctInput2(i);

            fd = open(iargs[1], O_WRONLY | O_CREAT | O_APPEND);

            if(fd < 0)
                exit(1);

            dup2(fd, 1);
            close(fd);
            return exec(iargs[0]);
        }
        else
        {
            memset(hold, 0, 64);
            strcpy(hold, newargs[n]);
            tokenize2(hold, '>');

            for(i = 0; i < icount; i++)
                correctInput2(i);

            fd = open(iargs[1], O_WRONLY | O_CREAT);

            if(fd < 0)
                exit(1);

            dup2(fd, 1);
            close(fd);
            return exec(iargs[0]);
        }
    }
    else
    {
        memset(hold, 0, 64);
        strcpy(hold, newargs[n]);
        tokenize2(hold, '<');
        for(i = 0; i < icount; i++)
        {
            correctInput2(i);
        }
        fd = open(iargs[1], O_RDONLY);
        if(fd < 0)
        {
            exit(1);
        }
        dup2(fd, 0);
        close(fd);
        return exec(iargs[0]);
    }
}

void tokenize(char *line, char delim)
{
  char *cp;
  cp = line;
  parseCount = 0;

  while (*cp != 0){
       while (*cp == delim)
            *cp++ = 0;

       if (*cp != 0)
           newargs[parseCount++] = cp;

       while (*cp != delim && *cp != 0)
            cp++;

       if (*cp != 0)
           *cp = 0;
       else
            break;

       cp++;
  }
  newargs[parseCount] = 0;
}

void correctInput(int num)
{
    char *cp;
    cp = newargs[num];

    while(*cp == ' ')
        *cp++;

    newargs[num] = cp;
}

int do_pipe(int o, int t)//intially 0,1
{
    int i = 1;
    int pid, pd[2];
    int status;

    for(i = 1; i < parseCount; i++)
        correctInput(i);

    pipe(pd);

    pid = fork();
    if(pid)
    {
        close(pd[1]); 
        dup2(pd[0], 0);

        if(t < parseCount - 1)//checks to see if there are more than one pipe
            do_pipe(t, t+1);//recursively process pipes

        do_indirects(t);//only one pipe
    }
    else
    {
        close(pd[0]); 
        dup2(pd[1], 1);
        do_indirects(o);
    }
}

int execute(char *command)
{
    char hold[128];
    strcpy(hold, command);
    tokenize(hold, '|');//tokenize for pipes
    if(parseCount > 1)//pipes exist in current command
    {
        return do_pipe(0, 1);
    }
    return do_indirects(0);
}


int main(int argc, char *argv[ ])
{
    char command[128], *token, splitCommand[128], cur[128];
    int shell = getpid(), pid, status, id;
    // Main shell loop, never ends.
    while(1)
    {
        memset(command, 0, 128);

        while(!command[0])//print to screen and await input
        {
            printf("Ryan's sh #%d : ", shell);
            gets(command);
        }

        if(strcmp(command, "logout") == 0)
            exit(1);

        if(strcmp(command, "") == 0)
            continue;

        strcpy(splitCommand, command);
        tokenize(splitCommand, ' ');//break command apart

        if(strcmp(newargs[0], "cd") == 0)
        {
            if(newargs[1])
                chdir(newargs[1]);

            else
                chdir("/");

            continue;
        }
        if(strcmp(newargs[0], "pwd") == 0)
        {
            pwd();
            continue;
        }

        pid = fork();

        if(pid)
            pid = wait(&status);//wait for proc to die

        else
        {
            strcpy(cur, command);

            id = execute(cur);

            if(id == -1)//commmand failed
                exit(1);
        }
    }
}

