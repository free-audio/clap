#ifdef __unix__
#   include <unistd.h>
#endif

#include "remote-channel.hh"

namespace clap {

   RemoteChannel::RemoteChannel(int socket) : socket_(socket) {}

   RemoteChannel::~RemoteChannel() { close(); }

   void RemoteChannel::close() {
      if (socket_ == -1)
         return;

      ::close(socket_);
      socket_ = -1;
   }
} // namespace clap