SET(LUA_CORE_C lapi.c lcode.c lctype.c ldebug.c ldo.c ldump.c lfunc.c lgc.c llex.c
    lmem.c lobject.c lopcodes.c lparser.c lstate.c lstring.c ltable.c
    ltm.c lundump.c lvm.c lzio.c)
SET(LUA_LIB_C lauxlib.c lbaselib.c lbitlib.c lcorolib.c ldblib.c liolib.c
    lmathlib.c loslib.c lstrlib.c ltablib.c lutf8lib.c loadlib.c linit.c)


add_library(lua STATIC ${LUA_CORE_C} ${LUA_LIB_C})

