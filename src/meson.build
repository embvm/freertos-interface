# FreeRTOS Meson Build Definition

freertos_include_dir = [
	include_directories('include', is_system: true)
]

freertos_sources = files(
	'src/croutine.c',
	'src/event_groups.c',
	'src/list.c',
	'src/queue.c',
	'src/stream_buffer.c',
	'src/tasks.c',
	'src/timers.c',
)

# the very simplest, does not permit memory to be freed
freertos_heap1_dep = files('src/portable/MemMang/heap_1.c')

# permits memory to be freed, but does not coalescence adjacent free blocks.
freertos_heap2_dep = files('src/portable/MemMang/heap_2.c')

# simply wraps the standard malloc() and free() for thread safety
freertos_heap3_dep = files('src/portable/MemMang/heap_3.c')

# coalescences adjacent free blocks to avoid fragmentation.
# Includes absolute address placement option
freertos_heap4_dep = files('src/portable/MemMang/heap_4.c')

# as per heap_4, with the ability to span the heap across multiple non-adjacent memory areas
freertos_heap5_dep = files('src/portable/MemMang/heap_5.c')

framework_freertos_sources = files(
	'freertos_condition_variable.cpp',
	'freertos_event_flags.cpp',
	'freertos_msg_queue.cpp',
	'freertos_mutex.cpp',
	'freertos_semaphore.cpp',
	'freertos_thread.cpp',
	'libcpp_threading.cpp',
	'os.cpp',
)

# TODO: can we hide the include folder somehow, so we don't need to expose it to the whole program?
# Note: for now we are always forcing heap 3, which wraps malloc/free
freertos_core_dep = declare_dependency(
	include_directories: [
		freertos_include_dir,
		include_directories('.'),
	],
	sources: [
		freertos_sources,
		framework_freertos_sources,
		freertos_heap3_dep
	],
	# TODO: clean up how libc/libcxx includes are handled
	dependencies: stdlib_host_dep
)

freertos_heap_dep = declare_dependency(
	sources: files('heap.cpp'),
	dependencies: libmemory_framework_rtos_dep,
)
