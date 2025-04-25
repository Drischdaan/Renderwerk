#include "pch.hpp"

#include "Renderwerk/Graphics/GfxCommon.hpp"

#include "Renderwerk/Graphics/GfxAdapter.hpp"
#include "Renderwerk/Graphics/GfxContext.hpp"
#include "Renderwerk/Graphics/GfxDevice.hpp"

IGfxContextChild::IGfxContextChild(FGfxContext* InGfxContext)
	: GfxContext(InGfxContext)
{
}

IGfxContextChild::IGfxContextChild(FGfxContext* InGfxContext, const FStringView& InDebugName)
	: IGfxObject(InDebugName), GfxContext(InGfxContext)
{
}

FNativeObject IGfxContextChild::GetRawNativeObject(const FNativeObjectId NativeObjectId)
{
	if (const FNativeObject NativeObject = GfxContext->GetRawNativeObject(NativeObjectId))
	{
		return NativeObject;
	}
	return IGfxObject::GetRawNativeObject(NativeObjectId);
}

IGfxAdapterChild::IGfxAdapterChild(FGfxAdapter* InGfxAdapter)
	: GfxAdapter(InGfxAdapter)
{
}

IGfxAdapterChild::IGfxAdapterChild(FGfxAdapter* InGfxAdapter, const FStringView& InDebugName)
	: IGfxObject(InDebugName), GfxAdapter(InGfxAdapter)
{
}

FNativeObject IGfxAdapterChild::GetRawNativeObject(const FNativeObjectId NativeObjectId)
{
	if (const FNativeObject NativeObject = GfxAdapter->GetRawNativeObject(NativeObjectId))
	{
		return NativeObject;
	}
	return IGfxObject::GetRawNativeObject(NativeObjectId);
}

IGfxDeviceChild::IGfxDeviceChild(FGfxDevice* InGfxDevice)
	: GfxDevice(InGfxDevice)
{
}

IGfxDeviceChild::IGfxDeviceChild(FGfxDevice* InGfxDevice, const FStringView& InDebugName)
	: IGfxObject(InDebugName), GfxDevice(InGfxDevice)
{
}

FNativeObject IGfxDeviceChild::GetRawNativeObject(const FNativeObjectId NativeObjectId)
{
	if (const FNativeObject NativeObject = GfxDevice->GetRawNativeObject(NativeObjectId))
	{
		return NativeObject;
	}
	return IGfxObject::GetRawNativeObject(NativeObjectId);
}
