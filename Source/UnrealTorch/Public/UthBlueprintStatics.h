// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UthBlueprintStatics.generated.h"


class UUthLuaState;




/**
 * 
 */
UCLASS()
class UNREALTORCH_API UUthBlueprintStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// comment block copy at UUthLuaState::UUthLuaState()
	/** Constructs a new UthLuaState object, creating a new Lua state and initializing it.
	 *
	 * A new Lua state is created and initialized with all Torch-related paths set, necessary packages and dlls loaded,
	 * and a set of utility functions defined.
	 *
	 * All Lua standard libraries are opened.
	 *
	 * A global variable 'uth' is created with the following structure:
	 *   uth								The main table for all UnrealTorch data
	 *     .statename						Name of this state; see UUthLuaState.setName()
	 *     .ue								Data from UE
	 *       .UE_LOG( verbosity, message )	Write log entries to UE log
	 *       .ELogVerbosity					Verbosity level enumeration for UE_LOG()
	 *       .BuildShippingOrTest			True if UE is running in Shipping or Test configuration
	 *       .FPaths						UE path information
	 *         .GameLogDir					Log directory path from FPaths::GameLogDir()
	 *
	 * The package.path variable is set to include the following locations, in this order:
	 *   1. UnrealTorch Lua files
	 *   2. Torch Lua files
	 *   3. Project level Lua files: Content/Lua/?.lua and Content/Lua/?/init.lua
	 *
	 * The package.cpath variable is set to include the following locations, in this order:
	 *   1. Torch DLLs
	 *   2. Project level DLLs: Content/Lua/bin/?.dll
	 *
	 * State name:
	 *
	 * The internal name of the state object is set to 'default' in the constructor. You can change it with setName() or
	 * by using the Blueprint helper function CreateLuaState(). All log output from Lua will be redirected to
	 * Saved\Logs\lua_<name>.log. Note that if you have several states with the same name, you will get the log output
	 * of only one of them. The internal name of the state object is not related to UObject names; we do not use those.
	 *
	 * Object lifecycle and garbage collection:
	 *
	 * When a new Lua state is created using the Blueprint helper function CreateLuaState(), it will be added to the
	 * root set of UObjects and thus excluded from garbage collection. Use UUthLuaState::destroy() to send it toward
	 * destruction. It is safe to call RemoveFromRoot() on the created object if you wish to put it under GC.
	 */
	UFUNCTION( BlueprintCallable, Category = "Unreal Torch|Lua", meta = ( HidePin = "protectFromGC" ) )
	static UUthLuaState * CreateLuaState( FName name = FName( "default" ), bool protectFromGC = false );

};
