//#define SOL_EXCEPTIONS_SAFE_PROPAGATION   // Do we need this? At least seems to work without..
//#define SOL_NO_EXCEPTIONS   // LuaJIT and exceptions did not play together: https://sol2.readthedocs.io/en/latest/exceptions.html
                            // ..but do now; check the link above!
//#ifndef NDEBUG   // what macro to use with UE?
//#define SOL_CHECK_ARGUMENTS
//#endif


#pragma warning( push )
#pragma warning( disable: 4628 )

#include "UndefineMacros_UE_4.17.h"
#include "sol2/sol.hpp"
#include "RedefineMacros_UE_4.17.h"

#pragma warning( pop )
