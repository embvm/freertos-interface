#ifndef FREERTOS_OS_HELPERS_HPP_
#define FREERTOS_OS_HELPERS_HPP_

#include "FreeRTOS.h"
#include <rtos/rtos_defs.hpp>

namespace os::freertos
{
inline uint32_t frameworkTimeoutToTicks(const embvm::os_timeout_t& timeout) noexcept
{
	if(timeout == embvm::OS_WAIT_FOREVER)
	{
		return portMAX_DELAY;
	}
	else
	{
		// TODO: actual conversion to ticks... this is fake millisecond conversion
		return static_cast<uint32_t>(
			std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
	}
}

} // namespace os::freertos

#endif // FREERTOS_OS_HELPERS_HPP_
