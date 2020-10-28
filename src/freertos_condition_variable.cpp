#include "freertos_condition_variable.hpp"
#include "freertos_os_helpers.hpp"
#include "rtos/thread.hpp"
#include <FreeRTOS.h>
#include <cassert>
#include <task.h>

using namespace os::freertos;

ConditionVariable::~ConditionVariable() noexcept
{
	assert(q_.empty()); // We can't destruct if threads have not been notified!
}

bool ConditionVariable::freertos_wait(embvm::VirtualMutex* mutex, uint32_t ticks_timeout) noexcept
{
	// We add our function to the waiting queue
	sem_.take();
	q_.push(embvm::this_thread::get_handle());
	sem_.give();

	// Tell FreeRTOS to sleep this thread until a notification occurs
	mutex->unlock();
	auto r = ulTaskNotifyTake(pdTRUE, ticks_timeout);
	mutex->lock();

	return r > 0; // 0 indicates we had no notifications
}

bool ConditionVariable::wait(embvm::VirtualMutex* mutex) noexcept
{
	return freertos_wait(mutex, portMAX_DELAY);
}

bool ConditionVariable::wait(embvm::VirtualMutex* mutex,
							 const embvm::os_timeout_t& timeout) noexcept
{
	auto timeout_ticks = frameworkTimeoutToTicks(timeout);
	return freertos_wait(mutex, timeout_ticks);
}

bool ConditionVariable::wait(embvm::VirtualMutex* mutex, const timespec& timeout) noexcept
{
	// TODO: improve this approach. it's stupid to go from duration->timespec->duration->uint32_t
	// Stupid standard implementation.
	auto timeout_ticks = frameworkTimeoutToTicks(embutil::timespecToDuration(timeout));
	return freertos_wait(mutex, timeout_ticks);
}

void ConditionVariable::signal() noexcept
{
	sem_.take();
	if(!q_.empty())
	{
		pop_and_notify();
	}
	sem_.give();
}

void ConditionVariable::broadcast() noexcept
{
	sem_.take();
	while(!q_.empty())
	{
		pop_and_notify();
	}
	sem_.give();
}

void ConditionVariable::pop_and_notify() noexcept
{
	embvm::thread::handle_t thread_handle = q_.front();
	q_.pop();
	xTaskNotifyGive((reinterpret_cast<TaskHandle_t>(thread_handle)));
}
