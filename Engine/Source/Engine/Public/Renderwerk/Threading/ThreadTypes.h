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
using FUniqueLock = std::unique_lock<FMutex>;
using FScopedLock = std::lock_guard<FMutex>;

template <int64 TMaxValue>
using FCountingSemaphore = std::counting_semaphore<TMaxValue>;

using FBinarySemaphore = std::binary_semaphore;
using FThread = std::thread;
