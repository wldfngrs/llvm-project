//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17, c++20
// UNSUPPORTED: availability-pmr-missing

// <flat_set>

#include <algorithm>
#include <cassert>
#include <climits>
#include <deque>
#include <initializer_list>
#include <list>
#include <flat_set>
#include <functional>
#include <memory_resource>
#include <ranges>
#include <type_traits>
#include <utility>
#include <vector>

#include "test_allocator.h"

using P  = std::pair<int, long>;
using PC = std::pair<const int, long>;

int main(int, char**) {
  {
    std::deque<int, test_allocator<int>> ks({1, 2, 1, INT_MAX, 3}, test_allocator<int>(0, 42));
    std::deque<int, test_allocator<int>> sorted_ks({1, 1, 2, 3, INT_MAX}, test_allocator<int>(0, 42));
    const int expected[] = {1, 1, 2, 3, INT_MAX};
    {
      // template<class KeyContainer, class Allocator>
      // flat_multiset(KeyContainer, Allocator)
      //   -> flat_multiset<typename KeyContainer::value_type,
      //                    less<typename KeyContainer::value_type>, KeyContainer>;
      std::pmr::monotonic_buffer_resource mr;
      std::pmr::monotonic_buffer_resource mr2;
      std::pmr::deque<int> pks(ks.begin(), ks.end(), &mr);
      std::flat_multiset s(std::move(pks), &mr2);

      ASSERT_SAME_TYPE(decltype(s), std::flat_multiset<int, std::less<int>, std::pmr::deque<int>>);
      assert(std::ranges::equal(s, expected));
      auto keys = std::move(s).extract();
      assert(keys.get_allocator().resource() == &mr2);
    }
    {
      // template<class KeyContainer, class Allocator>
      // flat_multiset(sorted_equivalent_t, KeyContainer, Allocator)
      //   -> flat_multiset<typename KeyContainer::value_type,
      //                    less<typename KeyContainer::value_type>, KeyContainer>;
      std::pmr::monotonic_buffer_resource mr;
      std::pmr::monotonic_buffer_resource mr2;
      std::pmr::deque<int> pks(sorted_ks.begin(), sorted_ks.end(), &mr);
      std::flat_multiset s(std::sorted_equivalent, std::move(pks), &mr2);

      ASSERT_SAME_TYPE(decltype(s), std::flat_multiset<int, std::less<int>, std::pmr::deque<int>>);
      assert(std::ranges::equal(s, expected));
      auto keys = std::move(s).extract();
      assert(keys.get_allocator().resource() == &mr2);
    }
  }
  {
    std::deque<int, test_allocator<int>> ks({1, 2, 1, INT_MAX, 3}, test_allocator<int>(0, 42));
    std::deque<int, test_allocator<int>> sorted_ks({INT_MAX, 3, 2, 1, 1}, test_allocator<int>(0, 42));
    const int expected[] = {INT_MAX, 3, 2, 1, 1};
    {
      // template<class KeyContainer, class Compare, class Allocator>
      // flat_multiset(KeyContainer, Compare, Allocator)
      //   -> flat_multiset<typename KeyContainer::value_type, Compare, KeyContainer>;
      std::pmr::monotonic_buffer_resource mr;
      std::pmr::monotonic_buffer_resource mr2;
      std::pmr::deque<int> pks(ks.begin(), ks.end(), &mr);
      std::flat_multiset s(std::move(pks), std::greater<int>(), &mr2);

      ASSERT_SAME_TYPE(decltype(s), std::flat_multiset<int, std::greater<int>, std::pmr::deque<int>>);
      assert(std::ranges::equal(s, expected));
      auto keys = std::move(s).extract();
      assert(keys.get_allocator().resource() == &mr2);
    }
    {
      // template<class KeyContainer, class Compare, class Allocator>
      // flat_multiset(sorted_equivalent_t, KeyContainer, Compare, Allocator)
      //   -> flat_multiset<typename KeyContainer::value_type, Compare, KeyContainer>;
      std::pmr::monotonic_buffer_resource mr;
      std::pmr::monotonic_buffer_resource mr2;
      std::pmr::deque<int> pks(sorted_ks.begin(), sorted_ks.end(), &mr);
      std::flat_multiset s(std::sorted_equivalent, std::move(pks), std::greater<int>(), &mr2);

      ASSERT_SAME_TYPE(decltype(s), std::flat_multiset<int, std::greater<int>, std::pmr::deque<int>>);
      assert(std::ranges::equal(s, expected));
      auto keys = std::move(s).extract();
      assert(keys.get_allocator().resource() == &mr2);
    }
  }

  return 0;
}
