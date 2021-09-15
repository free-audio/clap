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
      if (_channel)
         destroy();

      assert(!_channel);
   }

   bool RemoteGui::spawn() {
      assert(_child == -1);
      assert(!_channel);

      if (!_plugin.canUseTimerSupport() || !_plugin.canUseFdSupport())
         return false;

      auto &pathProvider = _plugin.pathProvider();

#ifdef __unix__
      /* create a socket pair */
      int sockets[2];
      if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sockets)) {
         return false;
      }

      auto path = pathProvider.getGuiExecutable();
      auto skin = pathProvider.getSkinDirectory();
      auto qmlLib = pathProvider.getQmlLibDirectory();

      printf("About to start GUI: %s --socket %d --skin %s --qml-import %s\n",
             path.c_str(),
             sockets[0],
             skin.c_str(),
             qmlLib.c_str());

      _child = ::fork();
      if (_child == -1) {
         ::close(sockets[0]);
         ::close(sockets[1]);
         return false;
      }

      if (_child == 0) {
         // Child
         ::close(sockets[0]);
         char socketStr[16];
         ::snprintf(socketStr, sizeof(socketStr), "%d", sockets[1]);
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

      _timerId = CLAP_INVALID_ID;
      _plugin._hostTimerSupport->register_timer(_plugin._host, 1000 / 60, &_timerId);
      _plugin._hostFdSupport->register_fd(_plugin._host, sockets[0], CLAP_FD_READ | CLAP_FD_ERROR);
      _channel.reset(new RemoteChannel(
         [this](const RemoteChannel::Message &msg) { onMessage(msg); }, *this, sockets[0], true));

      return true;
#else
      return false;
#endif
   }

   void RemoteGui::modifyFd(clap_fd_flags flags) {
      _plugin._hostFdSupport->modify_fd(_plugin._host, _channel->fd(), flags);
   }

   void RemoteGui::removeFd() {
      _plugin._hostFdSupport->unregister_fd(_plugin._host, _channel->fd());
      _plugin._hostTimerSupport->unregister_timer(_plugin._host, _timerId);
   }

   clap_fd RemoteGui::fd() const { return _channel ? _channel->fd() : -1; }

   void RemoteGui::onFd(clap_fd_flags flags) {
      if (flags & CLAP_FD_READ)
         _channel->onRead();
      if (flags & CLAP_FD_WRITE)
         _channel->onWrite();
      if (flags & CLAP_FD_ERROR)
         _channel->onError();
   }

   void RemoteGui::onMessage(const RemoteChannel::Message &msg) {
      switch (msg.type) {
      case messages::kAdjustRequest: {
         messages::AdjustRequest rq;
         msg.get(rq);
         _plugin.guiAdjust(rq.paramId, rq.value, rq.flags);
         break;
      }
      }
   }

   void RemoteGui::defineParameter(const clap_param_info &info) noexcept {
      _channel->sendRequestAsync(messages::DefineParameterRequest{info});
   }

   bool RemoteGui::size(uint32_t *width, uint32_t *height) noexcept {
      messages::SizeRequest request;
      messages::SizeResponse response;

      if (!_channel->sendRequestSync(request, response))
         return false;

      *width = response.width;
      *height = response.height;
      return true;
   }

   void RemoteGui::setScale(double scale) noexcept {
      _channel->sendRequestAsync(messages::SetScaleRequest{scale});
   }

   bool RemoteGui::show() noexcept {
      messages::ShowRequest request;
      messages::ShowResponse response;

      return _channel->sendRequestSync(request, response);
   }

   bool RemoteGui::hide() noexcept {
      messages::HideRequest request;
      messages::HideResponse response;

      return _channel->sendRequestSync(request, response);
   }

   void RemoteGui::destroy() noexcept {
      if (!_channel)
         return;

      messages::DestroyRequest request;
      messages::DestroyResponse response;

      _channel->sendRequestSync(request, response);
      _channel->close();
      _channel.reset();

      waitChild();
   }

   void RemoteGui::waitChild() {
#ifdef __unix__
      if (_child == -1)
         return;
      int stat = 0;
      int ret;

      do {
         ret = ::waitpid(_child, &stat, 0);
      } while (ret == -1 && errno == EINTR);

      _child = -1;
#endif
   }

   bool RemoteGui::attachCocoa(void *nsView) noexcept {
      messages::AttachCocoaRequest request{nsView};
      messages::AttachResponse response;

      return _channel->sendRequestSync(request, response);
   }

   bool RemoteGui::attachWin32(clap_hwnd window) noexcept {
      messages::AttachWin32Request request{window};
      messages::AttachResponse response;

      return _channel->sendRequestSync(request, response);
   }

   bool RemoteGui::attachX11(const char *display_name, unsigned long window) noexcept {
      messages::AttachX11Request request;
      messages::AttachResponse response;

      request.window = window;
      std::snprintf(request.display, sizeof(request.display), "%s", display_name ?: "");

      return _channel->sendRequestSync(request, response);
   }

   void RemoteGui::onTimer() {
      _plugin._pluginToGuiQueue.consume(
         [this](clap_id paramId, const CorePlugin::PluginToGuiValue &value) {
            messages::ParameterValueRequest rq{paramId, value.value, value.mod};
            _channel->sendRequestAsync(rq);
         });

      if (_isTransportSubscribed && _plugin._hasTransportCopy) {
         messages::UpdateTransportRequest rq{_plugin._hasTransport, _plugin._hasTransportCopy};
         _channel->sendRequestAsync(rq);
         _plugin._hasTransportCopy = false;
      }
   }

} // namespace clap