// Copyright 2020 Embedded Artistry LLC
// SPDX-License-Identifier: MIT

#include "freertos_thread.hpp"
#include "freertos_os_helpers.hpp"
#include <FreeRTOS.h>
#include <cassert>
#include <task.h>

#if configSUPPORT_STATIC_ALLOCATION
#include <etl/pool.h>

#ifndef FREERTOS_THREAD_POOL_COUNT
#define FREERTOS_THREAD_POOL_COUNT 1
#endif

#if FREERTOS_THREAD_POOL_COUNT
etl::pool<StaticTask_t, FREERTOS_THREAD_POOL_COUNT> static_thread_pool_;
#endif
#endif

using namespace os::freertos;

static volatile thread_exit_t exit_func_ = nullptr;

// TODO: do we want to static assert and assume configUSE_TIME_SLICING?

void os::freertos::register_threadexit_func(thread_exit_t atexit) noexcept
{
	assert(exit_func_ == nullptr); // We only support one func at this time, and it's for libcpp
	exit_func_ = atexit;
}

#pragma mark - Definitions -

#pragma mark - Helpers -

// In FreeRTOS, low priority numbers represent low priority tasks.
// priority 0 < priority 10
static inline UBaseType_t freertos_priority(embvm::thread::priority p) noexcept
{
	switch(p)
	{
		case embvm::thread::priority::panic:
			return freertos_port_priorities::panic;
		case embvm::thread::priority::interrupt:
			return freertos_port_priorities::interrupt;
		case embvm::thread::priority::realtime:
			return freertos_port_priorities::realtime;
		case embvm::thread::priority::veryHigh:
			return freertos_port_priorities::veryHigh;
		case embvm::thread::priority::high:
			return freertos_port_priorities::high;
		case embvm::thread::priority::aboveNormal:
			return freertos_port_priorities::aboveNormal;
		case embvm::thread::priority::normal:
			return freertos_port_priorities::normal;
		case embvm::thread::priority::belowNormal:
			return freertos_port_priorities::belowNormal;
		case embvm::thread::priority::low:
			return freertos_port_priorities::low;
		case embvm::thread::priority::lowest:
			return freertos_port_priorities::lowest;
		case embvm::thread::priority::idle:
			return tskIDLE_PRIORITY;
		default:
			assert(0 && "Invalid priority value");
			return freertos_port_priorities::normal;
	}
}

#pragma mark - Thread Class Implementation -

Thread::Thread(std::string_view name, embvm::thread::func_t func, embvm::thread::input_t arg,
			   embvm::thread::priority p, size_t stack_size, void* stack_ptr) noexcept
{
	// This variable is read, but the #if confuses cppcheck
	// cppcheck-suppress unreadVariable
	auto converted_priority = freertos_priority(p);

	// FreeRTOS stack size is in "words", not bytes
	// cppcheck-suppress unreadVariable
	auto adjusted_stack_size = stack_size >> 2;

	if(stack_ptr)
	{
#if configSUPPORT_STATIC_ALLOCATION
		handle_ =
			reinterpret_cast<embvm::thread::handle_t>(static_thread_pool_.allocate<StaticTask_t>());
		auto r = xTaskCreateStatic(
			func, name.data(), static_cast<uint16_t>(adjusted_stack_size), arg, converted_priority,
			reinterpret_cast<StackType_t*>(stack_ptr), reinterpret_cast<StaticTask_t*>(handle_));
		assert(r);
		assert(r == handle_); // TODO: is r == handle_? Or do I need to store the thread pool return
							  // as a separate variable?

		static_ = true;
#else
		// You cannot provide the stack pointer if static allocation support is not enabled
		// in the FreeRTOS configuration.
		assert(0);
#endif
	}
	else
	{
#if configSUPPORT_DYNAMIC_ALLOCATION
		auto r = xTaskCreate(func, name.data(), static_cast<uint16_t>(adjusted_stack_size), arg,
							 converted_priority, reinterpret_cast<TaskHandle_t*>(&handle_));
		assert(r == pdPASS);
#else
		// You must provide a stack pointer if dynamic allocation support is not enabled in
		// the FreeRTOS configuration.
		assert(0);
#endif
	}
}

Thread::~Thread() noexcept
{
	terminate();
}

void Thread::start() noexcept
{
	// TODO
}

void Thread::terminate() noexcept
{
	/// Grab our TLS handle before we destroy the thread
	auto pdata = pvTaskGetThreadLocalStoragePointer(NULL, 0);

	if(handle_)
	{
		vTaskDelete(reinterpret_cast<TaskHandle_t>(handle_));

#if configSUPPORT_STATIC_ALLOCATION
		if(static_)
		{
			static_thread_pool_.release(reinterpret_cast<StaticTask_t*>(handle_));
		}
#endif

		// This block is used for libcpp support. If there is TLS data and a registered exit func,
		// we need to destroy libcpp's data accordingly.
		if(exit_func_ && pdata)
		{
			exit_func_(pdata);
		}

		handle_ = 0;
	}
}

std::string_view Thread::name() const noexcept
{
	return pcTaskGetName(reinterpret_cast<TaskHandle_t>(handle_));
}

embvm::thread::state Thread::state() const noexcept
{
	embvm::thread::state s;

	if(handle_)
	{
		auto state = eTaskGetState(reinterpret_cast<TaskHandle_t>(handle_));
		switch(state)
		{
			case eReady:
				s = embvm::thread::state::ready;
				break;
			case eRunning:
				s = embvm::thread::state::executing;
				break;
			case eBlocked:
				s = embvm::thread::state::suspended;
				break;
			case eSuspended:
				s = embvm::thread::state::suspended;
				break;
			case eDeleted:
			case eInvalid:
			default:
				s = embvm::thread::state::completed;
				break;
		}
	}
	else
	{
		s = embvm::thread::state::completed;
	}

	return s;
}

void Thread::join() noexcept
{
	if(handle_)
	{
		while(state() != embvm::thread::state::completed &&
			  state() != embvm::thread::state::terminated)
		{
			vTaskDelay(10);
		}
	}
}

void Thread::delay_for(uint32_t ticks) noexcept
{
	vTaskDelay(ticks);
}

#pragma mark - this_thread implementations -

void embvm::this_thread::sleep_for(const embvm::os_timeout_t& delay) noexcept
{
	auto ticks = frameworkTimeoutToTicks(delay);
	Thread::delay_for(ticks);
}

embvm::thread::handle_t embvm::this_thread::get_handle() noexcept
{
	return reinterpret_cast<embvm::thread::handle_t>(xTaskGetCurrentTaskHandle());
}

void embvm::this_thread::yield() noexcept
{
	Thread::delay_for(1);
}
