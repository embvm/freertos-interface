// Copyright 2020 Embedded Artistry LLC
// SPDX-License-Identifier: MIT

#ifndef FREERTOS_MUTEX_HPP_
#define FREERTOS_MUTEX_HPP_

#include <cassert>
#include <cerrno>
#include <rtos/mutex.hpp>

namespace os::freertos
{
/** FreeRTOS Mutex Implementation.
 *
 * @ingroup FreeRTOSOS
 */
class Mutex final : public embvm::VirtualMutex
{
  public:
	/** Construct a FreeRTOS mutex
	 *
	 * @param type The mutex type to create (normal, recursive)
	 * @param mode The mutex poperating mode, which controls priority inheritance behaviors.
	 */
	explicit Mutex(embvm::mutex::type type = embvm::mutex::type::defaultType,
				   embvm::mutex::mode mode = embvm::mutex::mode::defaultMode) noexcept;

	/// Default destructor
	~Mutex() noexcept;

	void lock() noexcept final;
	void unlock() noexcept final;
	bool trylock() noexcept final;

	embvm::mutex::handle_t native_handle() const noexcept final
	{
		return handle_;
	}

  private:
	embvm::mutex::handle_t handle_;

	embvm::mutex::type type_;
};

} // namespace os::freertos

#endif // FREERTOS_MUTEX_HPP_
