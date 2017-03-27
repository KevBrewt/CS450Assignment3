#include "types.h"
#include "user.h"
#include "syscall.h"

int main(int argc, char *argv[])
{
  printf(1, "Initiate syscall counting\n");
  count();
  printf(1, "Forking!\n");
  getpid();
  int pid;
  if ((pid = fork()) == 0) {
    printf(1, "child fork\n");
    getpid();
    sleep(1);
    exit();
  } else {
    wait();
    printf(1, "Parent fork\n");
  }
  sleep(1);
  sleep(1);
  
  getpid();
  if(pid != 0) {
    wait();
    printf(1, "Stopping syscount now!\n");
    count();
  }
  exit();
}
