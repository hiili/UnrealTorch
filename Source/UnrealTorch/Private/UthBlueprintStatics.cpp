// Fill out your copyright notice in the Description page of Project Settings.

#include "UthBlueprintStatics.h"

#include "UthLuaState.h"




UUthLuaState * UUthBlueprintStatics::CreateLuaState( FName name /*= FName( "default" ) */ )
{
	UUthLuaState * lua = NewObject<UUthLuaState>( GetTransientPackage(), FName(), RF_MarkAsRootSet );    // don't interfere with the UObject name system
	if( !lua ) return nullptr;
	if( !lua->isValid() )
	{
		lua->destroy();
		return nullptr;
	}

	lua->setName( name );
	return lua;
}
