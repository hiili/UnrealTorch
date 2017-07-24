// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"
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

	/** The set of dlls that we will load */
	std::vector<ThirdPartyDll> thirdPartyDlls;
};