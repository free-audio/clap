#pragma once

#ifdef __unix__
#   include <sys/wait.h>
#endif

#include <memory>

#include "abstract-gui.hh"
#include "remote-channel.hh"

namespace clap {
   class RemoteGui : public AbstractGui, public RemoteChannel::EventControl {
   public:
      RemoteGui(PluginHelper &plugin) : AbstractGui(plugin) {}

      bool spawn();

      bool attachCocoa(void *nsView) noexcept override;
      bool attachWin32(clap_hwnd window) noexcept override;
      bool attachX11(const char *display_name, unsigned long window) noexcept override;

      bool size(uint32_t *width, uint32_t *height) noexcept override;
      void setScale(double scale) noexcept override;

      bool show() noexcept override;
      bool hide() noexcept override;

      void close() noexcept override;

      // RemoteChannel::EventControl
      void modifyFd(clap_fd_flags flags) override;

   private:
      void onMessage(const RemoteChannel::Message& msg);

      std::unique_ptr<RemoteChannel> channel_;

#ifdef __unix__
      pid_t child_ = -1;
#else
      STARTUPINFO si;
      PROCESS_INFORMATION childInfo_;
#endif
   };
} // namespace clap