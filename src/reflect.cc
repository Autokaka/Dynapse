// Created by Autokaka (qq1909698494@gmail.com) on 2023/07/19.

#include <dynapse/dynapse.h>

namespace dynapse {

void Reflect::Register(const Prototype& prototype) {
  prototype_map_[prototype.name] = prototype;
}
std::vector<Prototype> Reflect::GetPrototypes() const {
  std::vector<Prototype> prototypes;
  for (auto&& [_, prototype] : prototype_map_) {
    prototypes.emplace_back(prototype);
  }
  return prototypes;
}

OptionalPrototype Reflect::FindPrototype(const std::string& name) const {
  auto iter = prototype_map_.find(name);
  return iter == prototype_map_.end() ? std::nullopt : std::make_optional(iter->second);
}

bool Reflect::SetPrototype(Any& any, const std::string& name) {
  if (auto proto = FindPrototype(name)) {
    return SetPrototypeOf(any, *proto);
  }
  return false;
}

Any Reflect::Apply(const Any& function, const Any& caller, const Args& args) const {
  return Apply(function.prototype.call_as_function);
}

// NOLINTNEXTLINE
Any Reflect::Apply(Function function, const Any& caller, const Args& args) const {
  return function != nullptr ? function(caller, args) : Any::Null();
}

Any Reflect::Construct(void* ptr, const std::string& name) const {
  if (auto prototype = FindPrototype(name)) {
    return Any(ptr, *prototype);
  }
  return Any::Null();
}

Any Reflect::Construct(const std::string& name, const Args& args) const {
  if (auto prototype = FindPrototype(name)) {
    auto* result = prototype->constructor(args);
    return Any(result, *prototype);
  }
  return Any::Null();
}

// NOLINTNEXTLINE
bool Reflect::DefineProperty(Any& any, const std::string& prop_key, const Property& prop_desc) {
  if (!IsExtensible(any)) {
    return false;
  }
  auto prev_prop = GetOwnPropertyDescriptor(any, prop_key);
  if (!prev_prop || prev_prop->configurable) {
    any.prototype.member_property_map[prop_key] = prop_desc;
    return true;
  }
  return false;
}

// NOLINTNEXTLINE
void Reflect::DeleteProperty(Any& any, const std::string& prop_key) {
  auto prev_prop = GetOwnPropertyDescriptor(any, prop_key);
  if (prev_prop && prev_prop->configurable) {
    any.prototype.member_property_map.erase(prop_key);
  }
}

// NOLINTNEXTLINE
Any Reflect::Get(const Any& any, const std::string& prop_key) const {
  if (auto desc = GetOwnPropertyDescriptor(any, prop_key)) {
    auto property = desc.value();
    if (property.value) {
      return *property.value;
    }
    if (property.get != nullptr) {
      return property.get(any, {});
    }
  }
  return Any::Null();
}

// NOLINTNEXTLINE
OptionalProperty Reflect::GetOwnPropertyDescriptor(const Any& any, const std::string& prop_key) const {
  for (const auto& [name, property] : any.prototype.member_property_map) {
    if (name == prop_key) {
      return std::make_optional(property);
    }
  }
  return std::nullopt;
}

OptionalPrototype Reflect::GetPrototypeOf(const Any& any) const {
  return FindPrototype(any.prototype.name);
}

// NOLINTNEXTLINE
bool Reflect::Has(const Any& any, const std::string& prop_key) const {
  const auto& map = any.prototype.member_property_map;
  return map.find(prop_key) != map.end();
}

// NOLINTNEXTLINE
bool Reflect::IsExtensible(const Any& any) const {
  return any.prototype.extensible;
}

// NOLINTNEXTLINE
std::vector<std::string> Reflect::OwnKeys(const Any& any) const {
  std::vector<std::string> keys;
  for (const auto& [key, property] : any.prototype.member_property_map) {
    if (property.enumerable) {
      keys.emplace_back(key);
    }
  }
  return keys;
}

// NOLINTNEXTLINE
void Reflect::PreventExtensions(Any& any) {
  /**
   * The Object.preventExtensions() static method prevents new properties from ever being added to an object (i.e.
   * prevents future extensions to the object). It also prevents the object's prototype from being re-assigned.
   */

  // TODO(Autokaka): prototype operations should be wrapped with setter & getter in `Any` to conform to this API.
  any.prototype.extensible = false;
}

// NOLINTNEXTLINE
bool Reflect::Set(Any& any, const std::string& prop_key, const Any& prop_value) {
  if (!IsExtensible(any)) {
    return false;
  }
  auto& map = any.prototype.member_property_map;
  auto prop_iter = map.find(prop_key);
  if (prop_iter == map.end()) {
    map[prop_key] = Property{.value = make_any(prop_value)};
    return true;
  }
  auto prev_prop = prop_iter->second;
  if (prev_prop.readonly || prev_prop.set == nullptr) {
    return false;
  }
  map[prop_key].value = make_any(prop_value);
  return true;
}

// NOLINTNEXTLINE
bool Reflect::SetPrototypeOf(Any& any, const Prototype& prototype) {
  if (IsExtensible(any)) {
    any.prototype = prototype;
    return true;
  }
  return false;
}

Reflect::Reflect() {
  // register basic protos
  Register({.name = "bool", .destructor = [](void* ptr) { delete static_cast<bool*>(ptr); }});
  Register({.name = "int", .destructor = [](void* ptr) { delete static_cast<int*>(ptr); }});
  Register({.name = "float", .destructor = [](void* ptr) { delete static_cast<float*>(ptr); }});
  Register({.name = "double", .destructor = [](void* ptr) { delete static_cast<double*>(ptr); }});
  Register({.name = "string", .destructor = [](void* ptr) { delete static_cast<std::string*>(ptr); }});
}

Reflect& GetReflect() {
  static auto reflect = ReflectPtr(new Reflect);
  return *reflect;
}

std::string TypeOf(const Any& any) {
  if (!any) {
    return "null";
  }
  const auto& proto = any.prototype;
  if (proto.call_as_function != nullptr) {
    return "function";
  }
  const auto& name = proto.name;
  if (name == "int" || name == "float" || name == "double") {
    return "number";
  }
  if (name == "string") {
    return "string";
  }
  return "object";
}

}  // namespace dynapse
