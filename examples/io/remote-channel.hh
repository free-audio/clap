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
         virtual void removeFd() = 0;
      };

      struct Message final {
         uint32_t type = 0;
         uint32_t cookie = 0;
         uint32_t size = 0;
         const void *data = nullptr;

         Message() = default;

         template <typename T>
         Message(const T &msg, uint32_t c) : cookie(c) {
            set(msg);
         }

         template <typename T>
         void get(T &obj) const noexcept {
            constexpr const auto sz = sizeof(T);

            if (size != sz)
               std::terminate();

            if (type != T::type)
               std::terminate();

            std::memcpy(&obj, data, sizeof(obj));
         }

         template <typename T>
         const T &get() const noexcept {
            return *reinterpret_cast<const T *>(data);
         }

         template <typename T>
         void set(const T &msg) noexcept {
            type = T::type;
            data = &msg;
            size = sizeof(T);
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

      template <typename Request>
      bool sendRequestAsync(const Request &request) {
         return sendMessageAsync(RemoteChannel::Message(request, computeNextCookie()));
      }

      template <typename Response>
      bool sendResponseAsync(const Response &response, uint32_t cookie) {
         return sendMessageAsync(RemoteChannel::Message(response, cookie));
      }

      template <typename Request, typename Response>
      bool sendRequestSync(const Request &request, Response &response) {
         sendMessageSync(RemoteChannel::Message(request, computeNextCookie()),
                         [&response](const RemoteChannel::Message &m) { m.get(response); });
         return true;
      }

      void close();

      // Called when there is data to be read, non-blocking
      void onRead();

      // Called when data can be written, non-blocking
      void onWrite();

      // Called on socket exception
      void onError();

      void runOnce();

      clap_fd fd() const { return socket_; }
      bool isOpen() const noexcept { return socket_ != -1; }

   private:
      using ReadBuffer = Buffer<uint8_t, 128 * 1024>;
      using WriteBuffer = Buffer<uint8_t, 32 * 1024>;

      void write(const void *data, size_t size);
      WriteBuffer &nextWriteBuffer();

      void processInput();

      void modifyFd(clap_fd_flags flags);

      bool sendMessageAsync(const Message &msg);
      bool sendMessageSync(const Message &msg, const MessageHandler &handler);

      const bool cookieHalf_;
      uint32_t nextCookie_ = 0;

      MessageHandler handler_;
      std::unordered_map<uint32_t /* cookie */, const MessageHandler &> syncHandlers_;
      EventControl &evControl_;
      clap_fd socket_;
      clap_fd_flags ioFlags_ = 0;

      ReadBuffer inputBuffer_;
      std::queue<WriteBuffer> outputBuffers_;
   };
} // namespace clap