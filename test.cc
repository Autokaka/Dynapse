// Created by Autokaka (qq1909698494@gmail.com) on 2023/07/14.

#include <dynapse/dynapse.h>
#include <iostream>

using Meta = dynapse::Meta;
using MetaPtr = dynapse::MetaPtr;
using MetaCenter = dynapse::MetaCenter;

class Foo {
 public:
  static void Register() {
    auto center = MetaCenter::GetDefaultCenter();
    // clang-format off
    DYNMC_DECL_CLASS(center, Foo,
      DYNMC_CLASS_EXTENDS(Foo),
      DYNMC_CONSTRUCTOR([](auto) -> void* { return new Foo; }),
      DYNMC_DESTRUCTOR([](void* ptr) { delete reinterpret_cast<Foo*>(ptr); }),
      DYNMC_DECL_MEMBER_PROPS(
        DYNMC_PROPERTY("a", [](const MetaPtr& foo, auto) { return Meta::RefInt(&foo->As<Foo*>()->a); }),
        DYNMC_PROPERTY("b", [](const MetaPtr& foo, auto) { return Meta::RefFloat(&foo->As<Foo*>()->b); }),
        DYNMC_PROPERTY("c", [](const MetaPtr& foo, auto) { return Meta::RefDouble(&foo->As<Foo*>()->c); }),
        DYNMC_PROPERTY("d", [](const MetaPtr& foo, auto) { return Meta::RefBool(&foo->As<Foo*>()->d); }),
      ),
      DYNMC_DECL_MEMBER_FUNCS(
        DYNMC_FUNCTION("Print", [](const MetaPtr& caller, auto) -> MetaPtr {
            caller->As<Foo*>()->Print();
            return nullptr;
          }),
      ),
    );
    // clang-format on
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
  auto simple_class_meta = center->Access("Foo.constructor")->CallAsFunction();
  std::cout << "Foo is number? " << simple_class_meta->IsNumber() << std::endl;
  std::cout << "Foo is bool? " << simple_class_meta->IsBool() << std::endl;
  std::cout << "Foo is int? " << simple_class_meta->IsInt() << std::endl;
  std::cout << "Foo is float? " << simple_class_meta->IsFloat() << std::endl;
  std::cout << "Foo is double? " << simple_class_meta->IsDouble() << std::endl;
  std::cout << "Foo is string? " << simple_class_meta->IsString() << std::endl;
  std::cout << "Foo is function? " << simple_class_meta->IsFunction() << std::endl;
  std::cout << "Foo is object? " << simple_class_meta->IsObject() << std::endl;

  std::cout << "> TestFoo function reflection" << std::endl;
  // center->Access("Foo.Print", simple_class_meta)->CallAsFunction();
  simple_class_meta->Access("Print")->CallAsFunction();

  std::cout << "> TestFoo property reflection" << std::endl;
  auto simple_class_a_meta = center->Access("Foo.a", simple_class_meta);
  std::cout << "Foo.a? " << *simple_class_a_meta->As<int*>() << std::endl;
  auto simple_class_b_meta = center->Access("Foo.b", simple_class_meta);
  std::cout << "Foo.b? " << *simple_class_b_meta->As<float*>() << std::endl;
  auto simple_class_c_meta = center->Access("Foo.c", simple_class_meta);
  std::cout << "Foo.c? " << *simple_class_c_meta->As<double*>() << std::endl;
  auto simple_class_d_meta = center->Access("Foo.d", simple_class_meta);
  std::cout << "Foo.d? " << *simple_class_d_meta->As<bool*>() << std::endl;
}

int main() {
  Foo::Register();
  TestFoo();
  return 0;
}
