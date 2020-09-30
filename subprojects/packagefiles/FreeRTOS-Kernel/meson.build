project('FreeRTOS-Kernel',
	['c'],
	version: '10.4.1',
	meson_version: '>=0.55.0'
)

freertos_include_dir = [
	include_directories('include', is_system: true)
]

# Dependency for the core FreeRTOS Kernel
freertos_kernel_dep = declare_dependency(
	sources: files(
		'croutine.c',
		'event_groups.c',
		'list.c',
		'queue.c',
		'stream_buffer.c',
		'tasks.c',
		'timers.c',
	),
	include_directories: freertos_include_dir
)

# the very simplest, does not permit memory to be freed
freertos_heap1_dep = declare_dependency(
	sources: files('portable/MemMang/heap_1.c')
)

# permits memory to be freed, but does not coalescence adjacent free blocks.
freertos_heap2_dep = declare_dependency(
	sources: files('portable/MemMang/heap_2.c')
)

# simply wraps the standard malloc() and free() for thread safety
freertos_heap3_dep = declare_dependency(
	sources: files('portable/MemMang/heap_3.c')
)

# coalescences adjacent free blocks to avoid fragmentation.
# Includes absolute address placement option
freertos_heap4_dep = declare_dependency(
	sources: files('portable/MemMang/heap_4.c')
)

# as per heap_4, with the ability to span the heap across multiple non-adjacent memory areas
freertos_heap5_dep = declare_dependency(
	sources: files('portable/MemMang/heap_5.c')
)
