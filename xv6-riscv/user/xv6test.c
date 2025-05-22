#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include <stddef.h>

int var = 20;
int *page = (int*)0xdeadbeef;

void *do_nothing(void* arg){
  sleep(10);
  exit(0);
}

void *add_to_var(void* arg){
  sleep(10);
  var++;
  exit(0);
}

void *read_from_stack(void* arg) {
  uint64 *var_addr = (uint64*)arg;
  int my_var = *var_addr;
  if(my_var != 123)
    printf("FAILED TO READ ANOTHER THREAD'S DATA\n");
  exit(0);
}

void *write_to_stack(void* arg) {
  int local_var = 123;
  int tid;
  rthread_create((void*)&tid, (void*)read_from_stack, (void*)&local_var);
  rthread_join(tid);
  exit(0);
}

void *allocate_page(void* arg) {
  page = (int*)sbrk(4096);
  page[0] = 3;
  page[1] = 2;
  exit(0);
}

void *check_page(void* arg) {
  sleep(50);
  if(*page == 0xdeadbeef) {
    printf("FAILURE: TEST 4 (pagetable update not read)\n");
  } else if (page[0] == 3 && page[1] == 2) {
    printf("TEST 4: SUCCESS\n");
  } else {
    printf("FAILURE: TEST 4 (wrong values read from page)\n");
  }
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

// test shared memory
void test2() {

 printf("TEST 2: SHARED MEMORY\n");
 uint64 p = 0xdeadbeef;
 int t1, t2, t3, t4;
 rthread_create((void*)&t1, (void*)add_to_var, (void*)&p);
 sleep(3);
 rthread_create((void*)&t2, (void*)add_to_var, (void*)&p);
 sleep(3);
 rthread_create((void*)&t3, (void*)add_to_var, (void*)&p);
 sleep(3);
 rthread_create((void*)&t4, (void*)add_to_var, (void*)&p);

 rthread_join(t1);
 rthread_join(t2);
 rthread_join(t3);
 rthread_join(t4);

 if(var == 24) {
   printf("TEST 2: SUCCESS\n");
 } else {
   printf("FAILURE: SHARED MEMORY; expected var = 24, got var = %d\n", var);
 }

}

// make sure threads can access each others' stack
void test3() {
  printf("TEST 3: STACK ACCESS\n");
  int tid;
  rthread_create((void*)&tid, (void*)write_to_stack, NULL);
  rthread_join(tid);
  
  // if no error from thread, success
  printf("TEST 3: SUCCESS (if no errors above)\n");
}

// test pagetable updates
void test4() {
  int t1, t2;
  rthread_create((void*)&t1, (void*)allocate_page, NULL);
  rthread_create((void*)&t2, (void*)check_page, NULL);
  rthread_join(t1);
  rthread_join(t2);
}

int main(int argc, char *argv[]) {

  test1();
  test2();
  test3();
  test4();
  
  exit(0); 
}
