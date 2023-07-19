// Created by Autokaka (qq1909698494@gmail.com) on 2023/07/19.

#include <dynapse/dynapse.h>

namespace dynapse {

ReflectPtr Reflect::Get() {
  static auto reflect = std::shared_ptr<Reflect>(new Reflect);
  return reflect;
}

void Reflect::Register(const Prototype& prototype) {
  prototype_map_[prototype.name] = make_prototype(prototype);
}
std::vector<PrototypePtr> Reflect::GetPrototypes() const {
  std::vector<PrototypePtr> prototypes;
  for (auto&& [_, prototype] : prototype_map_) {
    prototypes.emplace_back(prototype);
  }
  return prototypes;
}

PrototypePtr Reflect::GetPrototypeOf(const std::string& name) const {
  auto iter = prototype_map_.find(name);
  return iter == prototype_map_.end() ? nullptr : iter->second;
}

Any Reflect::Construct(const std::string& name, const Args& args) const {
  if (auto prototype = GetPrototypeOf(name)) {
    auto* result = prototype->constructor(args);
    return Any(result, prototype);
  }
  return Any::Null();
}

}  // namespace dynapse
