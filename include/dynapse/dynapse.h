// Created by Autokaka (qq1909698494@gmail.com) on 2023/07/14.

#pragma once

#include <memory>
#include <string>

namespace dynapse {

/**
 * Meta serves as the CORE functionality of dynapse,
 * if you want the simplest and the most direct way to
 * use dynapse, Meta is just enough.
 */
class Meta;
using MetaPtr = std::shared_ptr<Meta>;
class Meta final {
 public:
  static constexpr char kUnknownType = 0;
  static constexpr char kBoolType = 1 << 0;
  static constexpr char kIntType = 1 << 1;
  static constexpr char kFloatType = 1 << 2;
  static constexpr char kDoubleType = 1 << 3;
  static constexpr char kStringType = 1 << 4;
  static constexpr char kFunctionType = 1 << 5;
  static constexpr char kObjectType = 1 << 6;

  // any
  explicit Meta() = default;
  Meta(const Meta&) = delete;
  Meta& operator=(const Meta&) = delete;
  Meta(Meta&&) = delete;
  Meta& operator=(Meta&&) = delete;
  ~Meta() {
    if (release_ != nullptr) {
      release_(ptr_);
    }
  }

  bool Owned() { return release_ != nullptr; }
  template <typename ClassType>
  [[nodiscard]] ClassType* As() const {
    return reinterpret_cast<ClassType*>(ptr_);
  }

  // number
  [[nodiscard]] bool IsNumber() const {
    auto type = kBoolType | kIntType | kFloatType | kDoubleType;
    return (type_ & type) != 0;
  }

  explicit Meta(bool bool_value) : ptr_(new bool(bool_value)), type_(kBoolType), release_(ReleaseBool) {}
  static MetaPtr FromBool(bool bool_value) { return std::make_shared<Meta>(bool_value); }
  [[nodiscard]] bool IsBool() const { return (type_ & kBoolType) != 0; }
  [[nodiscard]] bool ToBool() const { return *As<bool>(); }

  explicit Meta(int int_value) : ptr_(new int(int_value)), type_(kIntType), release_(ReleaseInt) {}
  static MetaPtr FromInt(int int_value) { return std::make_shared<Meta>(int_value); }
  [[nodiscard]] bool IsInt() const { return (type_ & kIntType) != 0; }
  [[nodiscard]] int ToInt() const { return *As<int>(); }

  explicit Meta(float float_value) : ptr_(new float(float_value)), type_(kFloatType), release_(ReleaseFloat) {}
  static MetaPtr FromFloat(float float_value) { return std::make_shared<Meta>(float_value); }
  [[nodiscard]] bool IsFloat() const { return (type_ & kFloatType) != 0; }
  [[nodiscard]] float ToFloat() const { return *As<float>(); }

  explicit Meta(double double_value) : ptr_(new double(double_value)), type_(kDoubleType), release_(ReleaseDouble) {}
  static MetaPtr FromDouble(double double_value) { return std::make_shared<Meta>(double_value); }
  [[nodiscard]] bool IsDouble() const { return (type_ & kDoubleType) != 0; }
  [[nodiscard]] double ToDouble() const { return *As<double>(); }

  // string
  explicit Meta(const std::string& string_value)
      : ptr_(new std::string(string_value)), type_(kStringType), release_(ReleaseString) {}
  static MetaPtr FromString(const std::string& string_value) { return std::make_shared<Meta>(string_value); }
  [[nodiscard]] bool IsString() const { return (type_ & kStringType) != 0; }
  [[nodiscard]] std::string ToString() const { return *As<std::string>(); }

  // function
  using CallAsFunctionCallback = MetaPtr (*)(const MetaPtr& args);
  explicit Meta(CallAsFunctionCallback call_as_function)
      : ptr_(reinterpret_cast<void*>(call_as_function)), type_(kFunctionType) {}
  static MetaPtr FromFunction(CallAsFunctionCallback call_as_function) {
    return std::make_shared<Meta>(call_as_function);
  }
  [[nodiscard]] bool IsFunction() const { return (type_ & kFunctionType) != 0; }
  MetaPtr CallAsFunction(const MetaPtr& args = nullptr) {
    auto* call_as_function = reinterpret_cast<CallAsFunctionCallback>(ptr_);
    return call_as_function(args);
  }

