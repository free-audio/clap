#pragma once

#include <queue>
#include <memory>

#include <clap/all.h>

#include "buffer.hh"

namespace clap {
   class RemoteChannel final {
   public:
      class EventControl {
      public:
         virtual void modifyFd(clap_fd_flags flags) = 0;
      };

      class Handler {
      public:
         virtual ~Handler() = default;

         // GUI callbacks
         virtual void defineParameter(const clap_param_info &info) {}
         virtual bool attachCocoa(void *nsView) { return false; }
         virtual bool attachWin32(clap_hwnd window) { return false; }
         virtual bool attachX11(const char *display_name, unsigned long window) { return false; }

         virtual void size(int32_t *width, int32_t *height) {}
         virtual void setScale(double scale) {}

         virtual bool show() { return false; }
         virtual bool hide() { return false; }

         virtual void close() {}

         // Plugin callbacks
         virtual void beginAdjust(clap_id paramId) {}
         virtual void adjust(clap_id paramId, double value) {}
         virtual void endAdjust(clap_id paramId) {}
      };

      RemoteChannel(Handler &handler, EventControl &evControl, clap_fd socket);
      ~RemoteChannel();

      RemoteChannel(const RemoteChannel &) = delete;
      RemoteChannel(RemoteChannel &&) = delete;
      RemoteChannel &operator=(const RemoteChannel &) = delete;
      RemoteChannel &operator=(RemoteChannel &&) = delete;

      void defineParameter(const clap_param_info &info);
      void setParameterValue(clap_id paramId, double value);

      void close();

      // Called when there is data to be read, non-blocking
      void onRead();

      // Called when data can be written, non-blocking
      void onWrite();

   private:
      using ReadBuffer = Buffer<uint8_t, 128 * 1024>;
      using WriteBuffer = Buffer<uint8_t, 32 * 1024>;

      void write(const uint8_t *data, size_t size);
      WriteBuffer& nextWriteBuffer();

      void parseInput();

      Handler &handler_;
      EventControl &evControl_;
      clap_fd socket_;

      ReadBuffer inputBuffer_;
      std::queue<WriteBuffer> outputBuffers_;
   };
} // namespace clap