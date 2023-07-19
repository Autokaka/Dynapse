// Created by Autokaka (qq1909698494@gmail.com) on 2023/07/14.

#include <dynapse/dynapse.h>
#include <iostream>

// NOLINTNEXTLINE
using namespace dynapse;

int main() {
  static auto int_proto = make_prototype({.destructor = [](void* ptr) { delete static_cast<int*>(ptr); }});

  // clang-format off
  DYN_REFLECT_CLASS(String,
    DYN_CONSTRUCTOR([](auto) -> void* { return new std::string(); }),
    DYN_DESTRUCTOR([](void* ptr) { delete static_cast<std::string*>(ptr); }),
    DYN_DECL_MEMBER_PROPS(
      DYN_PROPERTY("length", [](const Any& caller, auto) -> Any { 
        auto length = caller.As<std::string*>()->length();
        return Any(new int(length), int_proto);
      }),
    ),
  )
  // clang-format on

  auto reflect = Reflect::Get();
  auto string = reflect->Construct("String");
  string.As<std::string*>()->append("Hello, World!");
  std::cout << "String: " << string.To<std::string>() << std::endl;
  std::cout << "String.length: " << string["length"].To<int>() << std::endl;

  return 0;
}
