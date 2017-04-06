
#include "types.h"
#include "user.h"
#include "syscall.h"

int main(int argc, char *argv[])
{
  usermem();
  printf(1, "Test User Mem Executing, Behold the Power of Virtualization!\n\n");

  int r = 200;
  int *dog = (int *)malloc(r * r * sizeof(int));
  int i, j, count = 0;
  for(j = 0; j<r; j++) {
    for (i = 0; i<r; i++) {
      *(dog + i*r + j) = ++count;
    }
  }

  printf(1, "Array DOG created.\n");

  usermem();

  printf(1, "fuck\n");
  //do soome stuff print some memory
  exit();
  return 0;
}
