#ifdef __unix__
#   include <errno.h>
#   include <fcntl.h>
#   include <poll.h>
#   include <unistd.h>
#endif

#include "remote-channel.hh"

namespace clap {

   RemoteChannel::RemoteChannel(const MessageHandler &handler,
                                EventControl &evControl,
                                int socket,
                                bool cookieHalf)
      : _cookieHalf(cookieHalf), _handler(handler), _evControl(evControl), _socket(socket) {
#ifdef __unix__
      int flags = ::fcntl(_socket, F_GETFL);
      ::fcntl(_socket, F_SETFL, flags | O_NONBLOCK);
#endif
   }

   RemoteChannel::~RemoteChannel() { close(); }

   void RemoteChannel::onRead() {
      ssize_t nbytes = ::read(_socket, _inputBuffer.writePtr(), _inputBuffer.writeAvail());
      if (nbytes < 0) {
         if (errno == EWOULDBLOCK || errno == EAGAIN || errno == EINTR)
            return;

         close();
         return;
      }

      if (nbytes == 0) {
         close();
         return;
      }

      _inputBuffer.wrote(nbytes);
      processInput();
      _inputBuffer.rewind();
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
      if (_outputBuffers.empty()) {
         _outputBuffers.emplace();
         return _outputBuffers.back();
      }

      auto &buffer = _outputBuffers.back();
      if (buffer.writeAvail() > 0)
         return buffer;

      _outputBuffers.emplace();
      return _outputBuffers.back();
   }

   void RemoteChannel::onWrite() {
      while (!_outputBuffers.empty()) {
         auto &buffer = _outputBuffers.front();

         for (auto avail = buffer.readAvail(); avail > 0; avail = buffer.readAvail()) {
            auto nbytes = ::write(_socket, buffer.readPtr(), avail);
            if (nbytes == -1) {
               if (errno == EWOULDBLOCK || errno == EAGAIN || errno == EINTR) {
                  modifyFd(CLAP_FD_READ | CLAP_FD_WRITE);
                  return;
               }

               close();
               return;
            }

            buffer.read(nbytes);
         }

         _outputBuffers.pop();
      }

      modifyFd(CLAP_FD_READ);
   }

   void RemoteChannel::onError() { close(); }

   void RemoteChannel::close() {
      if (_socket == -1)
         return;

      _evControl.removeFd();

      ::close(_socket);
      _socket = -1;
   }

   uint32_t RemoteChannel::computeNextCookie() noexcept {
      uint32_t cookie = _nextCookie;
      if (_cookieHalf)
         cookie |= (1ULL << 31);
      else
         cookie &= ~(1ULL << 31);

      ++_nextCookie; // overflow is fine
      return cookie;
   }

   void RemoteChannel::processInput() {
      while (_inputBuffer.readAvail() >= 12) {
         const auto *data = _inputBuffer.readPtr();
         Message msg;

         std::memcpy(&msg.type, data, 4);
         std::memcpy(&msg.cookie, data + 4, 4);
         std::memcpy(&msg.size, data + 8, 4);
         msg.data = data + 12;

         uint32_t totalSize = 12 + msg.size;
         if (_inputBuffer.readAvail() < totalSize)
            return;

         auto it = _syncHandlers.find(msg.cookie);
         if (it != _syncHandlers.end()) {
            it->second(msg);
            _syncHandlers.erase(it);
         } else {
            _handler(msg);
         }

         _inputBuffer.read(totalSize);
      }
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

      auto it = _syncHandlers.emplace(msg.cookie, handler);
      assert(it.second);
      if (!it.second)
         return false;

      while (_syncHandlers.count(msg.cookie) > 0)
         runOnce();

      _syncHandlers.erase(msg.cookie);
      return true;
   }

   void RemoteChannel::modifyFd(clap_fd_flags flags) {
      if (flags == _ioFlags)
         return;

      _ioFlags = flags;
      _evControl.modifyFd(flags);
   }

   void RemoteChannel::runOnce() {
      if (!isOpen())
         return;

#ifdef __unix__
      pollfd pfd;
      pfd.fd = _socket;
      pfd.events = POLLIN | (_ioFlags & CLAP_FD_WRITE ? POLLOUT : 0);
      pfd.revents = 0;

      int ret = ::poll(&pfd, 1, -1);
      if (ret < 1) {
         if (errno == EAGAIN || errno == EINTR)
            return;
         close();
         return;
      }

      if (pfd.revents & POLLOUT)
         onWrite();
      if (isOpen() && pfd.revents & POLLIN)
         onRead();
      if (isOpen() && pfd.revents & POLLERR)
         close();
#endif
   }
} // namespace clap
