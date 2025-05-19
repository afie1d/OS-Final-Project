#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int var = 20;

void *thread_func(void* arg){
  sleep(10);
  uint64 *p = (uint64*)arg;

  var++;

  printf("OTHER THREAD, %x, %x\n", *p, getpid());

  printf("Var: %d\n", var);
  exit(0);
}

int main(int argc, char *argv[]) {
  printf("Var: %d\n", var);
  uint64 p = 0xdeadbeef;

  spoon((void*)p);
  
  int t, t2;
  printf("Starting o thread\n");
  rthread_create((void*)&t, (void*)thread_func, (void*)&p);
  sleep(5);
  rthread_create((void*)&t2, (void*)thread_func, (void*)&p);
  rthread_join(t);
  rthread_join(t2);
  //sleep(2);
  printf("PARENT, %x\n", t);
  exit(0);
}
