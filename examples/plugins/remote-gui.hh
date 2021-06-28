#pragma once

#include <boost/process.hpp>

#include "abstract-gui.hh"

namespace clap {
   class RemoteGui : public AbstractGui {
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

   private:
      boost::process::child child_;
      boost::process::pipe wpipe_;
      boost::process::pipe rpipe_;
   };
} // namespace clap