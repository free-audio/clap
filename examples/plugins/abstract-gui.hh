#pragma once

#include <clap/all.h>

namespace clap {

   class CorePlugin;
   class AbstractGui {
   public:
      AbstractGui(CorePlugin &plugin) : plugin_(plugin) {}
      virtual ~AbstractGui() = default;

      virtual void defineParameter(const clap_param_info&) noexcept = 0;

      virtual bool attachCocoa(void *nsView) noexcept = 0;
      virtual bool attachWin32(clap_hwnd window) noexcept = 0;
      virtual bool attachX11(const char *display_name, unsigned long window) noexcept = 0;

      virtual bool size(uint32_t *width, uint32_t *height) noexcept = 0;
      virtual void setScale(double scale) noexcept = 0;

      virtual bool show() noexcept = 0;
      virtual bool hide() noexcept = 0;

      virtual void destroy() noexcept = 0;

   protected:
      CorePlugin &plugin_;
   };

} // namespace clap