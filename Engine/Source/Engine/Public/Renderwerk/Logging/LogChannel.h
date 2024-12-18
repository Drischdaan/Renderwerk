#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

struct ENGINE_API ILogChannel
{
public:
	ILogChannel(const FAnsiString& InName)
		: Name(InName)
	{
	}

	virtual ~ILogChannel() = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(ILogChannel);

public:
	[[nodiscard]] FAnsiString GetName() const { return Name; }

private:
	FAnsiString Name;
};

#define DECLARE_LOG_CHANNEL(ChannelName) \
	struct ENGINE_API FLogChannel##ChannelName : public ILogChannel \
	{ \
		inline FLogChannel##ChannelName() : ILogChannel(#ChannelName) {} \
	}; \
	ENGINE_API extern FLogChannel##ChannelName ChannelName


#define DEFINE_LOG_CHANNEL(ChannelName) FLogChannel##ChannelName ChannelName
