// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

// include our header and UE stuff
// 
// UE pollutes the macro environment, but including these later does not help: UBT anyways adds statements to the VS
// response file to read in the UE stuff.
// 
// The thorough solution is described here: https://answers.unrealengine.com/questions/391017/constant-library-conflicts.html
//   "So essentially you have your unreal game, which includes an unreal plugin, which references an unreal friendly
//    wrapper library, which contains the third party code.... Yeah, it's garbage."
#include "UnrealTorch.h"
#include "ModuleManager.h"
#include "IPluginManager.h"
#include "MessageDialog.h"
#include "Paths.h"
#include "PlatformProcess.h"

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

// include Sol2
#pragma warning( push )
#pragma warning( disable: 4628 )    // UE pollution.. let's hope that this does not break anything
//#define SOL_EXCEPTIONS_SAFE_PROPAGATION   // Do we need this? At least seems to work without..
//#define SOL_NO_EXCEPTIONS   // LuaJIT and exceptions did not play together: https://sol2.readthedocs.io/en/latest/exceptions.html
                            // ..but do now; check the link above!
//#ifndef NDEBUG   // what macro to use with UE?
//#define SOL_CHECK_ARGUMENTS
//#endif
#pragma push_macro("check")
#undef check
#include "sol2/sol.hpp"
#pragma pop_macro("check")
#pragma warning( pop )

// include Torch
#include "TH/TH.h"


#include <string>
#include <algorithm>
#include <cstdint>


#define LOCTEXT_NAMESPACE "FUnrealTorchModule"




DECLARE_LOG_CATEGORY_EXTERN( LogUnrealTorch, Log, All );
DEFINE_LOG_CATEGORY( LogUnrealTorch );




DECLARE_LOG_CATEGORY_EXTERN( LogLua, Log, All );
DEFINE_LOG_CATEGORY( LogLua );

void UeLogProxy( ELogVerbosity::Type verbosity, const std::string & message )
{
	// need to copy-paste due to the nature of the UE_LOG macro definition
	if(			verbosity == ELogVerbosity::Fatal )			{ UE_LOG( LogLua, Fatal,		TEXT( "%s" ), UTF8_TO_TCHAR( message.c_str() ) ); }
	else if(	verbosity == ELogVerbosity::Error )			{ UE_LOG( LogLua, Error,		TEXT( "%s" ), UTF8_TO_TCHAR( message.c_str() ) ); }
	else if(	verbosity == ELogVerbosity::Warning )		{ UE_LOG( LogLua, Warning,		TEXT( "%s" ), UTF8_TO_TCHAR( message.c_str() ) ); }
	else if(	verbosity == ELogVerbosity::Display )		{ UE_LOG( LogLua, Display,		TEXT( "%s" ), UTF8_TO_TCHAR( message.c_str() ) ); }
	else if(	verbosity == ELogVerbosity::Log )			{ UE_LOG( LogLua, Log,			TEXT( "%s" ), UTF8_TO_TCHAR( message.c_str() ) ); }
	else if(	verbosity == ELogVerbosity::Verbose )		{ UE_LOG( LogLua, Verbose,		TEXT( "%s" ), UTF8_TO_TCHAR( message.c_str() ) ); }
	else if(	verbosity == ELogVerbosity::VeryVerbose )	{ UE_LOG( LogLua, VeryVerbose,	TEXT( "%s" ), UTF8_TO_TCHAR( message.c_str() ) ); }
	else checkf( false, TEXT( "(%s) Unknown verbosity level: %d" ), TEXT( __FUNCTION__ ), verbosity );
}




