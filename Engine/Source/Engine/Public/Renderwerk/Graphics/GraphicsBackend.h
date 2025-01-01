#pragma once

#include "Renderwerk/Graphics/GraphicsCommon.h"

struct ENGINE_API FGraphicsBackendDesc
{
	bool8 bEnableDebugging = false;
	bool8 bEnableGpuValidation = false;
};

/**
 * The graphics backend interface is responsible for creating and managing graphics API objects.
 */
class ENGINE_API IGraphicsBackend : public TGraphicsObject<IGraphicsBackend>
{
public:
	IGraphicsBackend(const EGraphicsBackendType InBackendType)
		: BackendType(InBackendType)
	{
	}

	virtual ~IGraphicsBackend() = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(IGraphicsBackend);

public:
	virtual void Initialize(const FGraphicsBackendDesc& InDescription) = 0;
	virtual void Destroy() = 0;

	[[nodiscard]] virtual TVector<TSharedPtr<IGraphicsAdapter>> GetAvailableAdapters() = 0;
	[[nodiscard]] virtual TSharedPtr<IGraphicsAdapter> GetAdapterByDeviceId(uint32 DeviceId);

public:
	[[nodiscard]] EGraphicsBackendType GetBackendType() const { return BackendType; }

public:
	[[nodiscard]] static TUniquePtr<IGraphicsBackend> Create(EGraphicsBackendType InBackendType);

protected:
	EGraphicsBackendType BackendType = EGraphicsBackendType::None;

	FGraphicsBackendDesc Description = {};
};
