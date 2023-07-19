// Created by Autokaka (qq1909698494@gmail.com) on 2023/07/19.

#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace dynapse {

class Any;
using Args = std::vector<Any>;

using Constructor = void* (*)(const Args& args);
using Destructor = void (*)(void* ptr);

using Function = Any (*)(const Any& caller, const Args& args);
using FunctionMap = std::unordered_map<std::string, Function>;

struct Property;
using PropertyMap = std::unordered_map<std::string, Property>;

struct Prototype;
using PrototypePtr = std::shared_ptr<Prototype>;
using PrototypeMap = std::unordered_map<std::string, PrototypePtr>;
using PrototypeVector = std::vector<PrototypePtr>;

class Reflect;
using ReflectPtr = std::shared_ptr<Reflect>;

}  // namespace dynapse
