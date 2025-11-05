#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int before = gettickets();

  if (argc == 2) {
    int n = atoi(argv[1]);
    if (n <= 0) {
      printf("ticktest: tickets debe ser > 0\n");
      exit(1);
    }
    if (settickets(n) < 0) {
      printf("ticktest: fallo settickets(%d)\n", n);
      exit(1);
    }
  } else if (argc > 2) {
    printf("uso: ticktest [tickets]\n");
    exit(1);
  }

  int after = gettickets();
  printf("PID %d -> tickets antes=%d despues=%d\n", getpid(), before, after);
  exit(0);
}


