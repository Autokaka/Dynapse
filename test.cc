// Created by Autokaka (qq1909698494@gmail.com) on 2023/07/14.

#include <dynapse/dynapse.h>
#include <iostream>

// NOLINTNEXTLINE
using namespace dynapse;

class Foo {
 public:
  void Print() const { std::cout << "Hello, this is " << foo_ << "!" << '\n'; }
  [[nodiscard]] std::string Get() const { return foo_; }

  // NOLINTNEXTLINE
  int aaa = 114514;
  // NOLINTNEXTLINE
  float bbb = 123.5;
  bool ccc = true;

 private:
  std::string foo_ = "foo";
};

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
        std::cout << "Setter invoked!" << '\n';
        return Any::Null();
      }),
      DYN_PROPERTY(V, "size", make_any(GetReflect().Construct(new int(0), "int"))),
    ),
  );

  DYN_REFLECT_CLASS(Foo,
    DYN_CONSTRUCTOR([](auto) -> void* { return new Foo(); }),
    DYN_DESTRUCTOR([](void* ptr) { delete static_cast<Foo*>(ptr); }),
    DYN_DECL_MEMBER_PROPS(
      DYN_PROPERTY(RW, "aaa", [](const Any& caller, const Args& args) -> Any {
        void* aaa_ptr = new int(caller.As<Foo*>()->aaa);
        return GetReflect().Construct(aaa_ptr, "int");
      }, [](const Any& caller, const Args& args) -> Any {
        std::cout << "Foo::aaa setter invoked!" << '\n';
        caller.As<Foo*>()->aaa = args[0].To<int>();
        return Any::Null();
      }),
      DYN_PROPERTY(RW, "bbb", [](const Any& caller, const Args& args) -> Any {
        void* bbb_ptr = new float(caller.As<Foo*>()->bbb);
        return GetReflect().Construct(bbb_ptr, "float");
      }, [](const Any& caller, const Args& args) -> Any {
        std::cout << "Foo::bbb setter invoked!" << '\n';
        caller.As<Foo*>()->bbb = args[0].To<float>();
        return Any::Null();
      }),
      DYN_PROPERTY(RW, "ccc", [](const Any& caller, const Args& args) -> Any {
        void* ccc_ptr = new bool(caller.As<Foo*>()->ccc);
        return GetReflect().Construct(ccc_ptr, "bool");
      }, [](const Any& caller, const Args& args) -> Any {
        std::cout << "Foo::ccc setter invoked!" << '\n';
        caller.As<Foo*>()->ccc = args[0].To<bool>();
        return Any::Null();
      }),
    ),
    DYN_DECL_MEMBER_FUNCS(
      DYN_FUNCTION("Print", [](const Any& caller, const Args& args) -> Any {
        caller.As<Foo*>()->Print();
        return Any::Null();
      }),
    ),
  );
  // clang-format on

  // Test `String`

  auto reflect = GetReflect();
  auto string = reflect.Construct("String");
  string.As<std::string*>()->append("Hello, World!");
  std::cout << "String: " << string.To<std::string>() << '\n';

  std::cout << "String.length: " << string["length"].To<int>() << '\n';
  string["length"] = Any::Null();
  std::cout << "String.length: " << string["length"].To<int>() << '\n';

  std::cout << "String.size: " << string["size"].To<int>() << '\n';
  // NOLINTNEXTLINE
  string["size"] = reflect.Construct(new int(10), "int");
  std::cout << "String.size: " << string["size"].To<int>() << '\n';

  // Test `Foo`

  auto foo = reflect.Construct("Foo");
  foo["Print"]();
  std::cout << "Foo.aaa: " << foo["aaa"].To<int>() << '\n';
  std::cout << "typeof Foo.aaa: " << TypeOf(foo["aaa"]) << '\n';
  // NOLINTNEXTLINE
  foo["aaa"] = reflect.Construct(new int(1024), "int");
  std::cout << "Foo.aaa: " << foo["aaa"].To<int>() << '\n';

  std::cout << "Foo.bbb: " << foo["bbb"].To<float>() << '\n';
  std::cout << "typeof Foo.bbb: " << TypeOf(foo["bbb"]) << '\n';
  // NOLINTNEXTLINE
  foo["bbb"] = reflect.Construct(new float(1024.1024), "float");
  std::cout << "Foo.bbb: " << foo["bbb"].To<float>() << '\n';

  return 0;
}
