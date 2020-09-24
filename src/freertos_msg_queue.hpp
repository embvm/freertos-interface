#ifndef FREERTOS_MSG_QUEUE_HPP_
#define FREERTOS_MSG_QUEUE_HPP_

#include <cassert>
#include <rtos/msg_queue.hpp>
#include <string>

namespace os::freertos
{
namespace details
{
class MessageQueueMediator
{
  public:
	static embvm::msgqueue::handle_t create(size_t length, size_t item_size);
	static void destroy(embvm::msgqueue::handle_t handle);
	static bool full(embvm::msgqueue::handle_t handle, size_t max_length);
	static bool empty(embvm::msgqueue::handle_t handle);
	static void reset(embvm::msgqueue::handle_t handle);
	static size_t size(embvm::msgqueue::handle_t handle);
	static bool pop(embvm::msgqueue::handle_t handle, void* buffer, embvm::os_timeout_t timeout);
	static bool push(embvm::msgqueue::handle_t handle, const void* buffer,
					 embvm::os_timeout_t timeout);
};
} // namespace details

/// @addtogroup FreeRTOSOS
/// @{

/** FreeRTOS Message Queue implementation for OSX
 *
 * @tparam TType The type of data to be stored in the message queue
 */
template<typename TType>
class MessageQueue final : public embvm::VirtualMessageQueue<TType>
{
  public:
	/** Construct a message queue
	 *
	 * @param queue_length The maximum size of the message queue.
	 */
	explicit MessageQueue(size_t queue_length) noexcept : max_length_(queue_length)
	{
		// cppcheck-suppress useInitializationList
		handle_ = details::MessageQueueMediator::create(queue_length, sizeof(TType));
		assert(handle_);
	}

	/// Default destructor, cleans up the message queue.
	~MessageQueue() noexcept
	{
		details::MessageQueueMediator::destroy(handle_);
	}

	bool push(TType val, embvm::os_timeout_t timeout = embvm::OS_WAIT_FOREVER) noexcept final
	{
		return details::MessageQueueMediator::push(handle_, &val, timeout);
	}

	std::optional<TType> pop(embvm::os_timeout_t timeout = embvm::OS_WAIT_FOREVER) noexcept final
	{
		TType val;
		auto recvd =
			details::MessageQueueMediator::pop(handle_, reinterpret_cast<void*>(&val), timeout);
		if(recvd)
		{
			return val;
		}
		else
		{
			return nullptr;
		}
	}

	size_t size() const noexcept final
	{
		return details::MessageQueueMediator::size(handle_);
	}

	void reset() noexcept final
	{
		details::MessageQueueMediator::reset(handle_);
	}

	bool empty() const noexcept final
	{
		return details::MessageQueueMediator::empty(handle_);
	}

	bool full() const noexcept final
	{
		return details::MessageQueueMediator::full(handle_, max_length_);
	}

	embvm::msgqueue::handle_t native_handle() const noexcept final
	{
		return handle_;
	}

  private:
	embvm::msgqueue::handle_t handle_;
	size_t max_length_;
};

/// @}

} // namespace os::freertos

#endif // FREERTOS_MSG_QUEUE_HPP_
