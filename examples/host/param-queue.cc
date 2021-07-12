#include <QDebug>

#include "param-queue.hh"

ParamQueue::ParamQueue() { reset(); }

void ParamQueue::reset() {
   for (auto &q : queues_)
      q.clear();

   free_ = &queues_[0];
   producer_ = &queues_[1];
   consumer_ = nullptr;
}

void ParamQueue::setCapacity(size_t capacity) {
   for (auto &q : queues_)
      q.reserve(2 * capacity);
}

void ParamQueue::set(clap_id id, void *cookie, double value) {
   producer_.load()->emplace(id, value_type{cookie, value});
}

void ParamQueue::producerDone() {
   if (consumer_)
      return;

   consumer_.store(producer_.load());
   producer_.store(free_.load());
   free_.store(nullptr);

   Q_ASSERT(producer_);
}

void ParamQueue::consume(const std::function<void(clap_id, void *, double)> consumer) {
   Q_ASSERT(consumer);

   if (!consumer_)
      return;

   for (auto &x : *consumer_)
      consumer(x.first, x.second.cookie, x.second.value);

   consumer_.load()->clear();
   if (free_)
      return;

   free_ = consumer_.load();
   consumer_ = nullptr;
}
