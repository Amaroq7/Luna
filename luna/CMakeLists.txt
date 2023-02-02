project(luna)

string(TIMESTAMP CURRENT_YEAR %Y)
set(ANUBIS_RELEASE_MODE ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION} ${CMAKE_BUILD_TYPE})

# Generate MetaInfo.hpp
find_program(GIT git)
if (GIT AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
    execute_process(COMMAND ${GIT} rev-parse --short HEAD OUTPUT_VARIABLE COMMIT OUTPUT_STRIP_TRAILING_WHITESPACE)
    configure_file(AnubisInfo.hpp.in AnubisInfo.hpp @ONLY)
else ()
    set(COMMIT "<unknown>")
    configure_file(LunaInfo.hpp.in LunaInfo.hpp @ONLY)
endif ()

set(SRC_FILES
        AnubisExports.cpp
        PluginSystem.cpp
        BasicNatives.cpp
        EdictNatives.cpp
        ClassNatives.cpp
        ClassHandler.cpp)

add_library(${PROJECT_NAME} MODULE ${SRC_FILES})

target_include_directories(${PROJECT_NAME}
        SYSTEM
        PRIVATE
        ${CMAKE_BINARY_DIR}/luna
        ${YAML_CPP_INCLUDE_DIR}
        ${FMT_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/deps/anubis
        ${LUA_INCLUDE_DIR}
        ${DEPS_INCLUDE_DIR})

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

if (UNIX)
    target_link_options(${PROJECT_NAME} PRIVATE -m32 -Wl,--disable-new-dtags)
    target_link_libraries(${PROJECT_NAME} PRIVATE dl)
    target_compile_options(${PROJECT_NAME} PRIVATE -m32 -Wall -Werror -Wextra -Wpedantic -pedantic-errors)

    if (IS_CLANG_COMPILER)
        target_compile_options(${PROJECT_NAME} PRIVATE -stdlib=libc++)
        target_link_options(${PROJECT_NAME} PRIVATE -stdlib=libc++ --rtlib=compiler-rt -fuse-ld=${LLD} -Wl,--as-needed)
        target_link_libraries(${PROJECT_NAME} PRIVATE c++ c++abi unwind m)
    endif ()
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE /experimental:external
            /external:I ${CMAKE_SOURCE_DIR}/rehlds
            /external:W0
            /W4 /WX)

    target_compile_definitions(${PROJECT_NAME} PRIVATE _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING)
endif ()

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        VISIBILITY_INLINES_HIDDEN ON
        CXX_VISIBILITY_PRESET hidden)

target_link_libraries(${PROJECT_NAME} PRIVATE ${YAML_CPP_LIBRARIES} ${FMT_LIBRARIES} lua_static)
add_dependencies(${PROJECT_NAME} ${YAML_CPP_LIBRARIES} ${FMT_LIBRARIES} lua_static)

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin/dlls
        LIBRARY DESTINATION bin/dlls
        )

install(
        DIRECTORY ${CMAKE_SOURCE_DIR}/configs
        DESTINATION bin
)

install(
        FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
        DESTINATION bin/lib
)

#SDK
install(
        DIRECTORY ${CMAKE_SOURCE_DIR}/public/
        DESTINATION sdk/include/luna
        FILES_MATCHING PATTERN "*.hpp"
)

# TODO: Come up with better solution than just hardcoding paths
if (IS_CLANG_COMPILER)
    file(GLOB_RECURSE LIBCXX_LIBS FOLLOW_SYMLINKS
            LIST_DIRECTORIES false
            /usr/lib32/libc++*.so*
            /usr/lib32/libunwind.so
            /usr/lib32/libunwind.so.1*)
    install(FILES ${LIBCXX_LIBS} DESTINATION bin/lib)
    install(FILES ${LIBCXX_LIBS} DESTINATION sdk/lib)
endif ()
