// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
// This file contains example code from add_reflection.md.

#include <iostream>
#include <typeinfo>

#include "proxy.h"

class RttiReflector {
 public:
  template <class T>
  constexpr explicit RttiReflector(std::in_place_type_t<T>) : type_(typeid(T)) {}

  PRO_DEF_REFL_AS_MEM_ACCESSOR(ReflectRtti);
  const char* GetName() const noexcept { return type_.name(); }

 private:
  const std::type_info& type_;
};

struct RttiAware : pro::facade_builder
    ::add_direct_reflection<RttiReflector>
    ::add_indirect_reflection<RttiReflector>
    ::build {};

int main() {
  int a = 123;
  pro::proxy<RttiAware> p = &a;
  std::cout << p.ReflectRtti().GetName() << "\n";  // Prints: "Pi" (assuming GCC)
  std::cout << p->ReflectRtti().GetName() << "\n";  // Prints: "i" (assuming GCC)
}
