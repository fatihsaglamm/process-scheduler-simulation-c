# Process Scheduler Simulation in C

A process scheduling simulation developed in C using POSIX processes, shared memory, and threads.

## Features

- Random process generation
- Arrival-time sorting
- Priority-based sorting
- Shortest Process Next scheduling
- Preemptive Priority Scheduling
- Turnaround and waiting time calculation
- Process creation with `fork()`
- Shared memory using `mmap()`
- POSIX threads using `pthread_create()` and `pthread_join()`

## Build

```bash
gcc process_scheduler.c -o process_scheduler -pthread