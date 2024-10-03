// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <any>
#include <array>
#include <forward_list>
#include <string>

#include <benchmark/benchmark.h>

#include "proxy.h"

namespace {

constexpr int TestManagedObjectCount = 12000;

}  // namespace

struct DefaultFacade : pro::facade_builder::build {};

void BM_SmallObjectManagementWithProxy(benchmark::State& state) {
  for (auto _ : state) {
    std::vector<pro::proxy<DefaultFacade>> data;
    data.reserve(TestManagedObjectCount);
    for (int i = 0; i < TestManagedObjectCount; i += 3) {
      auto p0 = pro::make_proxy<DefaultFacade>(123);
      auto p1 = pro::make_proxy<DefaultFacade, std::shared_ptr<int>>();
      auto p2 = pro::make_proxy<DefaultFacade, std::forward_list<double>>();

      benchmark::DoNotOptimize(p0);
      benchmark::DoNotOptimize(p1);
      benchmark::DoNotOptimize(p2);

      data.push_back(std::move(p0));
      data.push_back(std::move(p1));
      data.push_back(std::move(p2));
    }
  }
}

void BM_SmallObjectManagementWithAny(benchmark::State& state) {
  for (auto _ : state) {
    std::vector<std::any> data;
    data.reserve(TestManagedObjectCount);
    for (int i = 0; i < TestManagedObjectCount; i += 3) {
      std::any a0 = 123;
      std::any a1 = std::shared_ptr<int>{};
      std::any a2 = std::forward_list<double>{};

      benchmark::DoNotOptimize(a0);
      benchmark::DoNotOptimize(a1);
      benchmark::DoNotOptimize(a2);

      data.push_back(std::move(a0));
      data.push_back(std::move(a1));
      data.push_back(std::move(a2));
    }
  }
}

void BM_LargeObjectManagementWithProxy(benchmark::State& state) {
  for (auto _ : state) {
    std::vector<pro::proxy<DefaultFacade>> data;
    data.reserve(TestManagedObjectCount);
    for (int i = 0; i < TestManagedObjectCount; ++i) {
      auto p = pro::make_proxy<DefaultFacade, std::array<std::string, 3>>();
      benchmark::DoNotOptimize(p);
      data.push_back(std::move(p));
    }
    benchmark::DoNotOptimize(data);
  }
}

void BM_LargeObjectManagementWithAny(benchmark::State& state) {
  for (auto _ : state) {
    std::vector<std::any> data;
    data.reserve(TestManagedObjectCount);
    for (int i = 0; i < TestManagedObjectCount; ++i) {
      std::any a = std::array<std::string, 3>{};
      benchmark::DoNotOptimize(a);
      data.push_back(std::move(a));
    }
    benchmark::DoNotOptimize(data);
  }
}

BENCHMARK(BM_SmallObjectManagementWithProxy);
BENCHMARK(BM_SmallObjectManagementWithAny);
BENCHMARK(BM_LargeObjectManagementWithProxy);
BENCHMARK(BM_LargeObjectManagementWithAny);
