#include "remote-gui.hh"

namespace clap {

   bool RemoteGui::spawn() {
      assert(!child_.running());

      child_ = boost::process::child("clap-gui", boost::process::std_out = wpipe_);
      return child_.running();
   }

} // namespace clap