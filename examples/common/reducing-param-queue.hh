#pragma once

#include <atomic>
#include <functional>
#include <unordered_map>

#include <clap/all.h>

template <typename T>
class ReducingParamQueue {
public:
   using value_type = T;
   using queue_type = std::unordered_map<clap_id, value_type>;

   ReducingParamQueue();

   void setCapacity(size_t capacity);

   void set(clap_id id, const value_type &value);
   void producerDone();

   void consume(const std::function<void(clap_id id, const value_type &value)> consumer);

   void reset();

private:
   queue_type _queues[2];
   std::atomic<queue_type *> _free = nullptr;
   std::atomic<queue_type *> _producer = nullptr;
   std::atomic<queue_type *> _consumer = nullptr;
};