#include <memory>
#include <string>

namespace clap {
   class PathProvider {
   public:
      virtual ~PathProvider() = default;

      static const PathProvider *createInstance(const std::string &pluginPath);
      static void destroyInstance() { instance_.reset(); }
      static const PathProvider *instance() { return instance_.get(); }

      virtual std::string getGuiExecutable() const = 0;

   private:
      static std::unique_ptr<PathProvider> instance_;
   };
} // namespace clap