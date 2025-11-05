#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char **argv)
{
  int pid = getpid();

  // Trabajo tonto para consumir CPU y acumular "wins"
  volatile int sink = 0;
  for (int i = 0; i < 50000000; i++)
    sink += i;

  int w = getwins(pid);
  if (w < 0) {
    printf("getwins fallo para pid=%d\n", pid);
  } else {
    printf("pid %d -> wins=%d\n", pid, w);
  }
  exit(0);
}

