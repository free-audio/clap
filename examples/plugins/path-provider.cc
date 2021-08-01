#include <cassert>
#include <cstring>
#include <filesystem>
#include <regex>

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

   class LinuxDevelopmentPathProvider final : public PathProvider {
   public:
      LinuxDevelopmentPathProvider(const std::string &pluginPath)
         : srcRoot_(computeSrcRoot(pluginPath)), buildRoot_(computeBuildRoot(pluginPath)) {}

      static std::string computeSrcRoot(const std::string &pluginPath) {
         static const std::regex r("(/.*)/cmake-builds/.*$", std::regex::optimize);

         std::smatch m;
         if (!std::regex_match(pluginPath, m, r))
            return "";
         return m[1];
      }

      static std::string computeBuildRoot(const std::string &pluginPath) {
         static const std::regex r("(/.*/cmake-builds/.*)/examples/plugins/.*\\.clap$",
                                   std::regex::optimize);

         std::smatch m;
         if (!std::regex_match(pluginPath, m, r))
            return "";
         return m[1];
      }

      std::string getGuiExecutable() const override {
         return buildRoot_ / "/examples/gui/clap-gui";
      }

      bool isValid() const noexcept { return !srcRoot_.empty() && !buildRoot_.empty(); }

   private:
      const std::filesystem::path srcRoot_;
      const std::filesystem::path buildRoot_;
   };

   std::unique_ptr<PathProvider> PathProvider::instance_;

   const PathProvider *PathProvider::createInstance(const std::string &pluginPath) {
      assert(!instance_);

#ifdef __linux__
      {
         auto ptr = std::make_unique<LinuxDevelopmentPathProvider>(pluginPath);
         if (ptr->isValid())
            return ptr.release();
         return new LinuxPathProvider(pluginPath);
      }
#endif

      // TODO
      return nullptr;
   }
} // namespace clap