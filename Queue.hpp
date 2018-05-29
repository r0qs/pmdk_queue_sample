#include <iostream>
#include <string>
#include <libpmemobj++/make_persistent.hpp>
#include <libpmemobj++/p.hpp>
#include <libpmemobj++/persistent_ptr.hpp>
#include <libpmemobj++/pool.hpp>
#include <libpmemobj++/transaction.hpp>

using pmem::obj::p;
using pmem::obj::persistent_ptr;
using pmem::obj::pool_base;

class Queue {

  struct entry {
    pmem::obj::p<uint64_t> value;
    persistent_ptr<entry> next;
  };

  persistent_ptr<entry> head;
  persistent_ptr<entry> tail;

public:

  void push(pool_base &pool, uint64_t value);
  uint64_t pop(pool_base &pool);
  inline bool has_entry(){ return head != nullptr; }
  void show() const;
};
