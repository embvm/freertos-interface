#include "heap.hpp"
#include <FreeRTOS.h>
#include <cstdint>
#include <cstdlib>
#include <task.h>

/**
 * NOTE: This FreeRTOS malloc implementation requires heap_5.c
 *
 * Please define the correct heap_region for your project.
 */

#pragma mark - Definitions

/**
 * Your application can define this macro to increase the number of heap regions
 */
#ifndef FREERTOS_HEAP_REGION_CNT
#define FREERTOS_HEAP_REGION_CNT 2
#endif

#pragma mark - Private Functions -

static int cmp_heap(const void* a, const void* b) noexcept
{
	const HeapRegion_t* ua = reinterpret_cast<const HeapRegion_t*>(a);
	const HeapRegion_t* ub = reinterpret_cast<const HeapRegion_t*>(b);

	return ((ua->pucStartAddress < ub->pucStartAddress)
				? -1
				: ((ua->pucStartAddress != ub->pucStartAddress)));
}

#pragma mark - Declarations -

/// Maximum number of heap regions that can be specified
static const uint8_t heap_region_max = FREERTOS_HEAP_REGION_CNT;

/// Current number of allocated heap regions
static volatile uint8_t heap_region_cnt = 0;

/**
 * FreeRTOS internal memory pool stucture when using heap_5.c
 *
 * The block with the lowest starting address should appear first in the array
 *
 * An additional block is allocated to serve as a NULL terminator
 */
static HeapRegion_t heap_regions[FREERTOS_HEAP_REGION_CNT + 1];

#pragma mark - Class Implementations -

void os::freertos::Heap::addBlock(void* addr, size_t size) noexcept
{
	assert((heap_region_cnt < heap_region_max) && "Too many heap regions!");

	// Increment the count early to claim a spot in case of multi-threads
	uint8_t cnt = heap_region_cnt++;

	if(cnt < heap_region_max)
	{
		// We have space - add it to the table
		heap_regions[cnt].pucStartAddress = static_cast<uint8_t*>(addr);
		heap_regions[cnt].xSizeInBytes = size;
	}
	else
	{
		// Decrement the count if we don't have space
		heap_region_cnt--;
	}
}

void os::freertos::Heap::init() noexcept
{
	assert((heap_region_cnt > 0));

	if(heap_region_cnt > 0)
	{
		// Sort the heap regions so addresses are in the correct order
		qsort(heap_regions, heap_region_cnt, sizeof(HeapRegion_t), cmp_heap);

		// Pass the array into vPortDefineHeapRegions() to enable malloc()
		vPortDefineHeapRegions(heap_regions);
	}
}

void* os::freertos::Heap::alloc(size_t size) noexcept
{
	void* ptr = nullptr;

	if(size > 0)
	{
		ptr = pvPortMalloc(size);
	}

	return ptr;
}

void os::freertos::Heap::free(void* addr) noexcept
{
	vPortFree(addr);
}
