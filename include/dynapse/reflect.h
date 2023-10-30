// Created by Autokaka (qq1909698494@gmail.com) on 2023/07/19.

#pragma once

#include "any.h"
#include "types.h"

namespace dynapse {

class Reflect final : public std::enable_shared_from_this<Reflect> {
 public:
  explicit Reflect();

  void Register(const Prototype& prototype);
  PrototypeVector GetPrototypes() const;
  const Prototype* FindPrototype(const std::string& name) const;
  bool SetPrototype(Any& any, const std::string& name);

  Any Apply(const Any& function, const Any& caller = Any::Null(), const Args& args = {}) const;
  Any Apply(Function function, const Any& caller = Any::Null(), const Args& args = {}) const;
  Any Construct(void* ptr, const std::string& name) const;
  Any Construct(const std::string& name, const Args& args = {}) const;
  bool DefineProperty(Any& any, const std::string& prop_key, const Property& prop_desc);
  void DeleteProperty(Any& any, const std::string& prop_key);
  Any Get(const Any& any, const std::string& prop_key) const;
  const Property* GetOwnPropertyDescriptor(const Any& any, const std::string& prop_key) const;
  const Prototype* GetPrototypeOf(const Any& any) const;
  bool Has(const Any& any, const std::string& prop_key) const;
  bool IsExtensible(const Any& any) const;
  std::vector<std::string> OwnKeys(const Any& any) const;
  void PreventExtensions(Any& any);
  bool Set(Any& any, const std::string& prop_key, const Any& prop_value);
  bool SetPrototypeOf(Any& any, const Prototype& prototype);

 private:
  PrototypeMap prototype_map_;
};

Reflect& GetReflect();

std::string TypeOf(const Any& any);

}  // namespace dynapse

// clang-format off
#define DYN_REFLECT_CLASS(ClassName, ...) dynapse::GetReflect().Register({.name = #ClassName, __VA_ARGS__});
#define DYN_EXTENDS(ClassName) .parent_name = #ClassName
#define DYN_CONSTRUCTOR(Constructor) .constructor = Constructor
#define DYN_DESTRUCTOR(Constructor) .destructor = Constructor
#define DYN_DECL_MEMBER_PROPS(...) .member_property_map = {__VA_ARGS__}
#define DYN_DECL_STATIC_PROPS(...) .static_property_map = {__VA_ARGS__}
#define DYN_PROPERTY(AccessMode, PropertyName, ...) DYN_##AccessMode##_PROPERTY(PropertyName, __VA_ARGS__)
#define DYN_R_PROPERTY(PropertyName, Getter) {PropertyName, dynapse::Property{.get = Getter}}
#define DYN_W_PROPERTY(PropertyName, Setter) {PropertyName, dynapse::Property{.set = Setter}}
#define DYN_RW_PROPERTY(PropertyName, Getter, Setter) {PropertyName, dynapse::Property{.get = Getter, .set = Setter}}
#define DYN_RV_PROPERTY(PropertyName, Value) {PropertyName, dynapse::Property{.readonly = true, .value = Value}}
#define DYN_V_PROPERTY(PropertyName, Value) {PropertyName, dynapse::Property{.value = Value}}
#define DYN_DECL_MEMBER_FUNCS(...) .member_function_map = {__VA_ARGS__}
#define DYN_DECL_STATIC_FUNCS(...) .static_function_map = {__VA_ARGS__}
#define DYN_FUNCTION(FunctionName, Callback) {FunctionName, Callback}
// clang-format on
