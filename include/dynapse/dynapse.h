// Created by Autokaka (qq1909698494@gmail.com) on 2023/07/14.

#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace dynapse {

enum class MetaType : unsigned char { kUnknown, kBool, kInt, kFloat, kDouble, kString, kFunction, kObject };

/**
 * Meta serves as the CORE functionality of dynapse,
 * if you want the simplest and the most direct way to
 * use dynapse, Meta is just enough.
 */
class Meta;
using WeakMeta = std::weak_ptr<Meta>;
using MetaPtr = std::shared_ptr<Meta>;
class Meta final {
 public:
  // Meta
  using Destructor = void (*)(void* ptr);

  explicit Meta() = default;
  Meta(const Meta&) = delete;
  Meta& operator=(const Meta&) = delete;
  Meta(Meta&&) = delete;
  Meta& operator=(Meta&&) = delete;
  ~Meta() {
    if (destructor_ != nullptr) {
      destructor_(ptr_);
    }
  }

  [[nodiscard]] bool Owned() const { return destructor_ != nullptr; }
  template <typename PtrType>
  [[nodiscard]] PtrType As() const {
    return reinterpret_cast<PtrType>(ptr_);
  }

  // number
  [[nodiscard]] bool IsNumber() const {
    // clang-format off
    return type_ == MetaType::kBool
        || type_ == MetaType::kInt
        || type_ == MetaType::kFloat
        || type_ == MetaType::kDouble;
    // clang-format on
  }

  explicit Meta(bool bool_value) : ptr_(new bool(bool_value)), type_(MetaType::kBool), destructor_(ReleaseBool) {}
  explicit Meta(bool* bool_ref) : ptr_(bool_ref), type_(MetaType::kBool) {}
  static MetaPtr FromBool(bool bool_value) { return std::make_shared<Meta>(bool_value); }
  static MetaPtr RefBool(bool* bool_ref) { return std::make_shared<Meta>(bool_ref); }
  [[nodiscard]] bool IsBool() const { return type_ == MetaType::kBool; }

  explicit Meta(int int_value) : ptr_(new int(int_value)), type_(MetaType::kInt), destructor_(ReleaseInt) {}
  explicit Meta(int* int_ref) : ptr_(int_ref), type_(MetaType::kInt) {}
  static MetaPtr FromInt(int int_value) { return std::make_shared<Meta>(int_value); }
  static MetaPtr RefInt(int* int_ref) { return std::make_shared<Meta>(int_ref); }
  [[nodiscard]] bool IsInt() const { return type_ == MetaType::kInt; }

  explicit Meta(float float_value) : ptr_(new float(float_value)), type_(MetaType::kFloat), destructor_(ReleaseFloat) {}
  explicit Meta(float* float_ref) : ptr_(float_ref), type_(MetaType::kFloat) {}
  static MetaPtr FromFloat(float float_value) { return std::make_shared<Meta>(float_value); }
  static MetaPtr RefFloat(float* float_ref) { return std::make_shared<Meta>(float_ref); }
  [[nodiscard]] bool IsFloat() const { return type_ == MetaType::kFloat; }

  explicit Meta(double double_value)
      : ptr_(new double(double_value)), type_(MetaType::kDouble), destructor_(ReleaseDouble) {}
  explicit Meta(double* double_ref) : ptr_(double_ref), type_(MetaType::kDouble) {}
  static MetaPtr FromDouble(double double_value) { return std::make_shared<Meta>(double_value); }
  static MetaPtr RefDouble(double* double_ref) { return std::make_shared<Meta>(double_ref); }
  [[nodiscard]] bool IsDouble() const { return type_ == MetaType::kDouble; }

  // string
  explicit Meta(const std::string& string_value)
      : ptr_(new std::string(string_value)), type_(MetaType::kString), destructor_(ReleaseString) {}
  explicit Meta(std::string* string_ref) : ptr_(string_ref), type_(MetaType::kString) {}
  static MetaPtr FromString(const std::string& string_value) { return std::make_shared<Meta>(string_value); }
  static MetaPtr RefString(std::string* string_ref) { return std::make_shared<Meta>(string_ref); }
  [[nodiscard]] bool IsString() const { return type_ == MetaType::kString; }

  // function
  using Function = MetaPtr (*)(const MetaPtr& caller, const std::vector<MetaPtr>& args);
  explicit Meta(Function call_as_function, const MetaPtr& caller = nullptr)
      : ptr_(reinterpret_cast<void*>(call_as_function)), caller_(caller), type_(MetaType::kFunction) {}
  static MetaPtr FromFunction(Function call_as_function, const MetaPtr& caller = nullptr) {
    return std::make_shared<Meta>(call_as_function);
  }
  [[nodiscard]] bool IsFunction() const { return type_ == MetaType::kFunction; }
  MetaPtr CallAsFunction(const std::vector<MetaPtr>& args = {}) {
    auto* call_as_function = reinterpret_cast<Function>(ptr_);
    return call_as_function(caller_.lock(), args);
  }

