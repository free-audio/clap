#pragma once

#include <atomic>
#include <functional>
#include <unordered_map>

#include <clap/all.h>

template <typename T>
class ParamQueue {
public:
   using value_type = T;
   using queue_type = std::unordered_map<clap_id, value_type>;

   ParamQueue();

   void setCapacity(size_t capacity);

   void set(clap_id id, const value_type& value);
   void producerDone();

   void consume(const std::function<void(clap_id id, const value_type& value)> consumer);

   void reset();

private:
   queue_type queues_[2];
   std::atomic<queue_type *> free_ = nullptr;
   std::atomic<queue_type *> producer_ = nullptr;
   std::atomic<queue_type *> consumer_ = nullptr;
};