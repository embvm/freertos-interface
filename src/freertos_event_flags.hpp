#ifndef FREERTOS_EVENT_FLAGS_HPP_
#define FREERTOS_EVENT_FLAGS_HPP_

#include <rtos/event_flag.hpp>
#include <string_view>

namespace os::freertos
{
/** FreeRTOS Event Flag Implementation.
 *
 * @ingroup FreeRTOSOS
 */
class EventFlag final : public embvm::VirtualEventFlag
{
  public:
	/** Create an event flag group.
	 */
	explicit EventFlag() noexcept;

	/// Default destructor which cleans up the event flag group.
	~EventFlag() noexcept;

	embvm::eventflag::flag_t
		get(embvm::eventflag::flag_t bits_wait,
			embvm::eventflag::option opt = embvm::eventflag::option::OR, bool clearOnExit = true,
			const embvm::os_timeout_t& timeout = embvm::OS_WAIT_FOREVER) noexcept final;

	void set(embvm::eventflag::flag_t bits) noexcept final;

	void setFromISR(embvm::eventflag::flag_t bits) noexcept final;

	void clear() noexcept final;

	embvm::eventflag::handle_t native_handle() const noexcept final
	{
		return handle_;
	}

  private:
	embvm::eventflag::handle_t handle_;
};

} // namespace os::freertos

#endif // FREERTOS_EVENT_FLAGS_HPP_
