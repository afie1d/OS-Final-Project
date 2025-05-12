#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void *thread_func(void* arg){
  sleep(1);
  uint64 *p = (uint64*)arg;
  printf("OTHER THREAD, %x, %x\n", *p, getpid());
  exit(0);
}

int main(int argc, char *argv[]) {
  uint64 p = 0xdeadbeef;

  spoon((void*)p);
  
  int t;
  printf("Starting o thread\n");
  rthread_create((void*)&t, (void*)thread_func, (void*)&p);
  //rthread_join(&t);
  sleep(2);
  printf("PARENT, %x\n", t);
  exit(0);
}
