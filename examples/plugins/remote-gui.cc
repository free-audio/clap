#ifdef __unix__
#   include <fcntl.h>
#   include <sys/socket.h>
#endif

#include <cassert>

#include "../io/messages.hh"
#include "core-plugin.hh"
#include "path-provider.hh"
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

      auto &pathProvider = plugin_.pathProvider();

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
         auto qmlLib = pathProvider.getQmlLibDirectory();
         ::execl(path.c_str(),
                 path.c_str(),
                 "--socket",
                 socketStr,
                 "--skin",
                 skin.c_str(),
                 "--qml-import",
                 qmlLib.c_str(),
                 (const char *)nullptr);
         printf("Failed to start child process: %m\n");
         std::terminate();
      } else {
         // Parent
         ::close(sockets[1]);
      }

      timerId_ = CLAP_INVALID_ID;
      plugin_.hostEventLoop_->register_timer(plugin_.host_, 1000 / 60, &timerId_);
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

   void RemoteGui::removeFd() {
      plugin_.hostEventLoop_->unregister_fd(plugin_.host_, channel_->fd());
      plugin_.hostEventLoop_->unregister_timer(plugin_.host_, timerId_);
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
      switch (msg.type) {
      case messages::kAdjustRequest: {
         messages::AdjustRequest rq;
         msg.get(rq);
         plugin_.guiAdjust(rq.paramId, rq.value, rq.flags);
         break;
      }
      }
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
      channel_->close();
      channel_.reset();

      waitChild();
   }

   void RemoteGui::waitChild() {
#ifdef __unix__
      if (child_ == -1)
         return;
      int stat = 0;
      int ret;

      do {
         ret = ::waitpid(child_, &stat, 0);
      } while (ret == -1 && errno == EINTR);

      child_ = -1;
#endif
   }

   bool RemoteGui::attachCocoa(void *nsView) noexcept {
      messages::AttachCocoaRequest request{nsView};
      messages::AttachResponse response;

      return channel_->sendRequestSync(request, response);
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
      std::snprintf(request.display, sizeof(request.display), "%s", display_name ?: "");

      return channel_->sendRequestSync(request, response);
   }

   void RemoteGui::onTimer() {
      plugin_.pluginToGuiQueue_.consume(
         [this](clap_id paramId, const CorePlugin::PluginToGuiValue &value) {
            messages::ParameterValueRequest rq{paramId, value.value, value.mod};
            channel_->sendRequestAsync(rq);
         });
   }

} // namespace clap