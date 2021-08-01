#include <cassert>
#include <cstring>
#include <regex>
#include <filesystem>

#include "path-provider.hh"

namespace clap {

   class LinuxPathProvider final : public PathProvider {
   public:
      LinuxPathProvider(const std::string &pluginPath) : prefix_(computePrefix(pluginPath)) {}

      static std::string computePrefix(const std::string &pluginPath) {
         static const std::regex r("(/.*)/lib/clap/.*$", std::regex::optimize);

         std::smatch m;
         if (!std::regex_match(pluginPath, m, r))
            return "/usr";
         return m[1];
      }

      std::string getGuiExecutable() const override { return prefix_ / "/bin/clap-gui"; }

   private:
      const std::filesystem::path prefix_;
   };

   std::unique_ptr<PathProvider> PathProvider::instance_;

   const PathProvider *PathProvider::createInstance(const std::string &pluginPath) {
      assert(!instance_);

#ifdef __linux__
      return new LinuxPathProvider(pluginPath);
#endif

      // TODO
      return nullptr;
   }
} // namespace clap