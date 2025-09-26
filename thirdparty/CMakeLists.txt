# tinydir
add_subdirectory(tinydir)


# UnitTest++
if(CMAKE_BUILD_TYPE STREQUAL "debug" AND BUILD_TESTS)
    add_subdirectory(unittest-cpp)
endif()
