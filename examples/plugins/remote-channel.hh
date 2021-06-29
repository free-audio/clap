#pragma once

namespace clap {
   class RemoteChannel final {
   public:
      RemoteChannel(int socket);
      ~RemoteChannel();

      RemoteChannel(const RemoteChannel&) = delete;
      RemoteChannel(RemoteChannel&&) = delete;
      RemoteChannel& operator=(const RemoteChannel&) = delete;
      RemoteChannel& operator=(RemoteChannel&&) = delete;

      void close();

   private:
      int socket_;
   };
} // namespace clap