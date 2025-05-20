#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int var = 20;

void *do_nothing(void* arg){
  printf("thread doing nothing...\n");
  sleep(10);
  printf("thread done doing nothing, now exiting\n");
  exit(0);
}

void *thread_func(void* arg){
  sleep(10);
  uint64 *p = (uint64*)arg;

  var++;

  printf("OTHER THREAD, %x, %x\n", *p, getpid());

  printf("Var: %d\n", var);
  exit(0);
}

// test thread creation & joining
void test1() {
  printf("TEST 1: THREAD CREATION & JOINING\n");
  uint64 p = 0xdeadbeef;
  spoon((void*)p);

  // create threads
  int t1, t2, t3 = -1;
  rthread_create((void*)&t1, (void*)do_nothing, (void*)&p);
  sleep(3);
  rthread_create((void*)&t2, (void*)do_nothing, (void*)&p);
  sleep(3);
  rthread_create((void*)&t3, (void*)do_nothing, (void*)&p);

  // verify creation
  if(t1 < 0 || t2 < 0 || t3 < 0) { 
    printf("FAILURE: thread creation\n");
    return;
  }

  // join threads
  rthread_join(t1);
  rthread_join(t2);
  rthread_join(t3);

  // graceful join if reach this point
  printf("TEST 1: SUCCESS\n");
}

int main(int argc, char *argv[]) {

  test1();

/*
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
  printf("PARENT, %x\n", t); */
  exit(0); 
}
