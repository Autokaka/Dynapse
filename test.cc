// Created by Autokaka (qq1909698494@gmail.com) on 2023/07/14.

#include <dynapse/dynapse.h>
#include <iostream>
#include "dynapse/reflect.h"

// NOLINTNEXTLINE
using namespace dynapse;

int main() {
  // clang-format off
  DYN_REFLECT_CLASS(String,
    DYN_CONSTRUCTOR([](auto) -> void* { return new std::string(); }),
    DYN_DESTRUCTOR([](void* ptr) { delete static_cast<std::string*>(ptr); }),
    DYN_DECL_MEMBER_PROPS(
      DYN_PROPERTY(RW, "length", [](const Any& caller, auto) -> Any { 
        auto length = caller.As<std::string*>()->length();
        return GetReflect().Construct(new int(length), "int");
      }, [](const Any& caller, auto) -> Any { 
        std::cout << "Setter invoked!" << std::endl;
        return Any::Null();
      }),
      DYN_PROPERTY(V, "size", make_any(GetReflect().Construct(new int(0), "int"))),
    ),
  )
  // clang-format on

  auto reflect = GetReflect();
  auto string = reflect.Construct("String");
  string.As<std::string*>()->append("Hello, World!");
  std::cout << "String: " << string.To<std::string>() << std::endl;
  std::cout << "String.length: " << string["length"].To<int>() << std::endl;
  string["length"] = Any::Null();
  // NOLINTNEXTLINE
  string["size"] = reflect.Construct(new int(10), "int");
  // FIXME(Autokaka): make it 10!
  std::cout << "String.size: " << string["size"].To<int>() << std::endl;

  return 0;
}
