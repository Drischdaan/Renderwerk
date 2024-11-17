#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <semaphore>
#include <thread>

template <typename T>
using TAtomic = std::atomic<T>;

using FConditionVariable = std::condition_variable;
using FMutex = std::mutex;
using FScopedLock = std::scoped_lock;
using FBinarySemaphore = std::binary_semaphore;

template <std::ptrdiff_t MaxValue = PTRDIFF_MAX>
using FSemaphore = std::counting_semaphore<MaxValue>;

using FThread = std::thread;
