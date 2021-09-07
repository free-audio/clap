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
      RemoteGui(CorePlugin &plugin) : AbstractGui(plugin) {}
      ~RemoteGui();

      bool spawn();

      void defineParameter(const clap_param_info&) noexcept override;

      bool attachCocoa(void *nsView) noexcept override;
      bool attachWin32(clap_hwnd window) noexcept override;
      bool attachX11(const char *display_name, unsigned long window) noexcept override;

      bool size(uint32_t *width, uint32_t *height) noexcept override;
      void setScale(double scale) noexcept override;

      bool show() noexcept override;
      bool hide() noexcept override;

      void destroy() noexcept override;

      // RemoteChannel::EventControl
      void modifyFd(clap_fd_flags flags) override;
      void removeFd() override;

      clap_fd fd() const;
      void onFd(clap_fd_flags flags);

      clap_id timerId() const noexcept { return _timerId; }
      void onTimer();

   private:
      void onMessage(const RemoteChannel::Message& msg);
      void waitChild();

      std::unique_ptr<RemoteChannel> _channel;

      clap_id _timerId = CLAP_INVALID_ID;

#ifdef __unix__
      pid_t _child = -1;
#else
      STARTUPINFO _si;
      PROCESS_INFORMATION _childInfo;
#endif
   };
} // namespace clap