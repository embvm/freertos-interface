#ifndef FREERTOS_OS_HPP_
#define FREERTOS_OS_HPP_

#include "freertos_condition_variable.hpp"
#include "freertos_event_flags.hpp"
#include "freertos_msg_queue.hpp"
#include "freertos_mutex.hpp"
#include "freertos_semaphore.hpp"
#include "freertos_thread.hpp"
#include <rtos/rtos.hpp>

namespace os
{
namespace freertos
{
/// @addtogroup FreeRTOSOS
/// @{

/// Call this function to start the FreeRTOS scheduler. We recommend placing this in the platform
/// init() function.
void startScheduler() noexcept;

/// Implementation of the FreeRTOS OS Factory
/// For API documentation, see embvm::embvm::VirtualOSFactory
/// @related embvm::embvm::VirtualOSFactory
class freertosOSFactory_impl
{
  public:
	static embvm::VirtualConditionVariable* createConditionVariable_impl() noexcept;

	static embvm::VirtualThread* createThread_impl(std::string_view name, embvm::thread::func_t f,
												   embvm::thread::input_t input,
												   embvm::thread::priority p, size_t stack_size,
												   void* stack_ptr) noexcept;

	static embvm::VirtualMutex*
		createMutex_impl(embvm::mutex::type type = embvm::mutex::type::defaultType,
						 embvm::mutex::mode mode = embvm::mutex::mode::defaultMode) noexcept;

	static embvm::VirtualSemaphore*
		createSemaphore_impl(embvm::semaphore::mode mode, embvm::semaphore::count_t ceiling,
							 embvm::semaphore::count_t initial_count) noexcept;

	template<typename TType>
	static embvm::VirtualMessageQueue<TType>* createMessageQueue_impl(size_t queue_length) noexcept
	{
		return new freertos::MessageQueue<TType>(queue_length);
	}

	static embvm::VirtualEventFlag* createEventFlag_impl() noexcept;

	static void destroy_impl(embvm::VirtualConditionVariable* item) noexcept;
	static void destroy_impl(embvm::VirtualThread* item) noexcept;
	static void destroy_impl(embvm::VirtualMutex* item) noexcept;
	static void destroy_impl(embvm::VirtualSemaphore* item) noexcept;
	static void destroy_impl(embvm::VirtualEventFlag* item) noexcept;

  public:
	freertosOSFactory_impl() = default;
	~freertosOSFactory_impl() = default;
};

/// @}
} // namespace freertos

/// Convenience alias for the FreeRTOS OS Factory.
/// Use this type instead of the verbose embvm::VirtualOSFactory definition.
using Factory = embvm::VirtualOSFactory<os::freertos::freertosOSFactory_impl>;

} // namespace os

#endif // FREERTOS_OS_HPP_
