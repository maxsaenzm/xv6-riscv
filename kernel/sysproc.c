#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"


extern struct proc proc[NPROC];


uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if(t == SBRK_EAGER || n < 0) {
    if(growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if(addr + n < addr)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
// --- T1: nuevas syscalls ---
extern struct proc *myproc(void);

uint64
sys_getppid(void)
{
  struct proc *p = myproc();
  if(p->parent)
    return p->parent->pid;
  return -1;
}

uint64
sys_getancestor(void)
{
  int n;

  argint(0, &n);          // solo carga el argumento 0 en n (SIN if)
  if(n < 0) return -1;

  struct proc *p = myproc();
  for(int i = 0; i < n; i++){
    if(p->parent == 0) return -1;
    p = p->parent;
  }
  return p->pid;
}

uint64
sys_settickets(void)
{
  int n;
  argint(0, &n);              // lee el 1er argumento
  if (n < 1)                  // valida
    return -1;

  struct proc *p = myproc();
  acquire(&p->lock);
  p->tickets = n;
  release(&p->lock);
  return 0;
}


uint64
sys_gettickets(void)
{
  struct proc *p = myproc();
  int t;

  acquire(&p->lock);
  t = p->tickets;
  release(&p->lock);

  return t;
}


uint64
sys_getwins(void)
{
  int pid;
  if (argint(0, &pid) < 0) return -1;

  struct proc *p;
  for (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    if (p->pid == pid) {
      int w = p->wins;
      release(&p->lock);
      return w;
    }
    release(&p->lock);
  }
  return -1;
}


uint64
sys_mrdprotect(void)
{
  uint64 addr;
  int len;

  if(argaddr(0, &addr) < 0 || argint(1, &len) < 0)
    return -1;

  return mrdprotect(addr, len);
}

uint64
sys_munrdprotect(void)
{
  uint64 addr;
  int len;

  if(argaddr(0, &addr) < 0 || argint(1, &len) < 0)
    return -1;

  return munrdprotect(addr, len);
}


