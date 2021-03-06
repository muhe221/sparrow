#include <stdlib.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>

#include "log.h"

static void *sub_thread(void *) {
    sleep(1);
    LOG(DEBUG) << "application sub tid:" << pthread_self();
    LOG(INFO) << "SUB_THREAD kill--------------------";
    kill(getpid(), 11);

   while (true) {
     LOG(INFO) << "thread 1 sleep()";
     sleep(3);
   }
}

static void *sub_thread2(void *) {
    sleep(1);
    LOG(DEBUG) << "application sub tid:" << pthread_self();
//    LOG(INFO) << "SUB_THREAD kill--------------------";
//    kill(getpid(), 11);

   while (true) {
     LOG(INFO) << "thread 2 sleep()";
     sleep(3);
   }
}


static void application() {
   LOG(INFO) << "to sleep 5 s";
   LOG(DEBUG) << "application main tid:" << pthread_self();
   sleep(2);
   pthread_t pid1;
   pthread_create(&pid1, NULL, &sub_thread, (void *) NULL);


   pthread_t pid2;
   pthread_create(&pid2, NULL, &sub_thread2, (void *) NULL);
/*
  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, SIGSEGV);
  sigprocmask(SIG_BLOCK, &set, NULL);
*/
   while (true) {
     LOG(INFO) << "application sleep()";
     sleep(2);
   }
}


int onStart() {
  application();
  LOG(FATAL) << "exit(1)";
  exit(1);
}

