// Created by Autokaka (qq1909698494@gmail.com) on 2023/07/19.

#include <dynapse/dynapse.h>

namespace dynapse {

Any Any::Null() {
  static Any null;
  return null;
}

Any::Any(void* ptr, const Prototype& prototype) {
  Reset(ptr, prototype);
}

Any::Any(const Any& other) {
  ptr_ = other.ptr_;
  prototype = other.prototype;
  prototype.assign = nullptr;
}

Any& Any::operator=(const Any& other) {
  if (this == &other) {
    return *this;
  }
  auto assign = prototype.assign;
  if (assign == nullptr) {
    ptr_ = other.ptr_;
    prototype = other.prototype;
  } else {
    prototype.assign = nullptr;
    assign(*this, {other});
    prototype.assign = assign;
  }
  return *this;
}

Any::Any(Any&& other) noexcept {
  ptr_ = std::move(other.ptr_);
  prototype = std::move(other.prototype);
  prototype.assign = nullptr;
}

Any& Any::operator=(Any&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  auto assign = prototype.assign;
  if (assign == nullptr) {
    ptr_ = std::move(other.ptr_);
    prototype = std::move(other.prototype);
  } else {
    prototype.assign = nullptr;
    assign(*this, {other});
    prototype.assign = assign;
  }
  return *this;
}

Any Any::operator[](const std::string& path) {
  auto result = Any::Null();
  Access(path, prototype, &result);
  return result;
}

Any Any::operator()(const Args& args) const {
  if (prototype.call_as_function != nullptr) {
    return prototype.call_as_function(*this, args);
  }
  return Any::Null();
}

void Any::Reset(void* ptr) {
  ptr_ = std::shared_ptr<void>(ptr, prototype.destructor);
}

void Any::Reset(void* ptr, const Prototype& prototype) {
  ptr_ = std::shared_ptr<void>(ptr, prototype.destructor);
  this->prototype = prototype;
}

bool Any::Access(const std::string& name, const Any& caller, const PropertyMap& property_map, Any* result) {
  // NOLINTNEXTLINE
  for (const auto& [key, prop] : property_map) {
    if (key == name) {
      if (prop.value) {
        *result = *prop.value;
        (*result).prototype.assign = prop.readonly ? nullptr : DefaultSetter;
        return true;
      }
      if (prop.get != nullptr) {
        *result = prop.get(caller, {});
        (*result).prototype.assign = prop.set;
        return true;
      }
      *result = Any::Null();
      return true;
    }
  }
  return false;
}

Any Any::DefaultSetter(const Any& caller, const Args& args) {
  const_cast<Any&>(caller) = args[0];
  return Any::Null();
}

bool Any::Access(const std::string& name, const Any& caller, const FunctionMap& function_map, Any* result) {
  // NOLINTNEXTLINE
  for (auto&& [key, function] : function_map) {
    if (key == name) {
      *result = Any(caller.As(), {.call_as_function = function, .destructor = EmptyDestructor});
      return true;
    }
  }
  return false;
}

bool Any::Access(const std::string& path, OptionalPrototype prototype, Any* result) {
  if (!prototype) {
    return false;
  }
  const auto& proto = prototype.value();
  if (Access(path, *this, proto.member_property_map, result)) {
    return true;
  }
  if (Access(path, *this, proto.member_function_map, result)) {
    return true;
  }
  if (Access(path, Any::Null(), proto.static_function_map, result)) {
    return true;
  }
  if (Access(path, Any::Null(), proto.static_property_map, result)) {
    return true;
  }
  auto parent = GetReflect().FindPrototype(proto.parent_name);
  return Access(path, parent, result);
}

}  // namespace dynapse
