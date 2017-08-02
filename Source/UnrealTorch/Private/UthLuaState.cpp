// Fill out your copyright notice in the Description page of Project Settings.

#include "UthLuaState.h"

#include "IPluginManager.h"
#include "Paths.h"

#include "UEWrappedSol2.h"

#include <string>
#include <memory>




UUthLuaState::UUthLuaState()
{}


void UUthLuaState::construct()
{
	// make sure that we don't yet have a Lua state
	checkf( !lua, TEXT("Lua state already exists! Maybe the construct() method has been already called?") );


	// Get base directories
	std::string BaseDirPlugin = TCHAR_TO_UTF8( *IPluginManager::Get().FindPlugin( "UnrealTorch" )->GetBaseDir() );
	std::string BaseDirGameContent = TCHAR_TO_UTF8( *FPaths::GameContentDir() );
	std::string BaseDirGameLogs = TCHAR_TO_UTF8( *FPaths::GameLogDir() );

	// create a sol-wrapped Lua state
	lua = std::make_unique<sol::state>();

	// Open all standard libraries
	lua->open_libraries();

	// Set paths
	(*lua)["package"]["path"] =
		BaseDirPlugin + "/Source/UnrealTorch/Private/lua/?.lua;" +
		BaseDirPlugin + "/Source/ThirdParty/Torch/WindowsTorch/lua/?.lua;" +
		BaseDirPlugin + "/Source/ThirdParty/Torch/WindowsTorch/lua/?/init.lua;" +
		BaseDirGameContent + "/Lua/?.lua;" +
		BaseDirGameContent + "/Lua/?/init.lua";   // no semicolon at end to avoid getting ;; by accident later
	(*lua)["package"]["cpath"] =
		BaseDirPlugin + "/Source/ThirdParty/Torch/WindowsTorch/bin/?.dll;" +
		BaseDirGameContent + "/Lua/bin/?.dll";   // no semicolon at end to avoid getting ;; by accident later
}


UUthLuaState::~UUthLuaState()
{}
