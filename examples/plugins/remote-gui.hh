#pragma once

#ifdef __unix__
#   include <sys/wait.h>
#endif

#include <memory>

#include "abstract-gui.hh"
#include "remote-channel.hh"

namespace clap {
   class RemoteGui : public AbstractGui, public RemoteChannel::Handler, public RemoteChannel::EventControl {
      RemoteGui(PluginHelper &plugin) : AbstractGui(plugin) {}

      bool spawn();

      bool attachCocoa(void *nsView) noexcept override;
      bool attachWin32(clap_hwnd window) noexcept override;
      bool attachX11(const char *display_name, unsigned long window) noexcept override;

      void size(int32_t *width, int32_t *height) noexcept override;
      void setScale(double scale) noexcept override;

      bool show() noexcept override;
      bool hide() noexcept override;

      void close() noexcept override;

      // RemoteChannel::Handler
      void beginAdjust(clap_id paramId) override;
      void adjust(clap_id paramId, double value) override;
      void endAdjust(clap_id paramId) override;

      // RemoteChannel::EventControl
      void modifyFd(clap_fd_flags flags) override;

   private:
      std::unique_ptr<RemoteChannel> channel_;

#ifdef __unix__
      pid_t child_ = -1;
#else
      STARTUPINFO si;
      PROCESS_INFORMATION childInfo_;
#endif
   };
} // namespace clap