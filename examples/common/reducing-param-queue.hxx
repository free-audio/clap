#pragma once

#include <cassert>

#include "reducing-param-queue.hh"

template<typename T>
ReducingParamQueue<T>::ReducingParamQueue() { reset(); }

template<typename T>
void ReducingParamQueue<T>::reset() {
   for (auto &q : _queues)
      q.clear();

   _free = &_queues[0];
   _producer = &_queues[1];
   _consumer = nullptr;
}

template<typename T>
void ReducingParamQueue<T>::setCapacity(size_t capacity) {
   for (auto &q : _queues)
      q.reserve(2 * capacity);
}

template<typename T>
void ReducingParamQueue<T>::set(clap_id id, const value_type& value) {
   _producer.load()->insert_or_assign(id, value);
}

template<typename T>
void ReducingParamQueue<T>::producerDone() {
   if (_consumer)
      return;

   auto tmp = _producer.load();
   _producer = _free.load();
   _free = nullptr;
   _consumer = tmp;

   assert(_producer);
}

template<typename T>
void ReducingParamQueue<T>::consume(const std::function<void(clap_id, const value_type& value)> consumer) {
   assert(consumer);

   if (!_consumer)
      return;

   for (auto &x : *_consumer)
      consumer(x.first, x.second);

   _consumer.load()->clear();
   if (_free)
      return;

   _free = _consumer.load();
   _consumer = nullptr;
}
