--- UnrealTorch manual tests
-- @module uth.tests
--
-- A set of tests to be run and checked manually.


local tests = {}


tests.inspect = require('inspect')




--- Run all manual tests
function tests.run()
  uth.utility.log_function_start()

  -- Test output redirection and logging
  print( 'Testing redirection of print().' )
  io.write( 'Testing redirection of io.write().\n' )
  uth.ue.UE_LOG( ELogVerbosity.Log, 'Testing UE_LOG().' )
  uth.utility.LUA_LOG( ELogVerbosity.Log, 'Testing LUA_LOG().' )
  LOG( ELogVerbosity.Log, 'Testing generic LOG().' )
  function nestedfunc1()
    local nestedfunc1_local = 1
    function nestedfunc2()
      local nestedfunc2_local = nestedfunc1_local + 1
      function nestedfunc3()
        local nestedfunc3_local = nestedfunc2_local + 1
        local upvals = uth.utility.upvalues()
        LOG( ELogVerbosity.Log, 'Testing callstack logging.' )
        LOG( ELogVerbosity.Log, 'Testing upvalues(): ' .. tests.inspect( upvals ) )
      end
      nestedfunc3()
    end
    nestedfunc2()
  end
  nestedfunc1()

  -- Test locals and alllocals querying
  io.write( 'Inspecting locals and alllocals\n' )
  io.write( 'locals: \n' .. tests.inspect( uth.utility.locals() ) .. '\n\n' )
  io.write( 'alllocals: \n' .. tests.inspect( uth.utility.alllocals() ) .. '\n\n' )
  io.write( 'Inspecting locals and alllocals done\n\n' )

  -- Test trapping into ZeroBrane Studio
  io.write( 'Trapping into ZBS debugger\n' ); io.flush();
  uth.utility.breakpoint()
  io.write( 'Trapping into ZBS debugger done\n' ); io.flush();

  -- Test Torch (should be already imported by UnrealTorch)
  torch.test()

  -- Create a tensor to be read on the other side
  z = torch.Tensor(5)
  z[1] = 123
  z[2] = 456

  -- Write to a tensor injected from the other side
--  t[1] = 10
--  t[2] = 20
--  t[3] = 321123


  -- Test including a Content lua module
--  LOG( ELogVerbosity.Log, 'Testing Content/Lua/' )
--  local clt = require('content_lua_test')
--  clt.run()
--  LOG( ELogVerbosity.Log, 'Testing Content/Lua/ done' )

end




return tests
