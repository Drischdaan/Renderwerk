#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

struct ENGINE_API ILogChannel
{
public:
	constexpr ILogChannel(const FChar* Name);
	virtual ~ILogChannel() = default;

	DELETE_COPY_AND_MOVE(ILogChannel);

public:
	[[nodiscard]] constexpr const FChar* GetName() const { return Name; }

private:
	const FChar* Name;
};

template <>
struct std::hash<ILogChannel>
{
	size64 operator()(const ILogChannel& Channel) const noexcept
	{
		return std::hash<FString>()(Channel.GetName());
	}
};

#define DECLARE_LOG_CHANNEL(ChannelName) \
	struct ENGINE_API FLogChannel##ChannelName : public ILogChannel \
	{ \
		inline FLogChannel##ChannelName() : ILogChannel(#ChannelName) {} \
	}; \
	ENGINE_API extern FLogChannel##ChannelName ChannelName


#define DEFINE_LOG_CHANNEL(ChannelName) FLogChannel##ChannelName ChannelName
