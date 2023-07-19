// Created by Autokaka (qq1909698494@gmail.com) on 2023/07/19.

#pragma once

#include "any.h"
#include "types.h"

namespace dynapse {

class Reflect final : public std::enable_shared_from_this<Reflect> {
 public:
  static ReflectPtr Get();

  void Register(const Prototype& prototype);
  PrototypeVector GetPrototypes() const;
  PrototypePtr GetPrototypeOf(const std::string& name) const;
  Any Construct(const std::string& name, const Args& args = {}) const;

 private:
  Reflect() = default;
  PrototypeMap prototype_map_;
};

}  // namespace dynapse

// clang-format off
#define DYN_REFLECT_CLASS(ClassName, ...) dynapse::Reflect::Get()->Register({.name = #ClassName, __VA_ARGS__});
#define DYN_EXTENDS(ClassName) .parent_name = #ClassName
#define DYN_CONSTRUCTOR(Constructor) .constructor = Constructor
#define DYN_DESTRUCTOR(Constructor) .destructor = Constructor
#define DYN_DECL_MEMBER_PROPS(...) .member_property_map = {__VA_ARGS__}
#define DYN_DECL_STATIC_PROPS(...) .static_property_map = {__VA_ARGS__}
#define DYN_PROPERTY(PropertyName, Getter) { PropertyName, { .get = Getter }}
#define DYN_READONLY_PROPERTY(PropertyName, Getter) {#PropertyName, { .readonly = true, .get = Getter }}
#define DYN_DECL_MEMBER_FUNCS(...) .member_function_map = {__VA_ARGS__}
#define DYN_DECL_STATIC_FUNCS(...) .static_function_map = {__VA_ARGS__}
#define DYN_FUNCTION(FunctionName, Callback) {FunctionName, Callback}
// clang-format on
