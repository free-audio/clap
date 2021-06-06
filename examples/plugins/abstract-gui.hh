#pragma once

#include <clap/all.h>

namespace clap {

   class AbstractGui {
   public:
      virtual ~AbstractGui();

      virtual bool attach(void *nsView) noexcept = 0;
      virtual bool attachWin32(clap_hwnd window) noexcept = 0;
      virtual bool attachX11(const char *display_name, unsigned long window) noexcept = 0;

      virtual void size(int32_t *width, int32_t *height) noexcept = 0;
      virtual void setScale(double scale) noexcept = 0;

      virtual bool show() noexcept = 0;
      virtual bool hide() noexcept = 0;

      virtual void close() noexcept = 0;
   };

} // namespace clap