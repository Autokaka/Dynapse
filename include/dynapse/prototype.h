// Created by Autokaka (qq1909698494@gmail.com) on 2023/07/19.

#pragma once

#include "types.h"

namespace dynapse {

struct Property final {
  bool configurable = true;
  bool enumerable = true;
  // as value
  bool readonly = false;
  AnyPtr value = nullptr;
  // as accessor
  Function get = nullptr;
  Function set = nullptr;
};

struct Prototype final {
  std::string name;
  std::string parent_name;
  bool extensible = true;
  // as function
  Function call_as_function = nullptr;
  // as value
  Constructor constructor = nullptr;
  Destructor destructor = nullptr;
  Function copy = nullptr;
  Function assign = nullptr;
  // as object
  PropertyMap static_property_map;
  FunctionMap static_function_map;
  PropertyMap member_property_map;
  FunctionMap member_function_map;
};

}  // namespace dynapse
