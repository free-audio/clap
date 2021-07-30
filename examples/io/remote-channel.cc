#ifdef __unix__
#   include <errno.h>
#   include <poll.h>
#   include <unistd.h>
#endif

#include "remote-channel.hh"

namespace clap {

   RemoteChannel::RemoteChannel(const MessageHandler &handler,
                                EventControl &evControl,
                                int socket,
                                bool cookieHalf)
      : cookieHalf_(cookieHalf), handler_(handler), evControl_(evControl), socket_(socket) {}

   RemoteChannel::~RemoteChannel() { close(); }

   void RemoteChannel::onRead() {
      ssize_t nbytes = ::read(socket_, inputBuffer_.writeData(), inputBuffer_.writeAvail());
      if (nbytes < 0) {
         if (errno == EWOULDBLOCK || errno == EAGAIN || errno == EINTR)
            return;

         close();
         return;
      }

      inputBuffer_.wrote(nbytes);
      processInput();
   }

   void RemoteChannel::write(const void *_data, size_t size) {
      const uint8_t *data = static_cast<const uint8_t *>(_data);
      while (size > 0) {
         auto &buffer = nextWriteBuffer();
         buffer.write(data, size);
      }

      assert(size == 0);
   }

   RemoteChannel::WriteBuffer &RemoteChannel::nextWriteBuffer() {
      if (outputBuffers_.empty()) {
         outputBuffers_.emplace();
         return outputBuffers_.back();
      }

      auto &buffer = outputBuffers_.back();
      if (buffer.writeAvail() > 0)
         return buffer;

      outputBuffers_.emplace();
      return outputBuffers_.back();
   }

   void RemoteChannel::onWrite() {
      while (!outputBuffers_.empty()) {
         auto &buffer = outputBuffers_.front();

         auto avail = buffer.readAvail();
         while (avail > 0) {
            auto nbytes = ::write(socket_, buffer.readData(), avail);
            if (nbytes == -1) {
               if (errno == EWOULDBLOCK || errno == EAGAIN || errno == EINTR) {
                  modifyFd(CLAP_FD_READ | CLAP_FD_WRITE);
                  return;
               }

               close();
               return;
            }

            buffer.wrote(nbytes);
            avail -= nbytes;
            assert(avail == buffer.readAvail());
         }

         outputBuffers_.pop();
      }

      modifyFd(CLAP_FD_READ);
   }

   void RemoteChannel::close() {
      if (socket_ == -1)
         return;

      ::close(socket_);
      socket_ = -1;
   }

   uint32_t RemoteChannel::computeNextCookie() noexcept {
      uint32_t cookie = nextCookie_;
      if (cookieHalf_)
         cookie |= (1ULL << 31);
      else
         cookie &= ~(1ULL << 31);

      ++nextCookie_; // overflow is fine
      return cookie;
   }

   void RemoteChannel::processInput() {
      while (inputBuffer_.readAvail() > 12) {
         const auto *data = inputBuffer_.readData();
         Message msg;

         std::memcpy(&msg.type, data, 4);
         std::memcpy(&msg.cookie, data + 4, 4);
         std::memcpy(&msg.size, data + 8, 4);
         msg.data = data + 12;

         uint32_t totalSize = 12 + msg.size;
         if (inputBuffer_.readAvail() < totalSize)
            return;

         auto it = syncHandlers_.find(msg.cookie);
         if (it != syncHandlers_.end()) {
            it->second(msg);
            syncHandlers_.erase(msg.cookie);
         } else {
            handler_(msg);
         }

         inputBuffer_.read(totalSize);
      }

      inputBuffer_.rewind();
   }

   bool RemoteChannel::sendMessageAsync(const Message &msg) {
      write(&msg.type, sizeof(msg.type));
      write(&msg.cookie, sizeof(msg.cookie));
      write(&msg.size, sizeof(msg.size));
      write(msg.data, msg.size);
      onWrite();
      return true;
   }

   bool RemoteChannel::sendMessageSync(const Message &msg, const MessageHandler &handler) {
      sendMessageAsync(msg);

      syncHandlers_.emplace(msg.cookie, handler);

      while (syncHandlers_.count(msg.cookie) > 0)
         runOnce();

      syncHandlers_.erase(msg.cookie);
      return true;
   }

   void RemoteChannel::modifyFd(clap_fd_flags flags)
   {
      if (flags == ioFlags_)
         return;

      ioFlags_ = flags;
      evControl_.modifyFd(flags);
   }

   void RemoteChannel::runOnce() {
#ifdef __unix__
      pollfd pfd;
      pfd.fd = socket_;
      pfd.events = POLLIN | (ioFlags_ & CLAP_FD_WRITE ? POLLOUT : 0);
      pfd.revents = 0;

      int ret = ::poll(&pfd, 1, 0);
      if (ret < 1)
         // TODO error handling
         return;

      if (pfd.revents & POLLOUT)
         onWrite();
      if (pfd.revents & POLLIN)
         onRead();
#endif
   }
} // namespace clap