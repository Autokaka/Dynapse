// Created by Autokaka (qq1909698494@gmail.com) on 2023/07/14.

#include <dynapse/dynapse.h>
#include <iostream>

using Meta = dynapse::Meta;
using MetaPtr = dynapse::MetaPtr;
using MetaCenter = dynapse::MetaCenter;

class Foo {
 public:
  static void Register() {
    MetaCenter::ClassRegistry registry;
    registry.ctor = [](auto) -> MetaPtr {
      return Meta::FromObject(new Foo, [](void* ptr) { delete reinterpret_cast<Foo*>(ptr); });
    };
    registry.member_props["a"] = Meta::FromInt(kAValue);
    registry.member_props["b"] = Meta::FromFloat(kBValue);
    registry.member_props["c"] = Meta::FromDouble(kCValue);
    registry.member_props["d"] = Meta::FromBool(kDValue);
    registry.member_fns["Print"] = [](const MetaPtr& args) -> MetaPtr {
      args->As<Foo>()->Print();
      return nullptr;
    };
    MetaCenter::GetDefaultCenter()->Register("Foo", registry);
  }

  static constexpr int kAValue = 10;
  static constexpr float kBValue = 20.0;
  static constexpr double kCValue = 30.0;
  static constexpr bool kDValue = true;

  int a = kAValue;
  float b = kBValue;
  double c = kCValue;
  bool d = kDValue;

  void Print() { std::cout << "Hello, " << world_ << "!" << std::endl; }

 private:
  std::string world_ = "Foo";
};

void TestFoo() {
  auto center = MetaCenter::GetDefaultCenter();

  std::cout << "> TestFoo class reflection" << std::endl;
  auto simple_class_meta = center->DynCall("Foo.constructor");
  std::cout << "Foo is number? " << simple_class_meta->IsNumber() << std::endl;
  std::cout << "Foo is bool? " << simple_class_meta->IsBool() << std::endl;
  std::cout << "Foo is int? " << simple_class_meta->IsInt() << std::endl;
  std::cout << "Foo is float? " << simple_class_meta->IsFloat() << std::endl;
  std::cout << "Foo is double? " << simple_class_meta->IsDouble() << std::endl;
  std::cout << "Foo is string? " << simple_class_meta->IsString() << std::endl;
  std::cout << "Foo is function? " << simple_class_meta->IsFunction() << std::endl;
  std::cout << "Foo is object? " << simple_class_meta->IsObject() << std::endl;

  std::cout << "> TestFoo function reflection" << std::endl;
  center->DynCall("Foo.Print", simple_class_meta);

  std::cout << "> TestFoo property reflection" << std::endl;
  auto simple_class_a_meta = center->DynCall("Foo.a", simple_class_meta);
  std::cout << "Foo.a? " << simple_class_a_meta->ToInt() << std::endl;
  auto simple_class_b_meta = center->DynCall("Foo.b", simple_class_meta);
  std::cout << "Foo.b? " << simple_class_b_meta->ToFloat() << std::endl;
  auto simple_class_c_meta = center->DynCall("Foo.c", simple_class_meta);
  std::cout << "Foo.c? " << simple_class_c_meta->ToDouble() << std::endl;
  auto simple_class_d_meta = center->DynCall("Foo.d", simple_class_meta);
  std::cout << "Foo.d? " << simple_class_d_meta->ToBool() << std::endl;
}

int main() {
  Foo::Register();
  TestFoo();
  return 0;
}
