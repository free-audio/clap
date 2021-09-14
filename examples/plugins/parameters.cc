#include <cassert>

#include "parameters.hh"

namespace clap {
   void clap::Parameters::addParameter(const clap_param_info &info) {
      assert(_id2param.find(info.id) == _id2param.end());

      auto p = std::make_unique<Parameter>(info);
      _id2param.insert_or_assign(info.id, p.get());
      _params.emplace_back(std::move(p));
   }

   size_t Parameters::count() const noexcept { return _params.size(); }

   Parameter *Parameters::getByIndex(size_t index) const noexcept {
      if (index > _params.size())
         return nullptr;
      return _params[index].get();
   }

   Parameter *Parameters::getById(clap_id id) const noexcept {
      auto it = _id2param.find(id);
      if (it == _id2param.end())
         return nullptr;
      return it->second;
   }
} // namespace clap