FUnrealTorchModule::FUnrealTorchModule() :
	thirdPartyDlls{ 
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




void FUnrealTorchModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get base directories
	std::string BaseDirPlugin      = TCHAR_TO_UTF8( *IPluginManager::Get().FindPlugin( "UnrealTorch" )->GetBaseDir() );
	std::string BaseDirGameContent = TCHAR_TO_UTF8( *FPaths::GameContentDir() );
	std::string BaseDirGameLogs    = TCHAR_TO_UTF8( *FPaths::GameLogDir() );


	// Load all third party dlls
	for( auto & dll : thirdPartyDlls )
	{
		std::string dllPath = BaseDirPlugin + "/" + dll.name;
		dll.handle = !dllPath.empty() ? FPlatformProcess::GetDllHandle( UTF8_TO_TCHAR( dllPath.c_str() ) ) : nullptr;
	}

	// Did all dlls load?
	if( ranges::all_of( thirdPartyDlls, []( auto & dll ) { return dll.handle; } ) )
	{
		UE_LOG( LogUnrealTorch, Warning, TEXT( "======================================== About to call luajit" ) );
		FMessageDialog::Open( EAppMsgType::Ok, LOCTEXT( "ThirdPartyLibrarySuccess", "About to call luajit" ) );

		// Create Lua state and open all libraries
		sol::state lua;
		lua.open_libraries();


		// Make UE_LOG() available in Lua
		lua["UE_LOG"] = UeLogProxy;
		lua.new_enum( "ELogVerbosity",
			"Fatal", ELogVerbosity::Fatal,
			"Error", ELogVerbosity::Error,
			"Warning", ELogVerbosity::Warning,
			"Display", ELogVerbosity::Display,
			"Log", ELogVerbosity::Log,
			"Verbose", ELogVerbosity::Verbose,
			"VeryVerbose", ELogVerbosity::VeryVerbose
		);

		// Lua logging test
		lua.script( "UE_LOG( ELogVerbosity.Warning, 'Hello Lua via UE_LOG!' )" );
		lua.script( "UE_LOG( ELogVerbosity.Log, 'Hello Lua via UE_LOG!' )" );


		// Redefine package.path and package.cpath
		lua["package"]["path"] =
			BaseDirPlugin + "/Source/UnrealTorch/Private/lua/?.lua;" +
			BaseDirPlugin + "/Source/ThirdParty/Torch/WindowsTorch/lua/?.lua;" +
			BaseDirPlugin + "/Source/ThirdParty/Torch/WindowsTorch/lua/?/init.lua;" +
			BaseDirGameContent + "/Lua/?.lua;" +
			BaseDirGameContent + "/Lua/?/init.lua";   // no semicolon at end to avoid getting ;; by accident later
		lua["package"]["cpath"] =
			BaseDirPlugin + "/Source/ThirdParty/Torch/WindowsTorch/bin/?.dll;" +
			BaseDirGameContent + "/Lua/bin/?.dll";   // no semicolon at end to avoid getting ;; by accident later


		// set up some variables in the Lua state
		lua["uth"] = lua.create_table_with(
			"ue", lua.create_table_with(
				"BuildShippingOrTest", UE_BUILD_SHIPPING || UE_BUILD_TEST,
				"FPaths", lua.create_table_with(
					"GameLogDir", BaseDirGameLogs
				)
			)
		);


		// wrap C++ memory into a Lua Torch tensor view
		double tBuf[5] = { 1.0, 2.0, 123.0, 456.0, 789.0 };
		THDoubleStorage * tStorage = THDoubleStorage_newWithData( tBuf, 10 );
		THDoubleTensor * t = THDoubleTensor_newWithStorage1d( tStorage, 0, 10, 1 );
		UE_LOG( LogUnrealTorch, Warning, TEXT( "************************* t[0] == %g, t[1] == %g, t[2] == %g" ), THDoubleTensor_data( t )[0], THDoubleTensor_data( t )[1], THDoubleTensor_data( t )[2] );
		lua["t"] = t;
		lua.script( "torch = require('torch')" );
		lua["t"][sol::metatable_key] = lua.registry()["torch.DoubleTensor"];

		// wrap C++ memory into a Lua Torch tensor view in Lualand
		lua["tBuf"] = reinterpret_cast<std::uintptr_t>( tBuf );
		UE_LOG( LogUnrealTorch, Warning, TEXT( "************************* tBuf == %p" ), tBuf );
		// then in Lua: tStorage = torch.DoubleStorage( 5, tBuf )
		//   But this does not work: tStorage:data() returns a different pointer (lower 32 bits of tBuf, or tBuf & 0x00000000ffffffff) and accessing the storage leads to a crash!


		// run startup_module.lua
		try
		{
			lua.script_file( BaseDirPlugin + "/Source/UnrealTorch/Private/lua/startup_module.lua", sol::default_on_error );
		}
		catch( const sol::error & err )
		{
			// We won't get a stack traceback this way; to get one, we should use a protected_function (cf. https://github.com/ThePhD/sol2/issues/280)
			// but that leads to a rather convoluted/inelegant call syntax
			UE_LOG( LogUnrealTorch, Error, TEXT( "Failed to do startup_module.lua: %s" ), UTF8_TO_TCHAR( err.what() ) );
		}


		// call torch directly in C++
		THFloatTensor * x = THFloatTensor_newWithSize1d( 2 );
		THFloatTensor * y = THFloatTensor_newWithSize1d( 2 );
		THFloatTensor_data( x )[0] = 3;
		THFloatTensor_data( y )[0] = 4;
		THFloatTensor_data( x )[1] = 11;
		THFloatTensor_data( y )[1] = 12;
		auto result = THFloatTensor_dot( x, y );   // auto becomes 'double'..(why?)
		UE_LOG( LogUnrealTorch, Warning, TEXT( "************************* THFloatTensor_dot( x, y ) == %g, x[0] == %g, x[1] == %g" ), result, THFloatTensor_data( x )[0], THFloatTensor_data( x )[1] );

		// access Lua Torch tensors from C++
		THDoubleTensor * z = lua["z"];
		UE_LOG( LogUnrealTorch, Warning, TEXT("************************* Get 'z' from lua: z == %p, z[0] == %g, z[1] == %g"), z, THDoubleTensor_data( z )[0], THDoubleTensor_data( z )[1] );

		// read again our local buffer that is mapped as t in Lualand
		UE_LOG( LogUnrealTorch, Warning, TEXT( "************************* tBuf[0] == %g, tBuf[1] == %g, tBuf[2] == %g" ), tBuf[0], tBuf[1], tBuf[2]);



		UE_LOG( LogUnrealTorch, Warning, TEXT( "======================================== Called luajit" ) );
		FMessageDialog::Open( EAppMsgType::Ok, LOCTEXT( "ThirdPartyLibrarySuccess", "Called luajit" ) );
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load some of the third party DLLs"));
	}
}

void FUnrealTorchModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Free the dll handles
	for( auto & dll : thirdPartyDlls )
	{
		FPlatformProcess::FreeDllHandle( dll.handle );
		dll.handle = nullptr;
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUnrealTorchModule, UnrealTorch)