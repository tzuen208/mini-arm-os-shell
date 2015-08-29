#ifndef __THREADS_H__
#define __THREADS_H__

void thread_start();
int thread_create(void (*run)(void*), void* userdata);
int thread_create_int(void (*run)(int), int* userdata);
int thread_create_int2(void (*run)(int, int), int* userdata, int* userdata2);
void thread_kill(int thread_id);
void thread_self_terminal();

#endif
