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
	// Already unique?
	if( UUthLuaState::stateNamesInUse.find( baseName ) != UUthLuaState::stateNamesInUse.end() )
	{
		// No, add running suffix
		std::string uniqueName;
		std::string uniqueNameBase = TCHAR_TO_UTF8( *baseName.ToString() );
		int uniqueNameSuffix = 1;
		do {
			uniqueName = uniqueNameBase + "_" + std::to_string( uniqueNameSuffix++ );
		} while( UUthLuaState::stateNamesInUse.find( FName( uniqueName.c_str() ) ) != UUthLuaState::stateNamesInUse.end() );

		baseName = FName( uniqueName.c_str() );
	}

	return baseName;
}
