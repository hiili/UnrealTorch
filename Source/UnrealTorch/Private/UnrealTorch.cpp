// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "UnrealTorch.h"

#include "ModuleManager.h"
#include "IPluginManager.h"
#include "Paths.h"
#include "PlatformProcess.h"


#include <string>
#include <algorithm>


// include Range-v3
#pragma warning( push )
#pragma warning( disable: 4268 4668 )    // UE pollution.. let's hope that this does not break anything
#pragma push_macro("check")
#pragma push_macro("PI")
#undef check
#undef PI
#include <range/v3/all.hpp>
#pragma pop_macro("PI")
#pragma pop_macro("check")
#pragma warning( pop )


#define LOCTEXT_NAMESPACE "FUnrealTorchModule"




DEFINE_LOG_CATEGORY( LogUnrealTorch );




std::vector<FUnrealTorchModule::ThirdPartyDll> FUnrealTorchModule::thirdPartyDlls{
	// cd Source/ThirdParty/Torch/WindowsTorch
	// find bin/ -name '*.dll' | sed 's_\(.*\)_{ "Source/ThirdParty/Torch/WindowsTorch/\1", 0 },_'
	// wc -l  --> 23
	//
	// Note that order is important! Dependencies must be loaded before their dependents.

	// Lua
		{ "Source/ThirdParty/Torch/WindowsTorch/bin/libluajit.dll", 0 },
		{ "Source/ThirdParty/Torch/WindowsTorch/bin/lua51.dll", 0 },

		// MSYS2 for LAPACK
		{ "Source/ThirdParty/Torch/WindowsTorch/bin/libquadmath-0.dll", 0 },
		{ "Source/ThirdParty/Torch/WindowsTorch/bin/libgcc_s_seh-1.dll", 0 },
		{ "Source/ThirdParty/Torch/WindowsTorch/bin/libgfortran-3.dll", 0 },
		{ "Source/ThirdParty/Torch/WindowsTorch/bin/libwinpthread-1.dll", 0 },

		// LAPACK
		{ "Source/ThirdParty/Torch/WindowsTorch/bin/libblas.dll", 0 },
		{ "Source/ThirdParty/Torch/WindowsTorch/bin/liblapack.dll", 0 },

		// Torch
		{ "Source/ThirdParty/Torch/WindowsTorch/bin/TH.dll", 0 },
		{ "Source/ThirdParty/Torch/WindowsTorch/bin/luaT.dll", 0 },

		// the rest
		{ "Source/ThirdParty/Torch/WindowsTorch/bin/ffi.dll", 0 },
		{ "Source/ThirdParty/Torch/WindowsTorch/bin/lfs.dll", 0 },
		{ "Source/ThirdParty/Torch/WindowsTorch/bin/libimage.dll", 0 },
		{ "Source/ThirdParty/Torch/WindowsTorch/bin/libnnx.dll", 0 },
		{ "Source/ThirdParty/Torch/WindowsTorch/bin/libpaths.dll", 0 },
		{ "Source/ThirdParty/Torch/WindowsTorch/bin/libppm.dll", 0 },
		{ "Source/ThirdParty/Torch/WindowsTorch/bin/libsundown.dll", 0 },
		{ "Source/ThirdParty/Torch/WindowsTorch/bin/libsys.dll", 0 },
		{ "Source/ThirdParty/Torch/WindowsTorch/bin/libTHNN.dll", 0 },
		{ "Source/ThirdParty/Torch/WindowsTorch/bin/libtmglib.dll", 0 },
		{ "Source/ThirdParty/Torch/WindowsTorch/bin/libtorch.dll", 0 },
		{ "Source/ThirdParty/Torch/WindowsTorch/bin/mime/core.dll", 0 },
		{ "Source/ThirdParty/Torch/WindowsTorch/bin/socket/core.dll", 0 },
};




void FUnrealTorchModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get base directories
	std::string BaseDirPlugin = TCHAR_TO_UTF8( *IPluginManager::Get().FindPlugin( "UnrealTorch" )->GetBaseDir() );
	std::string BaseDirGameContent = TCHAR_TO_UTF8( *FPaths::GameContentDir() );
	std::string BaseDirGameLogs = TCHAR_TO_UTF8( *FPaths::GameLogDir() );


	// Load all third party dlls
	UE_LOG( LogUnrealTorch, Log, TEXT( "Loading Torch DLLs.." ) );
	for( auto & dll : thirdPartyDlls )
	{
		std::string dllPath = BaseDirPlugin + "/" + dll.name;
		dll.handle = !dllPath.empty() ? FPlatformProcess::GetDllHandle( UTF8_TO_TCHAR( dllPath.c_str() ) ) : nullptr;
	}

	// Did all dlls load?
	if( ranges::all_of( thirdPartyDlls, []( auto & dll ) { return dll.handle; } ) )
	{
		UE_LOG( LogUnrealTorch, Log, TEXT("All Torch DLLs loaded successfully.") );
	}
	else
	{
		UE_LOG( LogUnrealTorch, Error, TEXT( "Error while loading Torch DLLs!" ) );
	}
}




void FUnrealTorchModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Unload all dlls
	for( auto & dll : thirdPartyDlls )
	{
		FPlatformProcess::FreeDllHandle( dll.handle );
		dll.handle = nullptr;
	}
}




#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUnrealTorchModule, UnrealTorch)