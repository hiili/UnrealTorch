#include "CoreTypes.h"
#include "Containers/UnrealString.h"
#include "Misc/AutomationTest.h"

#include "UthLuaState.h"

#include "UObjectGlobals.h"

#include <memory>


#if WITH_DEV_AUTOMATION_TESTS


IMPLEMENT_SIMPLE_AUTOMATION_TEST( FUthLuaStateTest, "Project.UthLuaState",
								  EAutomationTestFlags::ApplicationContextMask |
								  EAutomationTestFlags::ProductFilter )




bool FUthLuaStateTest::RunTest( const FString & Parameters )
{
	// Object construction, destruction and naming
	{
		UUthLuaState * luaC1{ NewObject<UUthLuaState>( GetTransientPackage(), FName() ) };    // becomes a dangling pointer after next GC round
		UUthLuaState * luaC2{ NewObject<UUthLuaState>( GetTransientPackage(), FName(), RF_MarkAsRootSet ) };

		TestNotNull( TEXT( "UthLuaState created in C++ via NewObject() is not null" ), luaC1 );
		TestNotNull( TEXT( "UthLuaState created in C++ via NewObject() with MarkAsRootSet flag is not null " ), luaC2 );

		TestTrue( TEXT( "UthLuaState created in C++ via NewObject(): isValid() == true" ), luaC1->isValid() );
		TestTrue( TEXT( "UthLuaState created in C++ via NewObject() with MarkAsRootSet flag: isValid() == true" ), luaC2->isValid() );

		TestEqual( TEXT( "UthLuaState created in C++ via NewObject(): getName() == 'default'" ),
				   luaC1->getName(),
				   FName( "default" ) );

		luaC1->setName( "TestStateName" );
		std::string luaStateNameActual = luaC1->getLuaState()["uth"]["statename"];    // Sol requires assignment
		TestEqual( TEXT( "UthLuaState created in C++ via NewObject(): setName( <name> ) => getName() == <name>" ),
				   luaC1->getName(),
				   FName( "TestStateName" ) );
		TestEqual( TEXT( "UthLuaState created in C++ via NewObject(): setName( <name> ) => Lua uth.statename == <name>" ),
				   luaStateNameActual,
				   std::string( "TestStateName" ) );

		UUthLuaState * luaC1_PendingKill = luaC1;    // becomes a dangling pointer after next GC round
		UUthLuaState * luaC2_PendingKill = luaC2;    // becomes a dangling pointer after next GC round
		luaC1->destroy(); luaC1 = nullptr;
		luaC2->destroy(); luaC2 = nullptr;

		TestFalse( TEXT( "UthLuaState created in C++ via NewObject(): destroy() => isValid() == false" ), luaC1_PendingKill->isValid() );
		TestFalse( TEXT( "UthLuaState created in C++ via NewObject() with MarkAsRootSet flag: destroy() => isValid() == false" ), luaC2_PendingKill->isValid() );
	}

	// Object lifetime and garbage collection
	{
		UUthLuaState * luaC1{ NewObject<UUthLuaState>( GetTransientPackage(), FName() ) };    // becomes a dangling pointer after next GC round
		UUthLuaState * luaC2{ NewObject<UUthLuaState>( GetTransientPackage(), FName(), RF_MarkAsRootSet ) };

		TestTrue( TEXT( "(sanity check; we tested this already)" ), luaC1->isValid() && luaC2->isValid() );

		CollectGarbage( RF_NoFlags, /*full purge =*/ true );
		luaC1 = nullptr;

		// (*luaC1) should be dead now, but we cannot test it without risking a crash
		TestTrue( TEXT( "UthLuaState created in C++ via NewObject() with MarkAsRootSet flag: CollectGarbage() => isValid() == true" ), luaC2->isValid() );    // Might crash if luaC2 was nevertheless collected. Setting full purge to false does not help.

		luaC2->destroy(); luaC2 = nullptr;
	}

	// UUthLuaState::script()
	{
		UUthLuaState * lua{ NewObject<UUthLuaState>( GetTransientPackage(), FName(), RF_MarkAsRootSet ) };

		TestTrue( TEXT( "UthLuaState::script(<valid expression>) -> true" ), lua->script("x1 = 1 + 1") );

		AddExpectedError( TEXT( ".UUthLuaState::script. Failed to run script: runtime error .string .x2 = 1 . 1 . initially_undefined_variable..:1: attempt to perform arithmetic on global 'initially_undefined_variable' .a nil value." ),
						  EAutomationExpectedErrorFlags::Exact, 1 );
		TestFalse( TEXT( "UthLuaState::script(<invalid expression>) -> false" ), lua->script( "x2 = 1 + 1 + initially_undefined_variable" ) );

		lua->script( "initially_undefined_variable = 10" );
		TestTrue( TEXT( "UthLuaState::script(<set x>); UthLuaState::script(<use x>) -> true" ), lua->script( "x3 = 1 + 1 + initially_undefined_variable" ) );

		lua->destroy(); lua = nullptr;
	}

	return true;
}




#endif //WITH_DEV_AUTOMATION_TESTS
