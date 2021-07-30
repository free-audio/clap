#pragma once

#include <cstring>
#include <functional>
#include <memory>
#include <queue>
#include <unordered_map>

#include <clap/all.h>

#include "buffer.hh"

namespace clap {
   class RemoteChannel final {
   public:
      class EventControl {
      public:
         virtual void modifyFd(clap_fd_flags flags) = 0;
      };

      struct Message final {
         uint32_t type;
         uint32_t cookie;
         uint32_t size;
         const void *data;

         template <typename T>
         Message(const T &msg, uint32_t c) : cookie(c) {
            set(msg);
         }

         template <typename T>
         const T &get() const noexcept {
            return *reinterpret_cast<const T *>(data);
         }

         template <typename T>
         void set(const T &msg) noexcept {
            type = T::type;
            data = &msg;
         }
      };

      using MessageHandler = std::function<void(const Message &response)>;

      RemoteChannel(const MessageHandler &handler,
                    EventControl &evControl,
                    clap_fd socket,
                    bool cookieHalf);
      ~RemoteChannel();

      RemoteChannel(const RemoteChannel &) = delete;
      RemoteChannel(RemoteChannel &&) = delete;
      RemoteChannel &operator=(const RemoteChannel &) = delete;
      RemoteChannel &operator=(RemoteChannel &&) = delete;

      uint32_t computeNextCookie() noexcept;

      bool sendMessageAsync(const Message &msg);
      bool sendMessageSync(const Message &msg, const MessageHandler &handler);

      void close();

      // Called when there is data to be read, non-blocking
      void onRead();

      // Called when data can be written, non-blocking
      void onWrite();

   private:
      using ReadBuffer = Buffer<uint8_t, 128 * 1024>;
      using WriteBuffer = Buffer<uint8_t, 32 * 1024>;

      void write(const void *data, size_t size);
      WriteBuffer &nextWriteBuffer();

      void parseInput();

      const bool cookieHalf_;
      uint32_t nextCookie_ = 0;

      const MessageHandler &handler_;
      std::unordered_map<uint32_t /* cookie */, MessageHandler &> syncHandlers_;
      EventControl &evControl_;
      clap_fd socket_;

      ReadBuffer inputBuffer_;
      std::queue<WriteBuffer> outputBuffers_;
   };
} // namespace clap