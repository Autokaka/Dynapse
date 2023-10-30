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
    (*assign)({other});
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
    (*assign)({other});
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
        if (prop.readonly) {
          result->prototype.assign = nullptr;
        } else {
          Prototype proto{.call_as_function = CallAsPropertySetter, .destructor = EmptyDestructor};
          result->prototype.assign = make_any(const_cast<Property*>(&prop), proto);
        }
        return true;
      }
      if (prop.get != nullptr) {
        *result = prop.get(caller, {});
        auto assign = make_any();
        assign->ptr_ = caller.ptr_;
        assign->prototype = {.call_as_function = prop.set};
        result->prototype.assign = assign;
        return true;
      }
      *result = Any::Null();
      return true;
    }
  }
  return false;
}

Any Any::CallAsPropertySetter(const Any& setter_context, const Args& args) {
  auto* prop = setter_context.As<Property*>();
  prop->value = make_any();
  prop->value->ptr_ = args[0].ptr_;
  prop->value->prototype = args[0].prototype;
  return Any::Null();
}

bool Any::Access(const std::string& name, const Any& caller, const FunctionMap& function_map, Any* result) {
  // NOLINTNEXTLINE
  for (auto&& [key, function] : function_map) {
    if (key == name) {
      *result = Any();
      result->ptr_ = caller.ptr_;
      result->prototype = {.call_as_function = function};
      return true;
    }
  }
  return false;
}

bool Any::Access(const std::string& path, const Prototype& prototype, Any* result) {
  if (Access(path, *this, prototype.member_property_map, result)) {
    return true;
  }
  if (Access(path, *this, prototype.member_function_map, result)) {
    return true;
  }
  if (Access(path, Any::Null(), prototype.static_function_map, result)) {
    return true;
  }
  if (Access(path, Any::Null(), prototype.static_property_map, result)) {
    return true;
  }
  const auto* parent = GetReflect().FindPrototype(prototype.parent_name);
  if (parent != nullptr) {
    return Access(path, *parent, result);
  }
  return false;
}

}  // namespace dynapse
