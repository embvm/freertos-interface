#include "freertos_mutex.hpp"
#include <FreeRTOS.h>
#include <semphr.h>

#if configSUPPORT_DYNAMIC_ALLOCATION == 0
#include <etl/pool.h>

#ifndef FREERTOS_MUTEX_POOL_COUNT
#define FREERTOS_MUTEX_POOL_COUNT 1
#endif

#if FREERTOS_MUTEX_POOL_COUNT
etl::pool<StaticSemaphore_t, FREERTOS_MUTEX_POOL_COUNT> static_mutex_pool_;
#endif
#endif

using namespace os::freertos;

namespace
{
embvm::mutex::handle_t createMutex() noexcept
{
#if configSUPPORT_DYNAMIC_ALLOCATION
	return reinterpret_cast<embvm::mutex::handle_t>(xSemaphoreCreateMutex());
#elif configSUPPORT_STATIC_ALLOCATION
	auto buf = static_mutex_pool_.allocate<StaticSemaphore_t>();
	return reinterpret_castembvm::mutex::handle_t > (xSemaphoreCreateMutexStatic(buf))
#endif
}

embvm::mutex::handle_t createRecursiveMutex() noexcept
{
#if configSUPPORT_DYNAMIC_ALLOCATION
	return reinterpret_cast<embvm::mutex::handle_t>(xSemaphoreCreateRecursiveMutex());
#elif configSUPPORT_STATIC_ALLOCATION
	auto buf = static_mutex_pool_.allocate<StaticSemaphore_t>();
	return reinterpret_cast<embvm::mutex::handle_t>(xSemaphoreCreateRecursiveMutexStatic(buf));
#endif
}
}; // namespace

using namespace os::freertos;

Mutex::~Mutex() noexcept
{
	vSemaphoreDelete(reinterpret_cast<SemaphoreHandle_t>(handle_));

#if configSUPPORT_DYNAMIC_ALLOCATION == 0
	static_mutex_pool_.release(reinterpret_cast<StaticSemaphore_t*>(handle_));
#endif
}

Mutex::Mutex(embvm::mutex::type type, [[maybe_unused]] embvm::mutex::mode mode) noexcept
	: type_(type)
{
	switch(type)
	{
		case embvm::mutex::type::recursive:
			handle_ = createRecursiveMutex();
			break;
		case embvm::mutex::type::normal:
			handle_ = createMutex();
			break;
	}
}

void Mutex::lock() noexcept
{
	if(type_ == embvm::mutex::type::recursive)
	{
		auto r =
			xSemaphoreTakeRecursive(reinterpret_cast<SemaphoreHandle_t>(handle_), portMAX_DELAY);
		assert(r == pdTRUE);
	}
	else
	{
		auto r = xSemaphoreTake(reinterpret_cast<SemaphoreHandle_t>(handle_), portMAX_DELAY);
		assert(r == pdTRUE);
	}
}

void Mutex::unlock() noexcept
{
	if(type_ == embvm::mutex::type::recursive)
	{
		xSemaphoreGiveRecursive(reinterpret_cast<SemaphoreHandle_t>(handle_));
	}
	else
	{
		xSemaphoreGive(reinterpret_cast<SemaphoreHandle_t>(handle_));
	}
}

bool Mutex::trylock() noexcept
{
	BaseType_t r;

	if(type_ == embvm::mutex::type::recursive)
	{
		r = xSemaphoreTakeRecursive(reinterpret_cast<SemaphoreHandle_t>(handle_), 0);
	}
	else
	{
		r = xSemaphoreTake(reinterpret_cast<SemaphoreHandle_t>(handle_), 0);
	}

	return r == pdTRUE;
}
