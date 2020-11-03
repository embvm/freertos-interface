// Copyright 2020 Embedded Artistry LLC
// SPDX-License-Identifier: MIT

#include "freertos_msg_queue.hpp"
#include <FreeRTOS.h>
#include <queue.h>

#if configSUPPORT_DYNAMIC_ALLOCATION == 0
#error Static message queues are not supported at this time
#endif

using namespace os::freertos::details;

static inline TickType_t convert_timeout(embvm::os_timeout_t timeout) noexcept
{
	TickType_t timeout_converted;

	if(timeout == embvm::OS_WAIT_FOREVER)
	{
		timeout_converted = portMAX_DELAY;
	}
	else
	{
		// TODO: real time conversion
		timeout_converted = std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count();
	}

	return timeout_converted;
}

embvm::msgqueue::handle_t MessageQueueMediator::create(size_t length, size_t item_size) noexcept
{
	return reinterpret_cast<embvm::msgqueue::handle_t>(xQueueCreate(length, item_size));
}

bool MessageQueueMediator::full(embvm::msgqueue::handle_t handle, size_t max_length) noexcept
{
	return size(handle) == max_length;
}

bool MessageQueueMediator::empty(embvm::msgqueue::handle_t handle) noexcept
{
	return size(handle) == 0;
}

void MessageQueueMediator::reset(embvm::msgqueue::handle_t handle) noexcept
{
	xQueueReset(reinterpret_cast<QueueHandle_t>(handle));
}

size_t MessageQueueMediator::size(embvm::msgqueue::handle_t handle) noexcept
{
	return static_cast<size_t>(uxQueueMessagesWaiting(reinterpret_cast<QueueHandle_t>(handle)));
}

bool MessageQueueMediator::pop(embvm::msgqueue::handle_t handle, void* buffer,
							   embvm::os_timeout_t timeout) noexcept
{
	return pdTRUE ==
		   xQueueReceive(reinterpret_cast<QueueHandle_t>(handle), buffer, convert_timeout(timeout));
}

bool MessageQueueMediator::push(embvm::msgqueue::handle_t handle, const void* buffer,
								embvm::os_timeout_t timeout) noexcept
{
	return pdTRUE == xQueueSendToBack(reinterpret_cast<QueueHandle_t>(handle), buffer,
									  convert_timeout(timeout));
}
