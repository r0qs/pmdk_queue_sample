# PMDK fault-resilient Queue sample

The goal of this task is to create a small fault-resilient program using libpmemobj++, which are part of PMDK.

PMDK works on Non-Volatile Memory (NVM) but it can use regular DRAM as well by setting the environment variable in your shell:
```shell
$ export PMEM_IS_PMEM_FORCE=1
```
The program have two persistent queue instances, q1 and q2, and starts putting an item in q1. Then run an infinite loop making two transactions, the first dequeues the item from q1 and enqueues it in q2. The other, dequeues the item from q2 and enqueues in q1.

Upon starting, the program checks if there is already a pool file in /dev/shm/queue_pool and use it if it's there. Then, it checks that the item is either in q1 or q2. If it's in neither then something went very wrong, the item must be in one (and only one) of the queues, because this is intend to be a fault-resilient program! So please if you found an error, let me know!

Repeat the ctrl+c and restart procedure several times, so as to allow the execution to terminate in different lines of code. The program must restart correctly everytime, with the item in one of the queues, depending on which of the two transactions was last committed.

# Compile and run

```shell
$ make
$ ./queue /dev/shm/queue_pool
```
