#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "syscall.h"

/*This file contains entry code for system call processing. The syscall(void) function is the entry function for all system calls. Each system call is identified by a unique integer, which is placed in the processor's eax register. The syscall function checks the integer to ensure that it is in the appropriate range and then calls the corresponding function that implements that call by making an indirect funciton call to a function in the syscalls[] table. You need to ensure that the kernel function that implements your system call is in the proper sequence in the syscalls array.
 */
// User code makes a system call with INT T_SYSCALL.
// System call number in %eax.
// Arguments on the stack, from the user call to the C
// library system call function. The saved user %esp points
// to a saved program counter, and then the first argument.

// Fetch the int at addr from the current process.
int
fetchint(uint addr, int *ip)
{
  if(addr >= proc->sz || addr+4 > proc->sz)
    return -1;
  *ip = *(int*)(addr);
  return 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Doesn't actually copy the string - just sets *pp to point at it.
// Returns length of string, not including nul.
int
fetchstr(uint addr, char **pp)
{
  char *s, *ep;

  if(addr >= proc->sz)
    return -1;
  *pp = (char*)addr;
  ep = (char*)proc->sz;
  for(s = *pp; s < ep; s++)
    if(*s == 0)
      return s - *pp;
  return -1;
}

// Fetch the nth 32-bit system call argument.
int
argint(int n, int *ip)
{
  return fetchint(proc->tf->esp + 4 + 4*n, ip);
}

// Fetch the nth word-sized system call argument as a pointer
// to a block of memory of size n bytes.  Check that the pointer
// lies within the process address space.
int
argptr(int n, char **pp, int size)
{
  int i;
  
  if(argint(n, &i) < 0)
    return -1;
  if((uint)i >= proc->sz || (uint)i+size > proc->sz)
    return -1;
  *pp = (char*)i;
  return 0;
}

// Fetch the nth word-sized system call argument as a string pointer.
// Check that the pointer is valid and the string is nul-terminated.
// (There is no shared writable memory, so the string can't change
// between this check and being used by the kernel.)
int
argstr(int n, char **pp)
{
  int addr;
  if(argint(n, &addr) < 0)
    return -1;
  return fetchstr(addr, pp);
}

extern int sys_chdir(void);
extern int sys_close(void);
extern int sys_dup(void);
extern int sys_exec(void);
extern int sys_exit(void);
extern int sys_fork(void);
extern int sys_fstat(void);
extern int sys_getpid(void);
extern int sys_kill(void);
extern int sys_link(void);
extern int sys_mkdir(void);
extern int sys_mknod(void);
extern int sys_open(void);
extern int sys_pipe(void);
extern int sys_read(void);
extern int sys_sbrk(void);
extern int sys_sleep(void);
extern int sys_unlink(void);
extern int sys_wait(void);
extern int sys_write(void);
extern int sys_uptime(void);
extern int sys_usermem(void);

static int (*syscalls[])(void) = {
[SYS_fork]    sys_fork,
[SYS_exit]    sys_exit,
[SYS_wait]    sys_wait,
[SYS_pipe]    sys_pipe,
[SYS_read]    sys_read,
[SYS_kill]    sys_kill,
[SYS_exec]    sys_exec,
[SYS_fstat]   sys_fstat,
[SYS_chdir]   sys_chdir,
[SYS_dup]     sys_dup,
[SYS_getpid]  sys_getpid,
[SYS_sbrk]    sys_sbrk,
[SYS_sleep]   sys_sleep,
[SYS_uptime]  sys_uptime,
[SYS_open]    sys_open,
[SYS_write]   sys_write,
[SYS_mknod]   sys_mknod,
[SYS_unlink]  sys_unlink,
[SYS_link]    sys_link,
[SYS_mkdir]   sys_mkdir,
[SYS_close]   sys_close,
[SYS_usermem]   sys_usermem,
};

int sys_counts[NELEM(syscalls)];
int sys_count_print_flag = 0;
int sys_count_flag = 0;
void print_sys_counts();

void
syscall(void)
{
  int num;

  num = proc->tf->eax;
  if(num > 0 && num < NELEM(syscalls) && syscalls[num]) {
    if(num == SYS_count){
      sys_count_flag = !sys_count_flag;
    }

    proc->tf->eax = syscalls[num]();
    
    if(sys_count_flag == 1) {
      sys_counts[num]++;
      sys_count_print_flag = 1;
    } else if (sys_count_print_flag) {
      print_sys_counts();
      sys_count_print_flag = 0;
    }


  } else {
    cprintf("%d %s: unknown sys call %d\n",
            proc->pid, proc->name, num);
    proc->tf->eax = -1;
  }
}

void print_sys_counts() {
  int i;
  const char *syscallnames[NELEM(syscalls) + 2];
  //I wanted a nice way to print out all the system call names. Not sure how to do it aside from hard-coding them like this.
  syscallnames[1] = "fork";
  syscallnames[2] = "exit";
  syscallnames[3] = "wait";
  syscallnames[4] = "pipe";
  syscallnames[5] = "read";
  syscallnames[6] = "kill";
  syscallnames[7] = "exec";
  syscallnames[8] = "fstat";
  syscallnames[9] = "chdir";
  syscallnames[10] = "dup";
  syscallnames[11] = "getpid";
  syscallnames[12] = "sbrk";
  syscallnames[13] = "sleep";
  syscallnames[14] = "uptime";
  syscallnames[15] = "open";
  syscallnames[16] = "write";
  syscallnames[17] = "mknod";
  syscallnames[18] = "unlink";
  syscallnames[19] = "link";
  syscallnames[20] = "mkdir";
  syscallnames[21] = "close";
  syscallnames[22] = "count";

  cprintf("Kevin_Brewton_A20311395\nSYSTEM CALL COUNTS\n");

  for (i = 1; i < NELEM(syscalls); i++) {
    cprintf("Sys call %s count = %d\n", syscallnames[i], sys_counts[i]);
  }
}
