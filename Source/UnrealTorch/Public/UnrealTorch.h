// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"

#include <string>
#include <vector>


DECLARE_LOG_CATEGORY_EXTERN( LogUnrealTorch, Log, All );


class FUnrealTorchModule : public IModuleInterface
{
public:

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

	/** Torch dlls
	 * 
	 * We cannot move this into the Torch module, because including it into the .uplugin file would cause UBT to try to
	 * (re)compile also all C files from Torch itself. */
	static std::vector<ThirdPartyDll> thirdPartyDlls;
};
