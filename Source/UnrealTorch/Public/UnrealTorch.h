// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"

#include <string>
#include <vector>


class FUnrealTorchModule : public IModuleInterface
{
public:

	FUnrealTorchModule();

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:

	/** A single dll that we will load */
	struct ThirdPartyDll
	{
		std::string name;
		void * handle;
	};

	/** Torch dlls */
	std::vector<ThirdPartyDll> thirdPartyDlls;
};