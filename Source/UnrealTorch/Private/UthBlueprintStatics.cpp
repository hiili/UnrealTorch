// Fill out your copyright notice in the Description page of Project Settings.

#include "UthBlueprintStatics.h"

#include "UthLuaState.h"
#include "ScopeGuard.h"




UUthLuaState * UUthBlueprintStatics::CreateLuaState( FName name /*= FName( "default" ) */ )
{
	UUthLuaState * lua = nullptr;
	
	// Set error guard
	auto onError = MakeScopeGuard( [&lua]() {
		UE_LOG( LogUnrealTorch, Error, TEXT( "(%s) Failed to create a new Lua state!" ), TEXT( __FUNCTION__ ) );
		if( lua )
		{
			lua->destroy();
			lua = nullptr;
		}
	} );

	// Create a new Lua state and set its name
	lua = NewObject<UUthLuaState>( GetTransientPackage(), FName(), RF_MarkAsRootSet );    // don't interfere with the UObject name system
	if( !lua || !lua->isValid() ) return nullptr;
	if( !lua->setName( name ) ) return nullptr;

	onError.release();
	return lua;
}




FName UUthBlueprintStatics::MakeUniqueLuaStateName( FName baseName /*= FName( "default" ) */ )
{
	return UUthLuaState::MakeUniqueLuaStateName( baseName );
}
