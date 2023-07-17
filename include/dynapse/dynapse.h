// Created by Autokaka (qq1909698494@gmail.com) on 2023/07/14.

#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace dynapse {

enum class MetaType : unsigned char { kUnknown, kBool, kInt, kFloat, kDouble, kString, kFunction, kObject };

/**
 * Meta serves as the CORE functionality of dynapse,
 * if you want the simplest and the most direct way to
 * use dynapse, Meta is just enough.
 */
class Meta;
using MetaPtr = std::shared_ptr<Meta>;
class Meta final : public std::enable_shared_from_this<Meta> {
 public:
  // Meta

  using Constructor = void* (*)(const std::vector<MetaPtr>& args);
  using Destructor = void (*)(void* ptr);
  using Function = MetaPtr (*)(const MetaPtr& caller, const std::vector<MetaPtr>& args);
  using FunctionMap = std::unordered_map<std::string, Function>;
  struct Property final {
    Function get = nullptr;
    Function set = nullptr;
  };
  using PropertyMap = std::unordered_map<std::string, Property>;
  struct Prototype final {
    std::string class_name;
    Constructor constructor = nullptr;
    Destructor destructor = nullptr;
    PropertyMap static_property_map;
    FunctionMap static_function_map;
    PropertyMap member_property_map;
    FunctionMap member_function_map;
  };
  using WeakPrototype = std::weak_ptr<Prototype>;
  using PrototypePtr = std::shared_ptr<Prototype>;

  static bool Access(const std::string& name,
                     const MetaPtr& caller,
                     const std::vector<MetaPtr>& args,
                     const PropertyMap& property_map,
                     MetaPtr* result) {
    // NOLINTNEXTLINE
    for (auto&& [key, prop] : property_map) {
      if (key != name) {
        continue;
      }
      auto args_count = args.size();
      if (prop.get != nullptr && args_count == 0) {
        *result = prop.get(caller, args);
        return true;
      }
      if (prop.set != nullptr && args_count == 1) {
        *result = prop.set(caller, args);
        return true;
      }
    }
    return false;
  }

  static bool Access(const std::string& name,
                     const MetaPtr& caller,
                     const std::vector<MetaPtr>& args,
                     const FunctionMap& function_map,
                     MetaPtr* result) {
    // NOLINTNEXTLINE
    for (auto&& [key, function] : function_map) {
      if (key == name) {
        *result = Meta::RefFunction(function, caller);
        return true;
      }
    }
    return false;
  }

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
  explicit Meta(Function call_as_function, MetaPtr caller = nullptr)
      : ptr_(reinterpret_cast<void*>(call_as_function)), caller_(std::move(caller)), type_(MetaType::kFunction) {}
  static MetaPtr RefFunction(Function call_as_function, const MetaPtr& caller = nullptr) {
    return std::make_shared<Meta>(call_as_function, caller);
  }
  [[nodiscard]] bool IsFunction() const { return type_ == MetaType::kFunction; }
  MetaPtr CallAsFunction(const std::vector<MetaPtr>& args = {}) {
    auto* call_as_function = reinterpret_cast<Function>(ptr_);
    return call_as_function(caller_, args);
  }

  // object
  explicit Meta(void* object_ptr, Destructor dtor, const PrototypePtr& prototype = nullptr)
      : type_(MetaType::kObject), ptr_(object_ptr), destructor_(dtor), prototype_(prototype) {}
  explicit Meta(void* object_ref, const PrototypePtr& prototype = nullptr)
      : type_(MetaType::kObject), ptr_(object_ref), prototype_(prototype) {}
  static MetaPtr FromObject(void* object_ptr, Destructor dtor, const PrototypePtr& prototype = nullptr) {
    return std::make_shared<Meta>(object_ptr, dtor, prototype);
  }
  static MetaPtr RefObject(void* object_ref, const PrototypePtr& prototype = nullptr) {
    return std::make_shared<Meta>(object_ref, prototype);
  }
  [[nodiscard]] bool IsObject() const { return type_ == MetaType::kObject; }
  MetaPtr Access(const std::string& name, const std::vector<MetaPtr>& args = {}) {
    auto proto = prototype_.lock();
    if (!proto) {
      return nullptr;
    }
    MetaPtr result = nullptr;
    if (Access(name, shared_from_this(), args, proto->member_property_map, &result)) {
      return result;
    }
    if (Access(name, shared_from_this(), args, proto->member_function_map, &result)) {
      return result;
    }
    if (Access(name, nullptr, args, proto->static_function_map, &result)) {
      return result;
    }
    if (Access(name, nullptr, args, proto->static_property_map, &result)) {
      return result;
    }
    return result;
  }

