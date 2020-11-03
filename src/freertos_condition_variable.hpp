// Copyright 2020 Embedded Artistry LLC
// SPDX-License-Identifier: MIT

#ifndef FREERTOS_CONDITION_VARIABLE_HPP_
#define FREERTOS_CONDITION_VARIABLE_HPP_

#include "freertos_semaphore.hpp"
#include <ctime>
#include <etl/queue.h>
#include <queue>
#include <rtos/condition_variable.hpp>
#include <rtos/rtos_defs.hpp>
// TODO: #include <platform_os_options.hpp>

// If you set this to 0, dynamic allocation will be used
#ifndef FREERTOS_NUM_THREADS_PER_CV
#define FREERTOS_NUM_THREADS_PER_CV 0
#endif

/** FreeRTOS Condition Variable Implementation
 *
 * FreeRTOS does not provide a CV primitive, so we need to implement the logic using
 * a function queue and a semaphore.
 *
 * The approach used below is taken from this article by Microsoft Research:
 *	https://www.microsoft.com/en-us/research/wp-content/uploads/2004/12/ImplementingCVs.pdf
 *
 */
namespace os::freertos
{
class ConditionVariable final : public embvm::VirtualConditionVariable
{
	using TQueueType = typename std::conditional<
		(FREERTOS_NUM_THREADS_PER_CV == 0), std::queue<embvm::thread::handle_t>,
		etl::queue<embvm::thread::handle_t, FREERTOS_NUM_THREADS_PER_CV>>::type;

  public:
	ConditionVariable() = default;
	~ConditionVariable() noexcept;

	bool wait(embvm::VirtualMutex* mutex) noexcept final;
	bool wait(embvm::VirtualMutex* mutex, const embvm::os_timeout_t& timeout) noexcept final;
	bool wait(embvm::VirtualMutex* mutex, const timespec& timeout) noexcept;

	void signal() noexcept final;
	void broadcast() noexcept final;

	embvm::cv::handle_t native_handle() const noexcept final
	{
		return sem_.native_handle();
	}

  private:
	bool freertos_wait(embvm::VirtualMutex* mutex, uint32_t ticks_timeout) noexcept;
	void pop_and_notify() noexcept;

  private:
	os::freertos::Semaphore sem_{embvm::semaphore::mode::binary};
	TQueueType q_;
};

} // namespace os::freertos

#endif // FREERTOS_CONDITION_VARIABLE_HPP_
