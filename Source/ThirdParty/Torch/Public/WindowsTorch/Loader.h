// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Paths.h"
#include "PlatformProcess.h"

#include <string>
#include <vector>
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




DECLARE_LOG_CATEGORY_EXTERN( LogWindowsTorch, Log, All );
DEFINE_LOG_CATEGORY( LogWindowsTorch );




class WindowsTorch
{

public:

	/** Does not load dlls. Call loadDlls() to do that. */
	WindowsTorch();

	/** Does unload all dlls, if not unloaded already. */
	virtual ~WindowsTorch();

	/** Load all Torch dlls.
	 *
	 * @return    True on success, false otherwise.
	 */
	bool loadDlls();

	/** Unload all Torch dlls. */
	void unloadDlls();


private:

	/** A single dll. */
	struct ThirdPartyDll
	{
		std::string name;
		void * handle;
	};

	/** Torch dlls */
	std::vector<ThirdPartyDll> thirdPartyDlls;

};








/* Implementation and data begin
 * 
 * We cannot have compiled files in this module, because that would require us to include this module into the .uplugin
 * file, which in turn would cause UBT to try to (re)compile the C files from Torch itself (Torch needs them there). */




bool WindowsTorch::loadDlls()
{
	// Get base directories
	std::string BaseDirPlugin = TCHAR_TO_UTF8( *IPluginManager::Get().FindPlugin( "UnrealTorch" )->GetBaseDir() );
	std::string BaseDirGameContent = TCHAR_TO_UTF8( *FPaths::GameContentDir() );
	std::string BaseDirGameLogs = TCHAR_TO_UTF8( *FPaths::GameLogDir() );


	// Load all third party dlls
	UE_LOG( LogWindowsTorch, Log, TEXT( "Loading Torch DLLs.." ) );
	for( auto & dll : thirdPartyDlls )
	{
		if( dll.handle ) continue;    // dll is already loaded?
		std::string dllPath = BaseDirPlugin + "/" + dll.name;
		dll.handle = !dllPath.empty() ? FPlatformProcess::GetDllHandle( UTF8_TO_TCHAR( dllPath.c_str() ) ) : nullptr;
	}

	// Did all dlls load?
	if( ranges::all_of( thirdPartyDlls, []( auto & dll ) { return dll.handle; } ) )
	{
		UE_LOG( LogWindowsTorch, Log, TEXT( "All Torch DLLs loaded successfully." ) );
		return true;
	}
	else
	{
		UE_LOG( LogWindowsTorch, Error, TEXT( "Error while loading Torch DLLs!" ) );
		return false;
	}

}




void WindowsTorch::unloadDlls()
{
	for( auto & dll : thirdPartyDlls )
	{
		FPlatformProcess::FreeDllHandle( dll.handle );
		dll.handle = nullptr;
	}

	UE_LOG( LogWindowsTorch, Log, TEXT( "Unloaded all Torch DLLs." ) );
}




WindowsTorch::WindowsTorch() :
	thirdPartyDlls
	{
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
	}
{}


WindowsTorch::~WindowsTorch()
{
	unloadDlls();
}
