// Copyright 2020 Embedded Artistry LLC
// SPDX-License-Identifier: MIT

#ifndef FREERTOS_THREAD_HPP_
#define FREERTOS_THREAD_HPP_

#include <rtos/thread.hpp>

// TODO: suspend support... we create threads and suspend them immediately
// TODO: visit this and see if we are missing any crucial support
// https://www.codeproject.com/Articles/1278513/Cplusplus11-FreeRTOS-GCC

namespace os::freertos
{
/// @addtogroup FreeRTOSOS
/// @{

using thread_exit_t = void (*)(void*);
void register_threadexit_func(thread_exit_t atexit) noexcept;

static inline constexpr size_t FREERTOS_STACK_MIN = (1 * 1024);

/** Create a FreeRTOS thread
 *
 *
 * Your FreeRTOS port must define task priorities for the framework. We recommend
 * that you keep this definition in FreeRTOSConfig.h.
 *
 * For example, with a max priority of 32:
 * @code
 * enum freertos_port_priorities
 * {
 *	panic = 31,
 *	interrupt = 31,
 *	realtime = 30,
 *	veryHigh = 25,
 *	high = 20,
 *	aboveNormal = 15,
 *	normal = 10,
 *	belowNormal = 5,
 *	low = 3,
 *	lowest = 1,
 *	idle = 0
 * };
 * @endcode
 */
class Thread final : public embvm::VirtualThread
{
  public:
	Thread() {}

	/** Construct a FreeRTOS thread
	 *
	 * @param name The name associated with the mutex.
	 *	@note A std::string input must remain valid for the lifetime of this object, since
	 * 	std::string_view is used to store the name.
	 * @param func The thread function to execute; can be any functor type.
	 * @param arg The thread's optional input argument. This value is passed to the thread
	 * 	when it is created.
	 * @param p The thread priority setting.
	 * @param stack_size The thread stack size.
	 * @param stack_ptr The thread stack pointer. If stack_ptr is nullptr, then memory
	 * 	will be allocated by the pthread library.
	 */
	explicit Thread(std::string_view name, embvm::thread::func_t func, embvm::thread::input_t arg,
					embvm::thread::priority p = embvm::thread::priority::normal,
					size_t stack_size = FREERTOS_STACK_MIN, void* stack_ptr = nullptr) noexcept;

	/// Default destructor, cleans up thread on deletion.
	~Thread() noexcept;

	// FreeRTOS Threads Auto-start
	void start() noexcept final;

	void terminate() noexcept final;

	void join() noexcept final;

	std::string_view name() const noexcept final;

	embvm::thread::state state() const noexcept final;

	embvm::thread::handle_t native_handle() const noexcept final
	{
		return handle_;
	}

	static void delay_for(uint32_t ticks) noexcept;

  private:
	void thread_wrapper(embvm::thread::input_t arg) noexcept;

  private:
	/// The FreeRTOS thread handle
	embvm::thread::handle_t handle_ = 0;
	// embvm::thread::func_t func_; // TODO: should this be moved to template param for
	// compile-time? embvm::thread::input_t arg_; // TODO: how to remove this?
	bool static_ = false;
};

/// @}

} // namespace os::freertos

#endif // FREERTOS_THREAD_HPP_
