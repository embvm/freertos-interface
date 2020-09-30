# FreeRTOS Meson Build Definition

# TODO: Move dependency to top-level file?

# TODO: can we hide the include folder somehow, so we don't need to expose it to the whole program?
freertos_embvm_dep = declare_dependency(
	include_directories: [
		include_directories('.'),
	],
	sources: files(
		'freertos_condition_variable.cpp',
		'freertos_event_flags.cpp',
		'freertos_msg_queue.cpp',
		'freertos_mutex.cpp',
		'freertos_semaphore.cpp',
		'freertos_thread.cpp',
		'libcpp_threading.cpp',
		'os.cpp',
	),
	dependencies: [
		freertos_kernel_dep,
	]
)

#TODO: Enable
#freertos_heap_dep = declare_dependency(
#	sources: files('heap.cpp'),
#	dependencies: [
#		libmemory_framework_rtos_dep,
#	]
#)
