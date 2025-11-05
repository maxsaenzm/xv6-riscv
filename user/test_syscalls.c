#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  printf("PID actual: %d\n", getpid());
  printf("PPID (con getppid): %d\n", getppid());

  printf("Ancestro 1 (padre): %d\n", getancestor(1));
  printf("Ancestro 2 (abuelo): %d\n", getancestor(2));

  exit(0);
}

