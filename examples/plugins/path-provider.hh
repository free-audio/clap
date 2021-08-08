#pragma once

#include <memory>
#include <string>

namespace clap {
   class PathProvider {
   public:
      virtual ~PathProvider() = default;

      static std::unique_ptr<PathProvider> create(const std::string &pluginPath, const std::string& pluginName);

      virtual std::string getGuiExecutable() const = 0;
      virtual std::string getSkinDirectory() const = 0;

      virtual bool isValid() const = 0;
   };
} // namespace clap