  // object
  using GetObjectPropertyCallback = MetaPtr (*)(void* object_ref, const MetaPtr& property_key);
  using SetObjectPropertyCallback = void (*)(void* object_ref, const MetaPtr& property_key, const MetaPtr& value);
  explicit Meta(void* object_ptr,
                void (*release)(void* ptr),
                GetObjectPropertyCallback property_getter,
                SetObjectPropertyCallback property_setter)
      : type_(kObjectType),
        ptr_(object_ptr),
        release_(release),
        get_object_property_(property_getter),
        set_object_property_(property_setter) {}
  static MetaPtr FromObject(void* object_ptr,
                            void (*release)(void* ptr),
                            GetObjectPropertyCallback property_getter = nullptr,
                            SetObjectPropertyCallback property_setter = nullptr) {
    return std::make_shared<Meta>(object_ptr, release, property_getter, property_setter);
  }
  explicit Meta(void* object_ref, GetObjectPropertyCallback property_getter, SetObjectPropertyCallback property_setter)
      : type_(kObjectType),
        ptr_(object_ref),
        get_object_property_(property_getter),
        set_object_property_(property_setter) {}
  static MetaPtr FromObject(void* object_ref,
                            GetObjectPropertyCallback property_getter = nullptr,
                            SetObjectPropertyCallback property_setter = nullptr) {
    return std::make_shared<Meta>(object_ref, property_getter, property_setter);
  }
  [[nodiscard]] bool IsObject() const { return (type_ & kObjectType) != 0; }
  MetaPtr GetObjectProperty(const std::string& property_key) {
    if (get_object_property_ != nullptr) {
      return get_object_property_(ptr_, Meta::FromString(property_key));
    }
    return nullptr;
  }
  void SetObjectProperty(const std::string& property_key, const MetaPtr& value) {
    if (set_object_property_ != nullptr) {
      set_object_property_(ptr_, Meta::FromString(property_key), value);
    }
  }
  MetaPtr GetObjectProperty(int index) { return get_object_property_(ptr_, Meta::FromInt(index)); }
  void SetObjectProperty(int index, const MetaPtr& value) { set_object_property_(ptr_, Meta::FromInt(index), value); }

 private:
  // any
  void (*release_)(void* ptr);

  static void ReleaseBool(void* ptr) { delete reinterpret_cast<bool*>(ptr); }
  static void ReleaseInt(void* ptr) { delete reinterpret_cast<int*>(ptr); }
  static void ReleaseFloat(void* ptr) { delete reinterpret_cast<float*>(ptr); }
  static void ReleaseDouble(void* ptr) { delete reinterpret_cast<double*>(ptr); }
  static void ReleaseString(void* ptr) { delete reinterpret_cast<std::string*>(ptr); }

  // object
  GetObjectPropertyCallback get_object_property_;
  SetObjectPropertyCallback set_object_property_;

  const char type_ = kUnknownType;
  void* ptr_ = nullptr;
};
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
  struct ClassRegistry final {
    Meta::CallAsFunctionCallback ctor;
    std::unordered_map<std::string, MetaPtr> member_props;
    std::unordered_map<std::string, Meta::CallAsFunctionCallback> member_fns;
    std::unordered_map<std::string, MetaPtr> static_props;
    std::unordered_map<std::string, Meta::CallAsFunctionCallback> static_fns;
  };
  MetaCenter() = default;

  static std::shared_ptr<MetaCenter> GetDefaultCenter() {
    static auto default_center = std::make_shared<MetaCenter>();
    return default_center;
  }

  void Register(const std::string& class_name, const ClassRegistry& registry) {
    meta_map_[class_name + ".constructor"] = Meta::FromFunction(registry.ctor);
    for (auto&& [prop_key, prop_meta] : registry.member_props) {
      // clang-format off
      std::string path = class_name; path.append(".").append(prop_key);
      // clang-format on
      Register(path, prop_meta);
    }
    for (auto&& [prop_key, member_fn] : registry.member_fns) {
      // clang-format off
      std::string path = class_name; path.append(".").append(prop_key);
      // clang-format on
      Register(path, member_fn);
    }
    for (auto&& [prop_key, prop_meta] : registry.static_props) {
      // clang-format off
      std::string path = class_name; path.append(".").append(prop_key);
      // clang-format on
      Register(path, prop_meta);
    }
    for (auto&& [prop_key, static_fn] : registry.static_fns) {
      // clang-format off
      std::string path = class_name; path.append(".").append(prop_key);
      // clang-format on
      Register(path, static_fn);
    }
  }

  void Register(const std::string& path, const MetaPtr& static_prop) { meta_map_[path] = static_prop; }
  void Register(const std::string& path, const Meta::CallAsFunctionCallback& function) {
    meta_map_[path] = Meta::FromFunction(function);
  }

  // runtime unified access method
  MetaPtr DynCall(const std::string& path, const MetaPtr& args = nullptr) {
    auto meta = meta_map_[path];
    if (meta == nullptr) {
      return nullptr;
    }
    if (meta->IsNumber() || meta->IsString()) {
      return meta;
    }
    if (meta->IsFunction()) {
      return meta->CallAsFunction(args);
    }
    if (meta->IsObject()) {
      return meta;
    }
    // code will never reach here
    std::abort();
  }

 private:
  std::unordered_map<std::string, MetaPtr> meta_map_;
};
using MetaCenterPtr = std::shared_ptr<MetaCenter>;

#endif

}  // namespace dynapse
