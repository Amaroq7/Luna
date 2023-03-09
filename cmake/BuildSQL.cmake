include_guard(GLOBAL)

# Building YAML
include(FetchContent)
FetchContent_Declare(
        mariadbconnector
        GIT_REPOSITORY https://github.com/Amaroq7/mariadb-connector-cpp.git
        GIT_TAG 1.1.2-fix
        GIT_SHALLOW ON
)

set(MARIADB_LINK_DYNAMIC ON)
set(WITH_UNIT_TESTS OFF)

FetchContent_MakeAvailable(mariadbconnector)

set(SIZEOF_LONG 4 CACHE INTERNAL "")
set(SIZEOF_SIZE_T 4 CACHE INTERNAL "")
set(SIZEOF_CHARP 4 CACHE INTERNAL "")

if (UNIX)
    target_compile_options(mariadbcpp PRIVATE -Wno-return-type)

    if (IS_CLANG_COMPILER)
        target_compile_options(mariadbcpp PUBLIC -Wno-pedantic -stdlib=libc++)

        target_link_options(mariadbcpp PUBLIC -fuse-ld=${LLD} -stdlib=libc++ --rtlib=compiler-rt)
        target_link_libraries(mariadbcpp PUBLIC c++ c++abi unwind)
    else ()
        target_compile_options(mariadbcpp PRIVATE -Wno-effc++)
    endif ()
else ()
    target_compile_options(mariadbcpp PRIVATE /wd4251 /wd4275)
endif ()

set(SQL_CPP_INCLUDE_DIR ${mariadbconnector_SOURCE_DIR}/include)
set(SQL_CPP_LIBRARIES libmariadbcpp)

set_target_properties(mariadbcpp
        PROPERTIES
        SKIP_BUILD_RPATH OFF
        BUILD_WITH_INSTALL_RPATH ON
        BUILD_RPATH_USE_ORIGIN ON
        INSTALL_RPATH $ORIGIN)

install(TARGETS mariadbcpp
        RUNTIME DESTINATION lib
        LIBRARY DESTINATION lib)

install(TARGETS libmariadb
        RUNTIME DESTINATION lib
        LIBRARY DESTINATION lib)
