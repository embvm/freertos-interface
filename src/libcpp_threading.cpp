// Copyright 2020 Embedded Artistry LLC
// SPDX-License-Identifier: MIT

#include <FreeRTOS.h>
#include <__external_threading>
#include <errno.h>
#include <os.hpp>
#include <task.h>

static_assert(configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0,
			  "libcpp requires a FreeRTOS config that supports thread local storage. "
			  "Set the configNUM_THREAD_LOCAL_STORAGE_POINTERS parameter.");

#pragma mark - Recursive Mutex Functions -

int std::__libcpp_recursive_mutex_init(std::__libcpp_recursive_mutex_t* __m)
{
	*__m = os::Factory::createMutex(embvm::mutex::type::recursive);
	return 0;
}

int std::__libcpp_recursive_mutex_destroy(std::__libcpp_recursive_mutex_t* __m)
{
	os::Factory::destroy(*__m);
	return 0;
}

#pragma mark - Mutex Functions -

int std::__libcpp_mutex_init(std::__libcpp_recursive_mutex_t* __m)
{
	*__m = os::Factory::createMutex();
	return 0;
}

int std::__libcpp_mutex_destroy(std::__libcpp_mutex_t* __m)
{
	// There is the possibility that we are destroying our mutex without creating it,
	// since we have to call lock, unlock, or try_lock to actually initialize a std::mutex.
	// This is because the std::mutex constructor is constexpr... We don't know why.
	if(*__m)
	{
		os::Factory::destroy(*__m);
	}

	return 0;
}

#pragma mark - Thread Functions -

int std::__libcpp_thread_create(std::__libcpp_thread_t* __t, void* (*__func)(void*), void* __arg)
{
	*__t = os::Factory::createThread("std::thread", reinterpret_cast<embvm::thread::func_t>(__func),
									 __arg);
	return 0;
}

bool std::__libcpp_thread_isnull(const std::__libcpp_thread_t* __t)
{
	return *__t == nullptr;
}

int std::__libcpp_thread_destroy(std::__libcpp_thread_t* __t)
{
	if(__t && *__t)
	{
		os::Factory::destroy(*__t);
		*__t = nullptr;
	}

	return 0;
}

#pragma mark - Condition Variable -

int std::__libcpp_condvar_create(std::__libcpp_condvar_t* __cv)
{
	*__cv = os::Factory::createConditionVariable();
	return 0;
}

int std::__libcpp_condvar_timedwait(std::__libcpp_condvar_t* __cv, std::__libcpp_mutex_t* __m,
									timespec* __ts)
{
	if(!*__cv)
	{
		__libcpp_condvar_create(__cv);
	}

	bool success = reinterpret_cast<os::freertos::ConditionVariable*>(*__cv)->wait(*__m, *__ts);

	return success ? 0 : ETIMEDOUT;
}

int std::__libcpp_condvar_destroy(std::__libcpp_condvar_t* __cv)
{
	// There is the possibility that we are destroying our CV without creating it,
	// since we have to call a function to properly initialize it
	if(*__cv)
	{
		os::Factory::destroy(*__cv);
	}

	return 0;
}

#pragma mark - Thread Local Storage -

/// This function is called once, and the primary purpose is to register the destructor function
/// which will eventually free libcpp's thread data.
/// FreeRTOS can ignore the key.
int std::__libcpp_tls_create(std::__libcpp_tls_key* __key,
							 void(_LIBCPP_TLS_DESTRUCTOR_CC* __at_exit)(void*))
{
	(void)__key;
	os::freertos::register_threadexit_func(__at_exit);
	return 0;
}

// Only one value is supported: 0, so key is ignored
// This function updates the calling thread! Call from the thread you want to store data into.
void* std::__libcpp_tls_get(std::__libcpp_tls_key __key)
{
	(void)__key;
	return pvTaskGetThreadLocalStoragePointer(nullptr, 0);
}

// Only one value is supported: 0, so key is ignored
// This function updates the calling thread! Call from the thread you want to store data into.
int std::__libcpp_tls_set(std::__libcpp_tls_key __key, void* __p)
{
	(void)__key;
	vTaskSetThreadLocalStoragePointer(nullptr, 0, __p);
	return 0;
}

#pragma mark - Execute Once -

// Execute once
int std::__libcpp_execute_once(std::__libcpp_exec_once_flag* flag, void (*init_routine)(void))
{
	(void)flag;
	(void)init_routine;
	// return pthread_once(flag, init_routine);
	return -1;
}
