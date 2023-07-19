// Created by Autokaka (qq1909698494@gmail.com) on 2023/07/19.

#include <dynapse/prototype.h>

namespace dynapse {

PrototypePtr make_prototype(const Prototype& proto) {
  return std::make_shared<Prototype>(proto);
}

}  // namespace dynapse
