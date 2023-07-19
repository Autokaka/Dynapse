// Created by Autokaka (qq1909698494@gmail.com) on 2023/07/19.

#include "dynapse/any.h"
#include <dynapse/dynapse.h>
#include <memory>

namespace dynapse {

Any Any::Null() {
  static Any null;
  return null;
}

Any::Any(void* ptr, const PrototypePtr& prototype) {
  Reset(ptr, prototype);
}

Any Any::operator[](const std::string& path) {
  auto result = Any::Null();
  Access(path, prototype, &result);
  return result;
}

Any Any::operator()(const Args& args) const {
  if (prototype && prototype->call_as_function != nullptr) {
    return prototype->call_as_function(*this, args);
  }
  return Any::Null();
}

void Any::Reset(void* ptr) {
  ptr_ = std::shared_ptr<void>(ptr, prototype->destructor);
}

void Any::Reset(void* ptr, const PrototypePtr& prototype) {
  ptr_ = std::shared_ptr<void>(ptr, prototype->destructor);
  this->prototype = prototype;
}

bool Any::Access(const std::string& name, const Any& caller, const PropertyMap& property_map, Any* result) {
  // NOLINTNEXTLINE
  for (auto&& [key, prop] : property_map) {
    if (key == name) {
      *result = prop.get != nullptr ? prop.get(caller, {}) : Any::Null();
      return true;
    }
  }
  return false;
}

bool Any::Access(const std::string& name, const Any& caller, const FunctionMap& function_map, Any* result) {
  // NOLINTNEXTLINE
  for (auto&& [key, function] : function_map) {
    if (key == name) {
      *result = Any(caller.As<>(), make_prototype({.call_as_function = function, .destructor = EmptyDestructor}));
      return true;
    }
  }
  return false;
}

bool Any::Access(const std::string& path, const PrototypePtr& prototype, Any* result) {
  if (!prototype) {
    return false;
  }
  if (Access(path, *this, prototype->member_property_map, result)) {
    return true;
  }
  if (Access(path, *this, prototype->member_function_map, result)) {
    return true;
  }
  if (Access(path, Any::Null(), prototype->static_function_map, result)) {
    return true;
  }
  if (Access(path, Any::Null(), prototype->static_property_map, result)) {
    return true;
  }
  auto parent = Reflect::Get()->GetPrototypeOf(prototype->parent_name);
  return Access(path, parent, result);
}

}  // namespace dynapse
