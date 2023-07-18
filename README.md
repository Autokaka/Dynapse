# Dynapse

A tiny header-only C++ runtime reflection library.

The libdynapse consists of:

- Meta: **CORE** functionality of the Dynapse library.
- MetaCenter: **EXTRA** component that helps developers create Dynapse Meta in a more simple and human-readable way.

## A 'Hello World' Example

```cpp
#include <dynapse/dynapse.h>
#include <iostream>

void SayHelloToWorld() {
  std::cout << "Hello, World!" << std::endl;
}

int main() {
  // compile-time register
  auto center = dynapse::MetaCenter::GetDefaultCenter();
  center->Register("SayHelloToWorld", [](auto, auto) -> dynapse::MetaPtr {
    SayHelloToWorld();
    return nullptr;
  });
  
  // runtime reflection
  center->Access("SayHelloToWorld")->CallAsFunction();
  
  return 0;
}

// console will print: Hello, World!
```

## Complex Scenario

For those who want to reflect a CPP class, read the `test.cc` under Dynapse source code for now.

## Another Runtime Reflection Story

I've been thinking about designing a "proper" runtime reflection library for quite a long time, those already existed ones are too complex and too boring to read the source code.

From my perspective, without knowing any stupid outer logic, a runtime reflection library should only consider:

1. How to convert all of the CPP class functions, properties, static functions, etc. to the corresbonding **static function calls**.

1.1 How to design those static function callbacks to **request users** to convert their desired value to a void pointer and convert it back.

2. How the **other runtime system should interoperate** with this runtime reflection library.

When all of the above design points are confirmed, a runtime reflection library firstly provide users with some **static function slots a compile time** that expect the users to **convert a void pointer** back to its original **user-defined data structure** to access their original CPP functions/properties, etc., then convert the **resulted user-defined data structure** to a **void pointer** and return it back. **At runtime time**, users should be able to dynamically **call the static function slots** that are pre-registered at compile time with **a std::string as the function name and some void pointers as the arguments**.

- For the type basic value conversion part, I take the JavaScript type design as the reference. For JavaScript, all of the types could be considered as `undefined`, `null`, `number`, `bigint`, `boolean`, `symbol`, `string`, `function` and `object`. For CPP, to make things simple, the runtime reflection library should provide users with an **intermmediate AnyClass** that could simply **own or reference** a `nullptr`, `number`, `string`, `function` or `object` typed CPP value.
- For the **static function slots generation** part, I defined a structure named `ClassRegistry` to store all of the static function callbacks when users want to construct/destroy/access property/call a function. The users will register a `ClassRegistry` to generate and bind static functions with a std::string name.

Finally the fake code should look like this:

```
ReflectionLibrary.register(className, {
	constructor: (Any args) => {
		const args = args.toArray();
		return new FooClass(arg[0], arg[1], ...);
	},
	memberProperties: {
		"bar": {
			readonly: false,
			get: (Any owner) => {
				const foo = owner.to<FooClass>();
        return Any.ref(foo.bar);
      },
		},
	},
	memberFunctions: {
		
	},
});
```
