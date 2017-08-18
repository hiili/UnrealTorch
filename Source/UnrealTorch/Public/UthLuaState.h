// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "UEWrappedSol2.h"    // UBT does not seem to play with unique_ptr and forward declarations..

#include <set>
#include <memory>

#include "UthLuaState.generated.h"




/**
 * Represents a Lua state.
 */
UCLASS( Blueprintable, BlueprintType, meta = (DisplayName = "Lua State") )
class UNREALTORCH_API UUthLuaState : public UObject
{
	GENERATED_BODY()

public:

	// comment block copy at UUthBlueprintStatics::CreateLuaState()
	/** Constructs a new UthLuaState object, creating a new Lua state and initializing it.
	 *
	 * In C++, Use factory helpers like UUthBlueprintStatics::CreateLuaState() or UE's NewObject() to create instances
	 * of this class. Never explicitly delete these instances; instead use the destroy() method.
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
	 * State name and Lua output:
	 *
	 * The internal name of the state object is set to 'default' in the constructor. You can change it with setName() or
	 * by using the Blueprint helper function CreateLuaState(). All Lua output from io.write(), print() and LOG() will
	 * be redirected to Saved\Logs\lua_<name>.log. Note that if you have several states with the same name, you will get
	 * the output of only one of them. The internal name of the state object is not related to UObject names; we do not
	 * use those.
	 *
	 * (C++ only) Object lifecycle and garbage collection:
	 *
	 * When a new Lua state is created using the factory helper UUthBlueprintStatics::CreateLuaState() and the parameter
	 * 'protectFromGC' is to true, the object will be added to the root set of UObjects and thus excluded from garbage
	 * collection. Use UUthLuaState::destroy() to send it toward destruction. If the parameter 'protectFromGC' is left
	 * false, then make sure that you always keep at least one UPROPERTY pointer to the obtained instance.
	 *
	 * IMPORTANT NOTE: In C++, Never explicitly delete instances of UObject-derived classes like this: instances of this
	 * class are managed by the UE garbage collector. Consequently, you shouldn't neither use any smart pointers with
	 * instances of this class.
	 * 
	 * @param name						Internal name of the object. Affects logging; see full comments.
	 * @param protectFromGC (C++ only)	If true, then the object will be added to the GC root set. See full comments.
	 * @return	Returns a valid instance of the class (isValid() == true), or nullptr on any error.
	 */
	UUthLuaState();

	/** Never explicitly delete instances of UObject-derived classes like this: instances of this class are managed by
	 * the UE garbage collector. */
	virtual ~UUthLuaState();

	/** Immediately deletes the Lua state and starts the UObject destruction process.
	 *
	 * This method is safe to call even if the object has been added to the GC root set, in which case it will be
	 * unrooted first.
	 */
	UFUNCTION( BlueprintCallable, Category = "Unreal Torch|Lua" )
	void destroy();

	/** Checks whether the object is in a valid, usable state: the Lua state exists, UObject is not pending kill, .. */
	bool isValid();


	/** Returns a reference to the internal Sol2 Lua state object. */
	sol::state & getLuaState();

	/** Sets the internal name of the state object (the UObject name is not changed). */
	UFUNCTION( BlueprintCallable, Category = "Unreal Torch|Lua" )
	void setName( FName name );

	/** Returns the current internal name of the state object. */
	UFUNCTION( BlueprintCallable, BlueprintPure, Category = "Unreal Torch|Lua" )
	FName getName();


private:

	/** Sol-wrapped Lua state instance */
	std::unique_ptr<sol::state> lua;

	/** The name of this Lua state. See setName(). */
	FName name{ "default" };

};
