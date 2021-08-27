#pragma once

#include <cassert>

#include "param-queue.hh"

template<typename T>
ParamQueue<T>::ParamQueue() { reset(); }

template<typename T>
void ParamQueue<T>::reset() {
   for (auto &q : queues_)
      q.clear();

   free_ = &queues_[0];
   producer_ = &queues_[1];
   consumer_ = nullptr;
}

template<typename T>
void ParamQueue<T>::setCapacity(size_t capacity) {
   for (auto &q : queues_)
      q.reserve(2 * capacity);
}

template<typename T>
void ParamQueue<T>::set(clap_id id, const value_type& value) {
   producer_.load()->emplace(id, value);
}

template<typename T>
void ParamQueue<T>::producerDone() {
   if (consumer_)
      return;

   consumer_.store(producer_.load());
   producer_.store(free_.load());
   free_.store(nullptr);

   assert(producer_);
}

template<typename T>
void ParamQueue<T>::consume(const std::function<void(clap_id, const value_type& value)> consumer) {
   assert(consumer);

   if (!consumer_)
      return;

   for (auto &x : *consumer_)
      consumer(x.first, x.second);

   consumer_.load()->clear();
   if (free_)
      return;

   free_ = consumer_.load();
   consumer_ = nullptr;
}
