#ifdef __unix__
#   include <fcntl.h>
#   include <sys/socket.h>
#endif

#include <cassert>

#include "../io/messages.hh"
#include "path-provider.hh"
#include "core-plugin.hh"
#include "remote-gui.hh"

namespace clap {
   RemoteGui::~RemoteGui() {
      if (channel_)
         destroy();

      assert(!channel_);
   }

   bool RemoteGui::spawn() {
      assert(child_ == -1);
      assert(!channel_);

      auto& pathProvider = plugin_.pathProvider();

#ifdef __unix__
      /* create a socket pair */
      int sockets[2];
      if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sockets)) {
         return false;
      }

      printf("About to start GUI: %s --socket %d\n",
             pathProvider.getGuiExecutable().c_str(),
             sockets[0]);

      child_ = ::fork();
      if (child_ == -1) {
         ::close(sockets[0]);
         ::close(sockets[1]);
         return false;
      }

      if (child_ == 0) {
         // Child
         ::close(sockets[0]);
         char socketStr[16];
         ::snprintf(socketStr, sizeof(socketStr), "%d", sockets[1]);
         auto path = pathProvider.getGuiExecutable();
         auto skin = pathProvider.getSkinDirectory();
         ::execl(path.c_str(), path.c_str(), "--socket", socketStr, "--skin", skin.c_str(), nullptr);
         printf("Failed to start child process: %m\n");
         std::terminate();
      } else {
         // Parent
         ::close(sockets[1]);
      }

      plugin_.hostEventLoop_->register_fd(plugin_.host_, sockets[0], CLAP_FD_READ | CLAP_FD_ERROR);
      channel_.reset(new RemoteChannel(
         [this](const RemoteChannel::Message &msg) { onMessage(msg); }, *this, sockets[0], true));

      return true;
#else
      return false;
#endif
   }

   void RemoteGui::modifyFd(clap_fd_flags flags) {
      plugin_.hostEventLoop_->modify_fd(plugin_.host_, channel_->fd(), flags);
   }

   clap_fd RemoteGui::fd() const { return channel_ ? channel_->fd() : -1; }

   void RemoteGui::onFd(clap_fd_flags flags) {
      if (flags & CLAP_FD_READ)
         channel_->onRead();
      if (flags & CLAP_FD_WRITE)
         channel_->onWrite();
      if (flags & CLAP_FD_ERROR)
         channel_->onError();
   }

   void RemoteGui::onMessage(const RemoteChannel::Message &msg) {
      switch (msg.type) {}
   }

   void RemoteGui::defineParameter(const clap_param_info &info) noexcept {
      channel_->sendRequestAsync(messages::DefineParameterRequest{info});
   }

   bool RemoteGui::size(uint32_t *width, uint32_t *height) noexcept {
      messages::SizeRequest request;
      messages::SizeResponse response;

      if (!channel_->sendRequestSync(request, response))
         return false;

      *width = response.width;
      *height = response.height;
      return true;
   }

   void RemoteGui::setScale(double scale) noexcept {
      channel_->sendRequestAsync(messages::SetScaleRequest{scale});
   }

   bool RemoteGui::show() noexcept {
      messages::ShowRequest request;
      messages::ShowResponse response;

      return channel_->sendRequestSync(request, response);
   }

   bool RemoteGui::hide() noexcept {
      messages::HideRequest request;
      messages::HideResponse response;

      return channel_->sendRequestSync(request, response);
   }

   void RemoteGui::destroy() noexcept {
      if (!channel_)
         return;

      messages::DestroyRequest request;
      messages::DestroyResponse response;

      channel_->sendRequestSync(request, response);
      plugin_.hostEventLoop_->unregister_fd(plugin_.host_, channel_->fd());
      channel_.reset();
   }

   bool RemoteGui::attachCocoa(void *nsView) noexcept {
      // TODO
      return false;
   }

   bool RemoteGui::attachWin32(clap_hwnd window) noexcept {
      messages::AttachWin32Request request{window};
      messages::AttachResponse response;

      return channel_->sendRequestSync(request, response);
   }

   bool RemoteGui::attachX11(const char *display_name, unsigned long window) noexcept {
      messages::AttachX11Request request;
      messages::AttachResponse response;

      request.window = window;
      std::snprintf(request.display, sizeof(request.display), "%s", display_name ? : "");

      return channel_->sendRequestSync(request, response);
   }

} // namespace clap