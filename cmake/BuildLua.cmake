include_guard(GLOBAL)

# Building YAML
include(FetchContent)
FetchContent_Declare(
        lualib
        GIT_REPOSITORY https://github.com/walterschell/Lua.git
        GIT_SHALLOW ON
)

set(LUA_SUPPORT_DL OFF CACHE INTERNAL "")
set(LUA_BUILD_AS_CXX ON CACHE INTERNAL "")
set(LUA_ENABLE_SHARED ON CACHE INTERNAL "")
set(LUA_ENABLE_TESTING OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(lualib)

if (UNIX)
    target_compile_options(lua_static PUBLIC -m32)
    target_link_options(lua_static PUBLIC -m32)
    set_target_properties(lua_static PROPERTIES POSITION_INDEPENDENT_CODE ON)
    target_compile_options(lua_shared PUBLIC -m32)
    target_link_options(lua_shared PUBLIC -m32)
    set_target_properties(lua_shared PROPERTIES POSITION_INDEPENDENT_CODE ON)
    target_compile_options(luac PUBLIC -m32)
    target_link_options(luac PUBLIC -m32)
endif ()

set(LUA_INCLUDE_DIR ${lualib_SOURCE_DIR}/lua-5.4.4/include)
set(LUA_LIBRARIES lua)
set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB32_PATHS ON)

install(TARGETS lua_shared
        RUNTIME DESTINATION bin/lib
        LIBRARY DESTINATION bin/lib)
