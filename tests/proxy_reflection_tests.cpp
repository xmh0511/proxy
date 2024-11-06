// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <gtest/gtest.h>
#include <memory>
#include <typeinfo>
#include "proxy.h"
#include "utils.h"

namespace {

struct TraitsReflector {
 public:
  template <class T>
  constexpr explicit TraitsReflector(std::in_place_type_t<T>)
      : is_default_constructible_(std::is_default_constructible_v<T>),
        is_copy_constructible_(std::is_copy_constructible_v<T>),
        is_nothrow_move_constructible_(std::is_nothrow_move_constructible_v<T>),
        is_nothrow_destructible_(std::is_nothrow_destructible_v<T>),
        is_trivial_(std::is_trivial_v<T>) {}

  PRO_DEF_REFL_AS_MEM_ACCESSOR(ReflectTraits);

  bool is_default_constructible_;
  bool is_copy_constructible_;
  bool is_nothrow_move_constructible_;
  bool is_nothrow_destructible_;
  bool is_trivial_;
};

struct TestRttiFacade : pro::facade_builder
    ::add_reflection<utils::RttiReflector>
    ::add_direct_reflection<utils::RttiReflector>
    ::build {};

struct TestTraitsFacade : pro::facade_builder
    ::add_direct_reflection<TraitsReflector>
    ::build {};

}  // namespace

TEST(ProxyReflectionTests, TestRtti_RawPtr) {
  int foo = 123;
  pro::proxy<TestRttiFacade> p = &foo;
  ASSERT_STREQ(p.ReflectRtti().GetName(), typeid(int*).name());
  ASSERT_STREQ(p->ReflectRtti().GetName(), typeid(int).name());
}

TEST(ProxyReflectionTests, TestRtti_FancyPtr) {
  pro::proxy<TestRttiFacade> p = std::make_unique<double>(1.23);
  ASSERT_STREQ(p.ReflectRtti().GetName(), typeid(std::unique_ptr<double>).name());
  ASSERT_STREQ(p->ReflectRtti().GetName(), typeid(double).name());
}

TEST(ProxyReflectionTests, TestTraits_RawPtr) {
  int foo = 123;
  pro::proxy<TestTraitsFacade> p = &foo;
  ASSERT_EQ(p.ReflectTraits().is_default_constructible_, true);
  ASSERT_EQ(p.ReflectTraits().is_copy_constructible_, true);
  ASSERT_EQ(p.ReflectTraits().is_nothrow_move_constructible_, true);
  ASSERT_EQ(p.ReflectTraits().is_nothrow_destructible_, true);
  ASSERT_EQ(p.ReflectTraits().is_trivial_, true);
}

TEST(ProxyReflectionTests, TestTraits_FancyPtr) {
  pro::proxy<TestTraitsFacade> p = std::make_unique<double>(1.23);
  ASSERT_EQ(p.ReflectTraits().is_default_constructible_, true);
  ASSERT_EQ(p.ReflectTraits().is_copy_constructible_, false);
  ASSERT_EQ(p.ReflectTraits().is_nothrow_move_constructible_, true);
  ASSERT_EQ(p.ReflectTraits().is_nothrow_destructible_, true);
  ASSERT_EQ(p.ReflectTraits().is_trivial_, false);
}