 private:
  static void ReleaseBool(void* ptr) { delete reinterpret_cast<bool*>(ptr); }
  static void ReleaseInt(void* ptr) { delete reinterpret_cast<int*>(ptr); }
  static void ReleaseFloat(void* ptr) { delete reinterpret_cast<float*>(ptr); }
  static void ReleaseDouble(void* ptr) { delete reinterpret_cast<double*>(ptr); }
  static void ReleaseString(void* ptr) { delete reinterpret_cast<std::string*>(ptr); }

  const WeakPrototype prototype_;
  const MetaType type_ = MetaType::kUnknown;
  const MetaPtr caller_;
  void* ptr_ = nullptr;
  const Destructor destructor_ = nullptr;
};

/**
 * MetaCenter is an EXTRA component of dynapse that
 * helps you automatically pregenerate intermmediate
 * code to access a runtime Meta object.
 */
class MetaCenter;
using WeakMetaCenter = std::weak_ptr<MetaCenter>;
using MetaCenterPtr = std::shared_ptr<MetaCenter>;
class MetaCenter final : public std::enable_shared_from_this<MetaCenter> {
 public:
  using PrototypeMap = std::unordered_map<std::string, Meta::PrototypePtr>;
  using WeakPrototypeMap = std::unordered_map<std::string, Meta::WeakPrototype>;

  MetaCenter() = default;
  static std::shared_ptr<MetaCenter> GetDefaultCenter() {
    static auto default_center = std::make_shared<MetaCenter>();
    return default_center;
  }

  void Register(const std::string& path, const Meta::Property& property) { property_map_[path] = property; }
  void Register(const std::string& path, const Meta::Function& function) { function_map_[path] = function; }
  void Register(const Meta::Prototype& prototype) {
    const auto& class_name = prototype.class_name;
    Register(class_name, prototype.member_property_map);
    Register(class_name, prototype.member_function_map);
    Register(class_name, prototype.static_property_map);
    Register(class_name, prototype.static_function_map);
    auto proto = std::make_shared<Meta::Prototype>(prototype);
    constructor_map_[class_name + ".constructor"] = proto;
    prototype_map_[class_name] = proto;
  }

  MetaPtr Access(const std::string& path, const MetaPtr& caller = nullptr, const std::vector<MetaPtr>& args = {}) {
    for (auto&& [key, proto] : constructor_map_) {
      if (key == path) {
        return Meta::RefFunction(CreateObject, Meta::RefObject(&proto));
      }
    }
    MetaPtr result = nullptr;
    if (Meta::Access(path, caller, args, property_map_, &result)) {
      return result;
    }
    if (Meta::Access(path, caller, args, function_map_, &result)) {
      return result;
    }
    return result;
  }

  const Meta::PropertyMap& GetPropertyMap() const { return property_map_; }
  const Meta::FunctionMap& GetFunctionMap() const { return function_map_; }
  const PrototypeMap& GetPrototypeMap() const { return prototype_map_; }

 private:
  static MetaPtr CreateObject(const MetaPtr& caller, const std::vector<MetaPtr>& args) {
    auto proto = caller->As<Meta::WeakPrototype*>()->lock();
    return Meta::FromObject(proto->constructor(args), proto->destructor, proto);
  }

  void Register(const std::string& class_name, const Meta::PropertyMap& property_map) {
    for (auto&& [name, property] : property_map) {
      // clang-format off
      auto path = class_name; path.append(".").append(name);
      // clang-format on
      Register(path, property);
    }
  }
  void Register(const std::string& class_name, const Meta::FunctionMap& function_map) {
    for (auto&& [name, function] : function_map) {
      // clang-format off
      auto path = class_name; path.append(".").append(name);
      // clang-format on
      Register(path, function);
    }
  }

  Meta::PropertyMap property_map_;
  Meta::FunctionMap function_map_;

  WeakPrototypeMap constructor_map_;
  PrototypeMap prototype_map_;
};

}  // namespace dynapse
