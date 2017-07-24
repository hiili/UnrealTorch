--- UnrealTorch module startup script
-- @script startup_module
--
-- Called by UE from FUnrealTorchModule::StartupModule().
-- This script performs Lua-side initialization of the Lua state.


-- We assume that the following global variables have been already set from the C++ side:
--
--   uth                             The main table for all UnrealTorch data
--     .ue                           Static data from UE
--       .BuildShippingOrTest        True if UE is built in Shipping or Test configuration
--       .FPaths                     UE path information
--         .GameLogDir               Log directory path from FPaths::GameLogDir()
--
--   UE_LOG( verbosity, message )    Write log entries to UE log
--   ELogVerbosity                   Verbosity level enumeration for UE_LOG()
assert( uth and UE_LOG and ELogVerbosity,
        'Expected to have certain globals pre-set by UE. Some of these variables are missing!' )


UE_LOG( ELogVerbosity.Log, '[startup_module.lua] Initializing Lua state..' )




-- Configuration
uth.NDEBUG = false             -- Disable logging
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


-- Import loggers to the global table
LUA_LOG = uth.utility.LUA_LOG
LOG     = uth.utility.LOG

-- Decorate loggers
UE_LOG  = uth.utility.decorate_logger( UE_LOG, 2 )
LUA_LOG = uth.utility.decorate_logger( LUA_LOG, 2 )
LOG     = uth.utility.decorate_logger( LOG, 2 )

-- If NDEBUG then disable stuff
if uth.NDEBUG then
  function UE_LOG(...) end
  function LUA_LOG(...) end
  function LOG(...) end
end

-- Redirect output
uth.utility.redirect_output()




-- Run manual tests
local tests = require( 'uth.tests' )
tests.run()
