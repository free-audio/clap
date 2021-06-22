#include <cassert>

#include "parameters.hh"

namespace clap {
   void clap::Parameters::addParameter(const Parameter &param) {
      assert(id2param_.find(param.info().id) == id2param_.end());

      auto p = std::make_unique<Parameter>(param);
      id2param_.emplace(param.info().id, p.get());
      params_.emplace_back(std::move(p));
   }

   size_t Parameters::count() const noexcept { return params_.size(); }

   Parameter *Parameters::getByIndex(size_t index) const noexcept {
      if (index > params_.size())
         return nullptr;
      return params_[index].get();
   }

   Parameter *Parameters::getById(clap_id id) const noexcept {
      auto it = id2param_.find(id);
      if (it == id2param_.end())
         return nullptr;
      return it->second;
   }
} // namespace clap
