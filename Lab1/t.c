#include "ext2.h"

u16 prints(char *s){
	while (*s)
        putc(*s++);
}

u16 gets(char s[ ]){
	while ((*s = getc()) != '\r')
        putc(*s++);
	*s = 0;
}

u16 getblk(u16 blk, char buf[ ]){
   readfd(blk/18, (blk/9)%2, (blk*2)%18, buf);
}

INODE *search(char *str, INODE *ip, u16 iblk)
{
    u8 i;
    DIR   *dp;
    char *cp, buf[BLK], temp[64];
    
    for (i = 0; i < 12; i++)
    {
        if (ip->i_block[i] == 0) break;
        
        getblk((u16)ip->i_block[i], buf);
        dp = (DIR *)buf;
        cp = buf;
        
        while (cp < buf + BLK)
        {
            strncpy(temp, dp->name, dp->name_len);
            temp[dp->name_len] = 0;
            
            if (!strcmp(str, temp))
            {
                i = dp->inode - 1;
                getblk((i / 8) + iblk, buf);
                //mailmans
                return (INODE *)buf + (i % 8);
            }
            cp += dp->rec_len;
            dp = (DIR*)cp;
        }
    }
    return -1;
}

main()
{

    getblk((u16)2, buf1);
    gp = (GD *)buf1;
    iblk = (u16)gp->bg_inode_table;

    getblk(iblk, buf1);
    ip = (INODE *)buf1 + 1;

    prints("Boot: ");
    gets(file);


    ip = search("boot", ip, iblk);
    if (ip == -1)
        error();


    ip = search(file, ip, iblk);
    if (ip == -1)
        error();

    i = ip->i_block[12];
    getblk(i, buf2);
    iblk2 = buf2;


    setes(0x1000);

    for (i = 0; i < 12; i++)
    {
        getblk(ip->i_block[i], 0);
        inces();
    }

    while (*iblk2 != 0)
    {
        getblk(*iblk2++, 0);
        inces();
    }
}
