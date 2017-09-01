#include "CoreTypes.h"
#include "Containers/UnrealString.h"
#include "Misc/AutomationTest.h"

#include "UthLuaState.h"

#include "UObjectGlobals.h"
#include "Paths.h"

#include <memory>
#include <fstream>


#if WITH_DEV_AUTOMATION_TESTS


IMPLEMENT_SIMPLE_AUTOMATION_TEST( FUthLuaStateTest, "Project.UnrealTorch.UthLuaState",
								  EAutomationTestFlags::ApplicationContextMask |
								  EAutomationTestFlags::ProductFilter )




bool FUthLuaStateTest::RunTest( const FString & Parameters )
{
	// Object construction, destruction and naming
	{
		UUthLuaState * luaC1{ NewObject<UUthLuaState>( GetTransientPackage(), FName() ) };    // becomes a dangling pointer after next GC round
		UUthLuaState * luaC2{ NewObject<UUthLuaState>( GetTransientPackage(), FName(), RF_MarkAsRootSet ) };

		TestNotNull( TEXT( "UthLuaState created in C++ via NewObject()" ),
					 luaC1 );
		TestNotNull( TEXT( "UthLuaState created in C++ via NewObject() with MarkAsRootSet flag" ),
					 luaC2 );

		TestTrue( TEXT( "UthLuaState created in C++ via NewObject(): isValid()" ),
				  luaC1->isValid() );
		TestTrue( TEXT( "UthLuaState created in C++ via NewObject() with MarkAsRootSet flag: isValid()" ),
				  luaC2->isValid() );

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

		TestFalse( TEXT( "UthLuaState created in C++ via NewObject(): destroy() => isValid()" ),
				   luaC1_PendingKill->isValid() );
		TestFalse( TEXT( "UthLuaState created in C++ via NewObject() with MarkAsRootSet flag: destroy() => isValid()" ),
				   luaC2_PendingKill->isValid() );
	}

	// Object lifetime and garbage collection
	{
		UUthLuaState * luaC1{ NewObject<UUthLuaState>( GetTransientPackage(), FName() ) };    // becomes a dangling pointer after next GC round
		UUthLuaState * luaC2{ NewObject<UUthLuaState>( GetTransientPackage(), FName(), RF_MarkAsRootSet ) };
		check( luaC1 && luaC1->isValid() && luaC2 && luaC2->isValid() );

		CollectGarbage( RF_NoFlags, /*full purge =*/ true );
		luaC1 = nullptr;

		// (*luaC1) should be dead now, but we cannot test it without risking a crash
		TestTrue( TEXT( "UthLuaState created in C++ via NewObject() with MarkAsRootSet flag: CollectGarbage() => isValid()" ),
				  luaC2->isValid() );    // Might crash if luaC2 was nevertheless collected. Setting full purge to false does not help.

		luaC2->destroy(); luaC2 = nullptr;
	}

	// script()
	{
		UUthLuaState * lua{ NewObject<UUthLuaState>( GetTransientPackage(), FName(), RF_MarkAsRootSet ) };
		check( lua && lua->isValid() );

		TestTrue( TEXT( "UthLuaState::script(<valid expression>) return status" ),
				  lua->script("x1 = 1 + 1") );

		AddExpectedError( TEXT( ".UUthLuaState::script. Failed to run script: runtime error .string .x2 = 1 . 1 . initially_undefined_variable..:1: attempt to perform arithmetic on global 'initially_undefined_variable' .a nil value." ),
						  EAutomationExpectedErrorFlags::Exact, 1 );
		TestFalse( TEXT( "UthLuaState::script(<invalid expression>) return status && expected error detected" ),
				   lua->script( "x2 = 1 + 1 + initially_undefined_variable" ) );

		lua->script( "initially_undefined_variable = 10" );
		TestTrue( TEXT( "UthLuaState::script(<set x>); UthLuaState::script(<use x>) return status" ),
				  lua->script( "x3 = 1 + 1 + initially_undefined_variable" ) );

		lua->destroy(); lua = nullptr;
	}

	// Logging from Lua to UE_LOG
	{
		UUthLuaState * lua{ NewObject<UUthLuaState>( GetTransientPackage(), FName(), RF_MarkAsRootSet ) };
		check( lua && lua->isValid() );

		AddExpectedError( TEXT( ".. Test error message from Lua via UE_LOG" ), EAutomationExpectedErrorFlags::Exact, 1 );
		TestTrue( TEXT( "UthLuaState: Lua call to uth.ue.UE_LOG( <error>, <error message> ) return status && expected error detected" ),
				  lua->script( "uth.ue.UE_LOG( uth.ue.ELogVerbosity.Error, 'Test error message from Lua via UE_LOG' )" ) );

		lua->destroy(); lua = nullptr;
	}

	// Lua logging and output
	{
		std::string BaseDirGameLogs = TCHAR_TO_UTF8( *FPaths::GameLogDir() );

		UUthLuaState * lua{ NewObject<UUthLuaState>( GetTransientPackage(), FName(), RF_MarkAsRootSet ) };
		check( lua && lua->isValid() );

		// Set state name and define the expected log file name
		lua->setName( "LuaStateLoggingTest" );
		std::string expectedLogFileName{ BaseDirGameLogs + "/lua_LuaStateLoggingTest.log" };

		// Write to log and define the expected log contents
		lua->script( R"(  print('Test print()')  )" );
		lua->script( R"(  io.write('Test io.write()\n')  )" );
		lua->script( R"(  LOG( ELogVerbosity.Log, 'Test LOG()')  )" );
		const std::array<std::string, 3> expectedLogLines{ {
				"[print] Test print()",
				"Test io.write()",
				"[Log] [] Test LOG()",
			} };

		// Destroy the state so as to close the log file
		lua->destroy(); lua = nullptr;

		// Compare the log file to the expected lines
		std::ifstream logFile( expectedLogFileName, std::ios_base::in );
		std::string logLine;
		for( auto & expectedLogLine : expectedLogLines )
		{
			TestTrue( TEXT( "UthLuaState: Lua calls to print(), io.write() and LOG() cause expected output in the log file" ),
					  getline( logFile, logLine ) && logLine == expectedLogLine );
		}

		logFile.close();
	}

	return true;
}




#endif //WITH_DEV_AUTOMATION_TESTS
