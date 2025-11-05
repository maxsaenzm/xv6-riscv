
#include "kernel/types.h"
#include "user/user.h"

#define NCHILDREN 6        
#define BASE_TICKETS 50    
#define WORK 30000000U     

static void burn(unsigned int iters) {
  volatile unsigned int x = 0;
  for (unsigned int i = 0; i < iters; i++) {
    x ^= (i + (x << 1));   
  }
}

int
main(int argc, char *argv[])
{
  printf("demo: creando %d hijos, work=%u\n", NCHILDREN, (unsigned)WORK);

  for (int i = 0; i < NCHILDREN; i++) {
    int pid = fork();
    if (pid < 0) {
      printf("demo: fork fallo\n");
      exit(1);
    }
    if (pid == 0) {
      int mytickets = BASE_TICKETS * (i + 1);  // 50, 100, 150, ...
      settickets(mytickets);
      burn(WORK);
      int w = getwins(getpid());
      printf("child pid=%d tickets=%d wins=%d\n", getpid(), mytickets, w);
      exit(0);
    }
  }

  
  for (int k = 0; k < NCHILDREN; k++)
    wait(0);

  printf("demo: listo.\n");
  exit(0);
}
