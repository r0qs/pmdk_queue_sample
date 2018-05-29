#include <iostream>
#include <libpmemobj++/make_persistent.hpp>
#include <libpmemobj++/persistent_ptr.hpp>
#include <libpmemobj++/pool.hpp>
#include <libpmemobj++/transaction.hpp>

#include "Queue.hpp"

using pmem::obj::p;
using pmem::obj::persistent_ptr;
using pmem::obj::pool_base;
using pmem::obj::make_persistent;
using pmem::obj::delete_persistent;
using pmem::obj::transaction;

using namespace std;

void Queue::push(pool_base &pool, uint64_t value) {
  transaction::exec_tx(pool, [&] {
    auto n = make_persistent<entry>(entry{value, nullptr});
    if (head == nullptr) {
      head = tail = n;
    } else {
      tail->next = n;
      tail = n;
    }
  });
}

uint64_t Queue::pop(pool_base &pool) {
  uint64_t value = 0;
  transaction::exec_tx(pool, [&] {
    if (head == nullptr)
      transaction::abort(EINVAL);

    value = head->value;
    auto next = head->next;

    delete_persistent<entry>(head);
    head = next;

    if (head == nullptr)
      tail = nullptr;
  });
  return value;
}

void Queue::show() const {
  for (auto n = head; n != nullptr; n = n->next)
    cout << n->value << endl;
}