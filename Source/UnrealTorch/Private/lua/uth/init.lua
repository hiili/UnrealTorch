--- UnrealTorch Lua state initialization script
-- @script init
--
-- Called by UE from UUthLuaState::construct().
-- This script performs Lua-side initialization of the Lua state.
--
-- The global variable 'uth' has been already set from the C++ side with the following structure:
--   uth                                  The main table for all UnrealTorch data
--     .statename                         Name of this state; see UUthLuaState.setName()
--     .ue                                Static data from UE
--       .UE_LOG( verbosity, message )    Write log entries to UE log
--       .ELogVerbosity                   Verbosity level enumeration for UE_LOG()
--       .BuildShippingOrTest             True if UE is built in Shipping or Test configuration
--       .FPaths                          UE path information
--         .GameLogDir                    Log directory path from FPaths::GameLogDir()
--
-- Output from io.write() and print() is directed to a log file in the UE log directory 'Saved\Logs\'.
--
-- The utility module is imported into 'uth.utility'. A generic logger from the utility module is copied, with its
-- verbosity level enumeration, to the global variable LOG and ELogVerbosity. Usage:
--   LOG( ELogVerbosity.Warning, 'Log message here' )
--
-- Torch is imported into the global variable 'torch'.

uth.ue.UE_LOG( uth.ue.ELogVerbosity.Log, '[init.lua] Initializing Lua state..' )
assert( uth, 'The global variable \'uth\' is expected to be defined by UE but is not present!' )




-- Configuration
uth.NDEBUG = false             -- Disable logging (manually accessing loggers via the 'uth' table still works)
uth.NSTRICT = true             -- Don't use the strict package
uth.LOG_ALSO_TO_UE = false     -- Send the output of LOG() also to UE's log
uth.LOG_CALLSTACK_DEPTH = 2    -- How many levels of callstack to prepend to LOG() messages


-- Configuration overrides
if uth.ue.BuildShippingOrTest then
  uth.NDEBUG = true
  uth.NSTRICT = true
end




-- Load additional modules
require( 'fun' )()    -- import all lua.* functions to globals
if not uth.NSTRICT then require( 'strict' ) end
uth.utility = require( 'uth.utility' )
torch = require( 'torch' )




-- Initialization


-- Decorate loggers
uth.ue.UE_LOG       = uth.utility.decorate_logger( uth.ue.UE_LOG, 2 )
uth.utility.LUA_LOG = uth.utility.decorate_logger( uth.utility.LUA_LOG, 2 )
uth.utility.LOG     = uth.utility.decorate_logger( uth.utility.LOG, 2 )

-- Import main logger and verbosity levels to the global table
LOG           = uth.utility.LOG
ELogVerbosity = uth.ue.ELogVerbosity

-- If NDEBUG then disable various stuff
if uth.NDEBUG then
  function LOG(...) end
end

-- Redirect output
uth.utility.redirect_output()




-- Run manual tests
local tests = require( 'uth.tests' )
--tests.run()
