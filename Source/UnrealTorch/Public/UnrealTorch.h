// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"


#include <memory>


class WindowsTorchLoader;




DECLARE_LOG_CATEGORY_EXTERN( LogUnrealTorch, Log, All );




class FUnrealTorchModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:

	std::unique_ptr<WindowsTorchLoader> windowsTorchLoader;
};
