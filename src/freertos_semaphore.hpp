#ifndef FREERTOS_SEMAPHORE_HPP_
#define FREERTOS_SEMAPHORE_HPP_

#include <cassert>
#include <rtos/semaphore.hpp>
#include <string_view>

namespace os::freertos
{
/// @addtogroup FreeRTOSOS
/// @{

/** Create a FreeRTOS semaphore (OSX variant)
 *
 */
class Semaphore final : public embvm::VirtualSemaphore
{
  public:
	/** Create a FreeRTOS sempahore
	 *
	 * @param mode The semaphore mode (binary, counting).
	 * @param ceiling The maximum count of the semaphore
	 * @param initial_count The starting count of the semaphore. Can be used to indicate that
	 * resources are in use at hte itme of creation.
	 */
	explicit Semaphore(embvm::semaphore::mode mode = embvm::semaphore::mode::counting,
					   embvm::semaphore::count_t ceiling = 1,
					   embvm::semaphore::count_t initial_count = -1) noexcept;

	/// Default destructor, cleans up after semaphore on destruction.
	~Semaphore() noexcept;

	void give() noexcept final;
	void giveFromISR() noexcept final;
	bool take(const embvm::os_timeout_t& timeout = embvm::OS_WAIT_FOREVER) noexcept final;
	embvm::semaphore::count_t count() const noexcept final;

	embvm::semaphore::handle_t native_handle() const noexcept final
	{
		return handle_;
	}

  private:
	embvm::semaphore::handle_t handle_;
};

/// @}

} // namespace os::freertos

#endif // FREERTOS_SEMAPHORE_HPP_
