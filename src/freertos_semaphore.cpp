#include "freertos_semaphore.hpp"
#include "FreeRTOS.h"
#include "semphr.h"
#include <etl/pool.h>

#if configSUPPORT_DYNAMIC_ALLOCATION == 0
#include <etl/pool.h>

#ifndef FREERTOS_SEMAPHORE_POOL_COUNT
#define FREERTOS_SEMAPHORE_POOL_COUNT 1
#endif

#if FREERTOS_SEMAPHORE_POOL_COUNT
etl::pool<StaticSemaphore_t, FREERTOS_SEMAPHORE_POOL_COUNT> static_sem_pool_;
#endif
#endif

using namespace os::freertos;

namespace
{
embvm::semaphore::handle_t createBinarySemaphore() noexcept
{
#if configSUPPORT_DYNAMIC_ALLOCATION
	return reinterpret_cast<embvm::semaphore::handle_t>(xSemaphoreCreateBinary());
#elif configSUPPORT_STATIC_ALLOCATION
	auto buf = static_sem_pool_.allocate<StaticSemaphore_t>();
	return reinterpret_cast<embvm::semaphore::handle_t>(xSemaphoreCreateBinaryStatic(buf))
#endif
}

embvm::semaphore::handle_t createCountingSemaphore(embvm::semaphore::count_t ceiling,
												   embvm::semaphore::count_t initial_count) noexcept
{
#if configSUPPORT_DYNAMIC_ALLOCATION
	return reinterpret_cast<embvm::semaphore::handle_t>(
		xSemaphoreCreateCounting(ceiling, initial_count));
#elif configSUPPORT_STATIC_ALLOCATION
	auto buf = static_sem_pool_.allocate<StaticSemaphore_t>();
	return reinterpret_cast<embvm::semaphore::handle_t>(
		xSemaphoreCreateCountingStatic(ceiling, initial_count, buf));
#endif
}
}; // namespace

Semaphore::~Semaphore() noexcept
{
	vSemaphoreDelete(reinterpret_cast<SemaphoreHandle_t>(handle_));

#if configSUPPORT_DYNAMIC_ALLOCATION == 0
	static_sem_pool_.release(reinterpret_cast<StaticSemaphore_t*>(handle_));
#endif
}

Semaphore::Semaphore(embvm::semaphore::mode mode, embvm::semaphore::count_t ceiling,
					 embvm::semaphore::count_t initial_count) noexcept
{
	if(initial_count == -1)
	{
		initial_count = ceiling;
	}

	switch(mode)
	{
		case embvm::semaphore::mode::defaultMode:
		case embvm::semaphore::mode::counting:
			handle_ = createCountingSemaphore(ceiling, initial_count);
			assert(handle_);
			break;
		case embvm::semaphore::mode::binary:
			handle_ = createBinarySemaphore();
			assert(handle_);

			if(initial_count >= 1)
			{
				// FreeRTOS binary semaphores start in an empty state and must be given before
				// they can be taken.
				give();
			}

			break;
	}
}

void Semaphore::give() noexcept
{
	auto r = xSemaphoreGive(reinterpret_cast<SemaphoreHandle_t>(handle_));
	assert(r == pdTRUE);
}

void Semaphore::giveFromISR() noexcept
{
	BaseType_t higher_priority_task_woken;
	xSemaphoreGiveFromISR(reinterpret_cast<SemaphoreHandle_t>(handle_),
						  &higher_priority_task_woken);

	portYIELD_FROM_ISR(higher_priority_task_woken);
}

bool Semaphore::take(const embvm::os_timeout_t& timeout) noexcept
{
	// TODO: convert count properly to ms using
	auto r = xSemaphoreTake(reinterpret_cast<SemaphoreHandle_t>(handle_),
							std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());

	return r == pdTRUE;
}

embvm::semaphore::count_t Semaphore::count() const noexcept
{
	return static_cast<embvm::semaphore::count_t>(
		uxSemaphoreGetCount(reinterpret_cast<SemaphoreHandle_t>(handle_)));
}
