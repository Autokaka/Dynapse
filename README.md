# Dynapse

A tiny header-only C++ runtime reflection library.

The libdynapse consists of:

- Meta: **CORE** functionality of the Dynapse library.
- MetaCenter: **EXTRA** component that helps developers create Dynapse Meta in a more simple and human-readable way.

## A 'Hello World' Example

```shell
#include <dynapse/dynapse.h>
#include <iostream>

void SayHelloToWorld() {
	std::cout << "Hello, World!" << std::endl;
}

int main() {
	// compile-time register
  auto center = dynapse::MetaCenter::GetDefaultCenter();
  center->Register("SayHelloToWorld", [](auto) -> dynapse::MetaPtr {
    SayHelloToWorld();
    return nullptr;
  });
  
  // runtime reflection
  center->DynCall("SayHelloToWorld");
  
  return 0;
}

// cosole will print: Hello, World!
```

## Complex scenario

For those who want to reflect a CPP class, read the `test.cc` under Dynapse source code for now.
