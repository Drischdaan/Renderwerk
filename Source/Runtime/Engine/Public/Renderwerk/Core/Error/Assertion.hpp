#pragma once

#include "Renderwerk/Core/PrimitiveTypes.hpp"
#include "Renderwerk/Core/Error/ErrorHandler.hpp"
#include "Renderwerk/Core/Misc/StringUtilities.hpp"

#define RW_VERIFY(Condition) \
	do \
	{ \
		const bool8 Result = (Condition); \
		if (!Result) \
		{ \
			if(IsDebuggerPresent()) \
			{ \
				__debugbreak(); \
			} \
			FErrorHandler::Report(FErrorDetails(ERROR_ID_REPORT, CURRENT_CONTEXT())); \
		} \
	} while (false)

#define RW_VERIFY_MSG(Condition, Message, ...) \
	do \
	{ \
		const bool8 Result = (Condition); \
		if (!Result) \
		{ \
			if(IsDebuggerPresent()) \
			{ \
				__debugbreak(); \
			} \
			FErrorHandler::Report(FErrorDetails(ERROR_ID_REPORT, FStringUtilities::Format(TEXT(Message), __VA_ARGS__), CURRENT_CONTEXT())); \
		} \
	} while (false)

#define RW_VERIFY_ID(Id) \
	do \
	{ \
		if (Id < 0) \
		{ \
			if(IsDebuggerPresent()) \
			{ \
				__debugbreak(); \
			} \
			FErrorHandler::Report(FErrorDetails(Id, CURRENT_CONTEXT())); \
		} \
	} while (false)

#define RW_VERIFY_ID_MSG(Id, Message, ...) \
	do \
	{ \
		if (Id < 0) \
		{ \
			if(IsDebuggerPresent()) \
			{ \
				__debugbreak(); \
			} \
			FErrorHandler::Report(FErrorDetails(Id, FStringUtilities::Format(TEXT(Message), __VA_ARGS__), CURRENT_CONTEXT())); \
		} \
	} while (false)

#ifdef RW_ENABLE_ASSERTIONS
#	define RW_ASSERT(Condition) RW_VERIFY(Condition)
#	define RW_ASSERT_MSG(Condition, Message, ...) RW_VERIFY_MSG(Condition, Message, __VA_ARGS__)
#	define RW_ASSERT_ID(Id) RW_VERIFY_ID(Id)
#	define RW_ASSERT_ID_MSG(Id, Message, ...) RW_VERIFY_ID_MSG(Id, Message, __VA_ARGS__)
#else
#	define RW_ASSERT(Condition) do { [[maybe_unused]] const bool8 Result = (Condition); } while (false)
#	define RW_ASSERT_MSG(Condition, Message, ...) do { [[maybe_unused]] const bool8 Result = (Condition); } while (false)
#	define RW_ASSERT_ID(Id) do { [[maybe_unused]] const FErrorId ErrorId = (Id); } while (false)
#	define RW_ASSERT_ID_MSG(Id, Message, ...) do { [[maybe_unused]] const FErrorId ErrorId = (Id); } while (false)
#endif
