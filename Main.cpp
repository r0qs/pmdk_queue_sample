#include <iostream>
#include <string>
#include <libpmemobj++/make_persistent.hpp>
#include <libpmemobj++/persistent_ptr.hpp>
#include <libpmemobj++/pool.hpp>
#include <libpmemobj++/transaction.hpp>

#include "Queue.hpp"

using pmem::obj::persistent_ptr;
using pmem::obj::make_persistent;
using pmem::obj::transaction;

using namespace std;

const string layout = "queue_pool";

struct Queues {
  persistent_ptr<Queue> q1, q2;
};

void initialize_pool(pmem::obj::pool<Queues> pool) {
  persistent_ptr<Queues> root = pool.get_root();
  transaction::exec_tx(pool, [&] {
    root->q1 = make_persistent<Queue>();
    root->q2 = make_persistent<Queue>();
  });
  (root->q1)->push(pool, 1);
}

void show_queues(pmem::obj::pool<Queues> pool) {
  persistent_ptr<Queues> root = pool.get_root();
  (root->q1)->show();
  (root->q2)->show();
}

pmem::obj::pool<Queues> setup_pool(string path) {
  pmem::obj::pool<Queues> pool;
  try {
    if (pmem::obj::pool<Queues>::check(path, layout) == 1) {
      pool = pmem::obj::pool<Queues>::open(path, layout);
    } else {
      pool = pmem::obj::pool<Queues>::create(path, layout, PMEMOBJ_MIN_POOL, (S_IWUSR | S_IRUSR)); // write and read owner permission
      initialize_pool(pool);
    }
  } catch (pmem::pool_error &e) {
    cerr << "[ERR] Pool error: " << e.what() << endl;
  }
  return pool;
}

void pop_push_loop(pmem::obj::pool<Queues> pool, persistent_ptr<Queue> q, persistent_ptr<Queue> p) {
  for(;;) {
    try{
      transaction::exec_tx(pool, [&] {
        auto item  = q->pop(pool);
        p->push(pool,item);
      });
      transaction::exec_tx(pool, [&] {
        auto item  = p->pop(pool);
        q->push(pool,item);
      });
    } catch (pmem::transaction_error &e) {
      cerr << "[ERR] Transaction error: " << e.what() << endl;
    }
  }
}

int main(int argc, char *argv[])
{
  if (argc < 2) {
    cerr << "usage: " << argv[0] << " pool_path " << endl;
    return 1;
  }

  string pool_path = argv[1];
  pmem::obj::pool<Queues> pool = setup_pool(pool_path);
  persistent_ptr<Queues> root = pool.get_root();
  persistent_ptr<Queue> q1 = root->q1;
  persistent_ptr<Queue> q2 = root->q2;

  if (q1->has_entry() && !q2->has_entry()) {
    cout << "[INFO] Entry found in q1: " << q1->has_entry() << endl;
    pop_push_loop(pool, q1, q2);
  } else if (!q1->has_entry() && q2->has_entry()) {
    cout << "[INFO] Entry found in q2: " << q2->has_entry() << endl;
    pop_push_loop(pool, q2, q1);
  } else {
    cerr << "[FAIL] Entry not found in q1: " << q1->has_entry() << " neither q2: " << q2->has_entry() << endl;
  }
  pool.close();
  return 0;
}