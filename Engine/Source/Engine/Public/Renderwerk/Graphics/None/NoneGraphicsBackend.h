#pragma once

#include "Renderwerk/Graphics/GraphicsBackend.h"

class ENGINE_API FNoneGraphicsBackend : public IGraphicsBackend
{
public:
	FNoneGraphicsBackend();
	~FNoneGraphicsBackend() override;

	DELETE_COPY_AND_MOVE(FNoneGraphicsBackend);

public:
	void Initialize(const FGraphicsBackendDesc& InDescription) override;
	void Destroy() override;

	[[nodiscard]] TVector<TSharedPtr<IGraphicsAdapter>> GetAvailableAdapters() override;

private:
};
