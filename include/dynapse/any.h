// Created by Autokaka (qq1909698494@gmail.com) on 2023/07/19.

#pragma once

#include "prototype.h"
#include "types.h"

namespace dynapse {

class Any final {
 public:
  using NativePtr = std::shared_ptr<void>;

  static Any Null();
  explicit Any() = default;
  explicit Any(void* ptr, const Prototype& prototype);
  Any(const Any& other);
  Any& operator=(const Any& other);
  Any(Any&& other) noexcept;
  Any& operator=(Any&& other) noexcept;
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
  void Reset(void* ptr, const Prototype& prototype);

  Prototype prototype;

 private:
  static void EmptyDestructor(void* ptr) {}
  static Any CallAsPropertySetter(const Any& setter_context, const Args& args);
  static bool Access(const std::string& name, const Any& caller, const PropertyMap& property_map, Any* result);
  static bool Access(const std::string& name, const Any& caller, const FunctionMap& function_map, Any* result);
  bool Access(const std::string& path, const Prototype& prototype, Any* result);

  NativePtr ptr_ = nullptr;
};

template <typename... Args>
AnyPtr make_any(Args... args) {
  return std::make_shared<Any>(args...);
}

}  // namespace dynapse
