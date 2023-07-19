// Created by Autokaka (qq1909698494@gmail.com) on 2023/07/19.

#pragma once

#include "prototype.h"
#include "types.h"

namespace dynapse {

class Any final {
 public:
  static Any Null();
  explicit Any() = default;
  explicit Any(void* ptr, const Prototype& prototype);
  ~Any() = default;

  explicit operator bool() const { return ptr_ != nullptr; }
  Any operator[](const std::string& path);
  Any operator()(const Args& args = {}) const;

  template <typename T>
  T To() const {
    return *static_cast<T*>(ptr_.get());
  }
  template <typename T = void*>
  T As() const {
    return static_cast<T>(ptr_.get());
  }
  void Reset(void* ptr);
  void Reset(void* ptr, const Prototype& prototype);

  Prototype prototype;

 private:
  static void EmptyDestructor(void* ptr) {}
  static bool Access(const std::string& name, const Any& caller, const PropertyMap& property_map, Any* result);
  static bool Access(const std::string& name, const Any& caller, const FunctionMap& function_map, Any* result);
  bool Access(const std::string& path, OptionalPrototype prototype, Any* result);

  std::shared_ptr<void> ptr_ = nullptr;
};

template <typename... Args>
AnyPtr make_any(Args... args) {
  return std::make_shared<Any>(args...);
}

}  // namespace dynapse
