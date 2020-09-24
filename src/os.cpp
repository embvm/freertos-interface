#include "os.hpp"
#include "freertos_os_helpers.hpp"
#include <FreeRTOS.h>
#include <etl/pool.h>
#include <task.h>
// TODO: #include <platform_os_settings.hpp>

// TODO: Size 0 should enable new/delete and ETL types should not be declared.

using namespace os::freertos;

#pragma mark - Definitions -

#ifndef OS_CV_POOL_SIZE
#define OS_CV_POOL_SIZE 4
#endif

#ifndef OS_THREAD_POOL_SIZE
#define OS_THREAD_POOL_SIZE 4
#endif

#ifndef OS_MUTEX_POOL_SIZE
#define OS_MUTEX_POOL_SIZE 4
#endif

#ifndef OS_SEMAPHORE_POOL_SIZE
#define OS_SEMAPHORE_POOL_SIZE 4
#endif

#ifndef OS_EVENT_FLAG_POOL_SIZE
#define OS_EVENT_FLAG_POOL_SIZE 4
#endif

#pragma mark - Static Memory Pools -

namespace
{
etl::pool<ConditionVariable, OS_CV_POOL_SIZE> cv_factory_;
etl::pool<Thread, OS_THREAD_POOL_SIZE> thread_factory_;
etl::pool<Mutex, OS_MUTEX_POOL_SIZE> mutex_factory_;
etl::pool<Semaphore, OS_SEMAPHORE_POOL_SIZE> semaphore_factory_;
etl::pool<EventFlag, OS_EVENT_FLAG_POOL_SIZE> event_factory_;
} // namespace

#pragma mark - FreeRTOS Handlers -

extern "C" void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char* pcTaskName)
{
	// TODO: assert(0), or print out register info
	while(1)
		;
}

#pragma mark - Factory Functions -

embvm::VirtualConditionVariable* freertosOSFactory_impl::createConditionVariable_impl() noexcept
{
	return cv_factory_.create();
}

embvm::VirtualThread* freertosOSFactory_impl::createThread_impl(
	std::string_view name, embvm::thread::func_t f, embvm::thread::input_t input,
	embvm::thread::priority p, size_t stack_size, void* stack_ptr) noexcept
{
	return thread_factory_.create(name, f, input, p, stack_size, stack_ptr);
}

embvm::VirtualMutex* freertosOSFactory_impl::createMutex_impl(embvm::mutex::type type,
															  embvm::mutex::mode mode) noexcept
{
	return mutex_factory_.create(type, mode);
}

embvm::VirtualSemaphore*
	freertosOSFactory_impl::createSemaphore_impl(embvm::semaphore::mode mode,
												 embvm::semaphore::count_t ceiling,
												 embvm::semaphore::count_t initial_count) noexcept
{
	return semaphore_factory_.create(mode, ceiling, initial_count);
}

embvm::VirtualEventFlag* freertosOSFactory_impl::createEventFlag_impl() noexcept
{
	return event_factory_.create();
}

void freertosOSFactory_impl::destroy_impl(embvm::VirtualConditionVariable* item) noexcept
{
	assert(item);
	cv_factory_.destroy(reinterpret_cast<ConditionVariable*>(item));
}

void freertosOSFactory_impl::destroy_impl(embvm::VirtualThread* item) noexcept
{
	assert(item);
	thread_factory_.destroy(reinterpret_cast<Thread*>(item));
}

void freertosOSFactory_impl::destroy_impl(embvm::VirtualMutex* item) noexcept
{
	assert(item);
	mutex_factory_.destroy(reinterpret_cast<Mutex*>(item));
}

void freertosOSFactory_impl::destroy_impl(embvm::VirtualSemaphore* item) noexcept
{
	assert(item);
	semaphore_factory_.destroy(reinterpret_cast<Semaphore*>(item));
}

void freertosOSFactory_impl::destroy_impl(embvm::VirtualEventFlag* item) noexcept
{
	assert(item);
	event_factory_.destroy(reinterpret_cast<EventFlag*>(item));
}

#pragma mark - Supporting Functions -

void os::freertos::startScheduler()
{
	vTaskStartScheduler();
}
