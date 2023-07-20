// Created by Autokaka (qq1909698494@gmail.com) on 2023/07/14.

#include <dynapse/dynapse.h>
#include <iostream>

// NOLINTNEXTLINE
using namespace dynapse;

int main() {
  // clang-format off
  DYN_REFLECT_CLASS(String,
    DYN_CONSTRUCTOR([](auto) -> void* { return new std::string(); }),
    DYN_DESTRUCTOR([](void* ptr) { delete static_cast<std::string*>(ptr); }),
    DYN_DECL_MEMBER_PROPS(
      DYN_PROPERTY(R, "length", [](const Any& caller, auto) -> Any { 
        auto length = caller.As<std::string*>()->length();
        return GetReflect()->Construct(new int(length), "int");
      }),
    ),
  )
  // clang-format on

  auto reflect = GetReflect();
  auto string = reflect->Construct("String");
  string.As<std::string*>()->append("Hello, World!");
  std::cout << "String: " << string.To<std::string>() << std::endl;
  std::cout << "String.length: " << string["length"].To<int>() << std::endl;

  return 0;
}
