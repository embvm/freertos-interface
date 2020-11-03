// Copyright 2020 Embedded Artistry LLC
// SPDX-License-Identifier: MIT

#ifndef FREERTOS_HEAP_HPP_
#define FREERTOS_HEAP_HPP_

#include <rtos/heap.hpp>

namespace os
{
namespace freertos
{
/// @addtogroup FreeRTOSOS
/// @{

class Heap
{
  public:
	static void addBlock(void* addr, size_t size) noexcept;
	static void init() noexcept;
	static void* alloc(size_t size) noexcept;
	static void free(void* addr) noexcept;
};

/// @}
} // namespace freertos

using Heap = embvm::VirtualHeap<os::freertos::Heap>;

} // namespace os

#endif FREERTOS_HEAP_HPP_
