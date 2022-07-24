/*
 *  * xname : change window and icom name of xterm
 *   * author: Morgan Burke (TRIUMF) 1992-June-22
 *    */

#include <stdio.h>
#include <stdlib.h>

int main ( argc, argv )
     unsigned int argc;
     char **argv;
{
  int i;
  char name[80];
  sprintf(name, "%s", argv[1]);
  for (i = 2; i < argc; i++)
    {
    sprintf(name, "%s %s", name, argv[i]);
    }
  printf("%c]0;%s%c", (char) 27, name, (char) 7);
  exit(0);
}

