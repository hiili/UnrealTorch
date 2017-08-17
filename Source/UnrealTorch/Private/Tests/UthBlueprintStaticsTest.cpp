#include "CoreTypes.h"
#include "Containers/UnrealString.h"
#include "Misc/AutomationTest.h"

#include "UthBlueprintStatics.h"
#include "UthLuaState.h"

#include "UEWrappedSol2.h"

#include <memory>


#if WITH_DEV_AUTOMATION_TESTS


IMPLEMENT_SIMPLE_AUTOMATION_TEST( FUthBlueprintStaticsTest, "Project.UthBlueprintStatics",
								  EAutomationTestFlags::ApplicationContextMask |
								  EAutomationTestFlags::ProductFilter )




bool FUthBlueprintStaticsTest::RunTest( const FString & Parameters )
{
	{
		UUthLuaState * luaBP{ UUthBlueprintStatics::CreateLuaState() };

		TestNotNull( TEXT( "UthLuaState created in C++ via Blueprint helper CreateLuaState() is not null" ), luaBP );
		TestTrue( TEXT( "UthLuaState created in C++ via Blueprint helper CreateLuaState(): isValid() == true" ), luaBP->isValid() );

		TestEqual( TEXT( "UthLuaState created in C++ via Blueprint helper CreateLuaState() must have name 'default'" ),
				   luaBP->getName(),
				   FName( "default" ) );
	}

	{
		UUthLuaState * luaBP{ UUthBlueprintStatics::CreateLuaState( "TestStateName" ) };

		TestNotNull( TEXT( "UthLuaState created in C++ via Blueprint helper CreateLuaState( <name> ) is not null" ), luaBP );
		TestTrue( TEXT( "UthLuaState created in C++ via Blueprint helper CreateLuaState( <name> ): isValid() == true" ), luaBP->isValid() );

		TestEqual( TEXT( "UthLuaState created in C++ via Blueprint helper CreateLuaState( <name> ) must have name <name>" ),
				   luaBP->getName(),
				   FName( "TestStateName" ) );

		std::string luaStateNameActual = luaBP->getLuaState()["uth"]["statename"];    // Sol requires assignment
		TestEqual( TEXT( "UthLuaState created in C++ via Blueprint helper CreateLuaState( <name> ): Lua uth.statename == <name>" ),
				   luaStateNameActual,
				   std::string( "TestStateName" ) );

		// Testing the current Lua log file is not straightforward, so ignore that
	}

	{
		UUthLuaState * luaBP{ UUthBlueprintStatics::CreateLuaState( "Dummy", false ) };
		UUthLuaState * luaBP_protectFromGC{ UUthBlueprintStatics::CreateLuaState( "Dummy", true ) };

		TestNotNull( TEXT( "UthLuaState created in C++ via Blueprint helper CreateLuaState( <name>, /*protectFromGC =*/ false ) is not null" ), luaBP );
		TestNotNull( TEXT( "UthLuaState created in C++ via Blueprint helper CreateLuaState( <name>, /*protectFromGC =*/ true ) is not null" ), luaBP_protectFromGC );
		TestTrue( TEXT( "UthLuaState created in C++ via Blueprint helper CreateLuaState( <name>, /*protectFromGC =*/ false ): isValid() == true" ), luaBP->isValid() );
		TestTrue( TEXT( "UthLuaState created in C++ via Blueprint helper CreateLuaState( <name>, /*protectFromGC =*/ true ): isValid() == true" ), luaBP_protectFromGC->isValid() );

		CollectGarbage( RF_NoFlags, /*full purge =*/ true );

		// (*luaBP) should be dead now, but we cannot test it without risking a crash
		TestTrue( TEXT( "UthLuaState created in C++ via Blueprint helper CreateLuaState( <name>, /*protectFromGC =*/ true ): CollectGarbage() => isValid() == true" ), luaBP_protectFromGC->isValid() );    // Might crash if luaBP_protectFromGC was nevertheless collected. Setting full purge to false does not help.

		luaBP_protectFromGC->destroy(); luaBP_protectFromGC = nullptr;
	}

	return true;
}




#endif //WITH_DEV_AUTOMATION_TESTS
