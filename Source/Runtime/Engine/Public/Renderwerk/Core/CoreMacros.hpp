#pragma once


#define NON_COPYABLE_PREFIX(Class, Prefix) \
	Prefix Class(const Class&) = delete; \
	Prefix Class& operator=(const Class&) = delete;

#define NON_COPYABLE(Class) \
	NON_COPYABLE_PREFIX(Class, )

#define NON_MOVEABLE_PREFIX(Class, Prefix) \
	Prefix Class(Class&&) = delete; \
	Prefix Class& operator=(Class&&) = delete;

#define NON_MOVEABLE(Class) \
	NON_MOVEABLE_PREFIX(Class, )

#define NON_COPY_MOVEABLE(Class) \
	NON_COPYABLE(Class) \
	NON_MOVEABLE(Class)

#define NON_COPY_MOVEABLE_PREFIX(Class, Prefix) \
	NON_COPYABLE_PREFIX(Class, Prefix) \
	NON_MOVEABLE_PREFIX(Class, Prefix)

#define DEFAULT_COPYABLE_PREFIX(Class, Prefix) \
	Prefix Class(const Class&) noexcept = default; \
	Prefix Class& operator=(const Class&) = default;

#define DEFAULT_COPYABLE(Class) \
	DEFAULT_COPYABLE_PREFIX(Class, )

#define DEFAULT_MOVEABLE_PREFIX(Class, Prefix) \
	Prefix Class(Class&&) noexcept = default; \
	Prefix Class& operator=(Class&&) = default;

#define DEFAULT_MOVEABLE(Class) \
	DEFAULT_MOVEABLE_PREFIX(Class, )

#define DEFAULT_COPY_MOVEABLE(Class) \
	DEFAULT_COPYABLE(Class) \
	DEFAULT_MOVEABLE(Class)

#define DEFAULT_COPY_MOVEABLE_PREFIX(Class, Prefix) \
	DEFAULT_COPYABLE_PREFIX(Class, Prefix) \
	DEFAULT_MOVEABLE_PREFIX(Class, Prefix)

#define INNER_STRINGIFY(Value) #Value
#define STRINGIFY(Value) INNER_STRINGIFY(Value)

#define INNER_APPEND(X, Y) X##Y
#define APPEND(X, Y) INNER_APPEND(X, Y)

#define UNIQUE_VARIABLE(Prefix) APPEND(Prefix, __LINE__)

#define FORWARD(...) __VA_ARGS__