  // object
  using ObjectAccessor = MetaPtr (*)(const std::string& name, const MetaPtr& caller, const std::vector<MetaPtr>& args);
  explicit Meta(void* object_ptr, Destructor dtor, ObjectAccessor accessor = nullptr)
      : type_(MetaType::kObject), ptr_(object_ptr), destructor_(dtor), accessor_(accessor) {}
  explicit Meta(void* object_ref, ObjectAccessor accessor = nullptr)
      : type_(MetaType::kObject), ptr_(object_ref), accessor_(accessor) {}
  static MetaPtr FromObject(void* object_ptr, Destructor dtor, ObjectAccessor accessor = nullptr) {
    return std::make_shared<Meta>(object_ptr, dtor, accessor);
  }
  static MetaPtr RefObject(void* object_ref, ObjectAccessor accessor = nullptr) {
    return std::make_shared<Meta>(object_ref, accessor);
  }
  [[nodiscard]] bool IsObject() const { return type_ == MetaType::kObject; }
  MetaPtr AccessObject(const std::string& name, const std::vector<MetaPtr>& args = {}) {
    return accessor_(name, this->caller_.lock(), args);
  }

 private:
  static void ReleaseBool(void* ptr) { delete reinterpret_cast<bool*>(ptr); }
  static void ReleaseInt(void* ptr) { delete reinterpret_cast<int*>(ptr); }
  static void ReleaseFloat(void* ptr) { delete reinterpret_cast<float*>(ptr); }
  static void ReleaseDouble(void* ptr) { delete reinterpret_cast<double*>(ptr); }
  static void ReleaseString(void* ptr) { delete reinterpret_cast<std::string*>(ptr); }

  ObjectAccessor accessor_ = nullptr;
  Destructor destructor_ = nullptr;
  WeakMeta caller_;
  void* ptr_ = nullptr;
  MetaType type_ = MetaType::kUnknown;
};
using WeakMeta = std::weak_ptr<Meta>;
using MetaPtr = std::shared_ptr<Meta>;

#ifndef DYNAPSE_CORE

#include <cstdlib>
#include <unordered_map>

/**
 * MetaCenter is an EXTRA component of dynapse that
 * helps you automatically pregenerate intermmediate
 * code to access a runtime Meta object.
 */
class MetaCenter;
using MetaCenterPtr = std::shared_ptr<MetaCenter>;
class MetaCenter {
 public:
  struct MetaDescriptor final {
   public:
    bool is_static = false;
    // function container
    Meta::Function value = nullptr;
    // property container
    Meta::Function getter = nullptr;
    Meta::Function setter = nullptr;
  };
  using MetaDescriptorMap = std::unordered_map<std::string, MetaDescriptor>;
  struct ClassRegistry final {
    Meta::Function constructor;
    std::unordered_map<std::string, MetaDescriptor> member_props;
    std::unordered_map<std::string, Meta::Function> member_fns;
    std::unordered_map<std::string, MetaDescriptor> static_props;
    std::unordered_map<std::string, Meta::Function> static_fns;
  };
  MetaCenter() = default;
  static std::shared_ptr<MetaCenter> GetDefaultCenter() {
    static auto default_center = std::make_shared<MetaCenter>();
    return default_center;
  }

  void Register(const std::string& class_name, const ClassRegistry& registry) {
    Register(class_name + ".constructor", registry.constructor);

    for (auto [prop_key, prop_meta_desc] : registry.member_props) {
      // clang-format off
      std::string path = class_name; path.append(".").append(prop_key);
      // clang-format on
      auto desc_ptr = std::make_shared<MetaDescriptor>(prop_meta_desc);
      prop_meta_desc.is_static = false;
      prop_meta_desc.value = nullptr;
      meta_desc_map_[path] = prop_meta_desc;
    }
    for (auto&& [prop_key, member_fn] : registry.member_fns) {
      // clang-format off
      std::string path = class_name; path.append(".").append(prop_key);
      // clang-format on
      Register(path, member_fn);
    }
    for (auto [prop_key, prop_meta_desc] : registry.static_props) {
      // clang-format off
      std::string path = class_name; path.append(".").append(prop_key);
      // clang-format on
      prop_meta_desc.is_static = true;
      prop_meta_desc.value = nullptr;
      meta_desc_map_[path] = prop_meta_desc;
    }
    for (auto&& [prop_key, static_fn] : registry.static_fns) {
      // clang-format off
      std::string path = class_name; path.append(".").append(prop_key);
      // clang-format on
      Register(path, static_fn);
    }
  }

  void Register(const std::string& path, MetaDescriptor static_prop_desc) {
    static_prop_desc.is_static = true;
    meta_desc_map_[path] = static_prop_desc;
  }
  void Register(const std::string& path, const Meta::Function& function) {
    MetaDescriptor desc;
    desc.is_static = true;
    desc.value = function;
    meta_desc_map_[path] = desc;
  }

  // runtime unified access method
  MetaPtr DynCall(const std::string& path, const MetaPtr& caller = nullptr, const std::vector<MetaPtr>& args = {}) {
    if (!meta_desc_map_.contains(path)) {
      return nullptr;
    }
    auto desc = meta_desc_map_[path];
    auto arg_count = args.size();
    if ((desc.getter != nullptr) && arg_count == 0) {
      return desc.getter(caller, args);
    }
    if ((desc.setter != nullptr) && arg_count == 1) {
      desc.setter(caller, args);
      return nullptr;
    }
    if (desc.value != nullptr) {
      return desc.value(caller, args);
    }
    return nullptr;
  }

  std::optional<MetaDescriptor> GetDescriptorOf(const std::string& path) {
    if (!meta_desc_map_.contains(path)) {
      return std::nullopt;
    }
    return meta_desc_map_[path];
  }

 private:
  MetaDescriptorMap meta_desc_map_;
};
using MetaCenterPtr = std::shared_ptr<MetaCenter>;

#endif

}  // namespace dynapse
