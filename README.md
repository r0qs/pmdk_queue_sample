# PMDK fault-resilient Queue sample

The goal of this task is to create a small fault-resilient program using libpmemobj or libpmemobj++, which are part of PMDK.

PMDK works on Non-Volatile Memory (NVM) but it can use regular DRAM as well by setting the environment variable in your shell:
```shell
$ export PMEM_IS_PMEM_FORCE=1
```
The program have two persistent queue instances, q1 and q2. And run a inifinite loop that place one item in q1 and start a transaction that dequeues from q1 and enqueues the dequeued item in q2. Then make another transaction that dequeues the item from q2 and enqueues the item in q1. Run for some seconds and kill the process from another shell.

Upon starting, the program checks if there is already a pool file in /dev/shm/ and use it if it's there. Then, it checks that the item is either in q1 or q2. If it's in neither then something goes very wrong, the item must be in one (and only one) of the queues, because this is intend to be a fault-resilient program!

Repeat the ctrl+c and restart procedure several times, so as to allow the execution to terminate in different lines of code. The program must restart correctly everytime, with the item in one of the queues, depending on which of the two transactions was last committed.

# Compile and run

```shell
$ make
$ ./queue /dev/shm/queue_pool
```
