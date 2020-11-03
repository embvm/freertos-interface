// Copyright 2020 Embedded Artistry LLC
// SPDX-License-Identifier: MIT

#include "freertos_event_flags.hpp"
#include "freertos_os_helpers.hpp"
#include <FreeRTOS.h>
#include <cassert>
#include <event_groups.h>

#if configSUPPORT_DYNAMIC_ALLOCATION == 0
#include <etl/pool.h>

#ifndef FREERTOS_EVENT_GROUP_POOL_COUNT
#define FREERTOS_EVENT_GROUP_POOL_COUNT 1
#endif

#if FREERTOS_EVENT_GROUP_POOL_COUNT
etl::pool<StaticEventGroup_t, FREERTOS_EVENT_GROUP_POOL_COUNT> static_event_pool_;
#endif
#endif

namespace
{
#if configUSE_16_BIT_TICKS == 1
constexpr embvm::eventflag::flag_t MAX_SUPPORTED_BITS = (1 << 9);
#else
constexpr embvm::eventflag::flag_t MAX_SUPPORTED_BITS = (1 << 24);
#endif
}; // namespace

using namespace os::freertos;

EventFlag::~EventFlag() noexcept
{
	vEventGroupDelete(reinterpret_cast<EventGroupHandle_t>(handle_));

#if configSUPPORT_DYNAMIC_ALLOCATION == 0
	static_event_pool_.release(reinterpret_cast<StaticEventGroup_t*>(handle_));
#endif
}

EventFlag::EventFlag() noexcept
{
#if configSUPPORT_DYNAMIC_ALLOCATION
	// cppcheck-suppress useInitializationList
	handle_ = reinterpret_cast<embvm::eventflag::handle_t>(xEventGroupCreate());
	assert(handle_);
#else
	auto buf = static_event_pool_.allocate<StaticEventGroup_t>();
	// cppcheck-suppress useInitializationList
	handle_ = reinterpret_cast<embvm::eventflag::handle_t>(xEventGroupCreateStatic(buf));
#endif
}

embvm::eventflag::flag_t EventFlag::get(embvm::eventflag::flag_t bits_wait,
										embvm::eventflag::option opt, bool clearOnExit,
										const embvm::os_timeout_t& timeout) noexcept
{
	bool wait_for_all_bits = opt == embvm::eventflag::option::AND;
	TickType_t timeout_converted = frameworkTimeoutToTicks(timeout);

	assert(bits_wait < MAX_SUPPORTED_BITS);

	auto set_bits = xEventGroupWaitBits(reinterpret_cast<EventGroupHandle_t>(handle_), bits_wait,
										clearOnExit, wait_for_all_bits, timeout_converted);

	return static_cast<embvm::eventflag::flag_t>(set_bits);
}

void EventFlag::set(embvm::eventflag::flag_t bits) noexcept
{
	assert(bits < MAX_SUPPORTED_BITS);

	xEventGroupSetBits(reinterpret_cast<EventGroupHandle_t>(handle_), bits);
}

void EventFlag::setFromISR(embvm::eventflag::flag_t bits) noexcept
{
	assert(bits < MAX_SUPPORTED_BITS);

	BaseType_t higher_priority_task_woken;
	auto r = xEventGroupSetBitsFromISR(reinterpret_cast<EventGroupHandle_t>(handle_), bits,
									   &higher_priority_task_woken);
	assert(r == pdTRUE);

	portYIELD_FROM_ISR(higher_priority_task_woken);
}

void EventFlag::clear() noexcept
{
	xEventGroupClearBits(reinterpret_cast<EventGroupHandle_t>(handle_), MAX_SUPPORTED_BITS - 1);
}
