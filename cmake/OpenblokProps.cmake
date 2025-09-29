include(TryAddingCompilerFlag)
include(CheckIPOSupported)


function(openblok_add_common_props_for target)
    target_compile_features(${target} PUBLIC cxx_std_17)
    set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)

    if(MSVC)
        try_adding_compiler_flag(${target} /W3)
    else()
        try_adding_compiler_flag(${target} -Wall -Wextra -pedantic)
    endif()

    if(CMAKE_BUILD_TYPE STREQUAL Release OR CMAKE_BUILD_TYPE STREQUAL MinSizeRel)
        check_ipo_supported(RESULT ipo_supported LANGUAGES CXX)
        if(ipo_supported)
            set_target_properties(${target} PROPERTIES INTERPROCEDURAL_OPTIMIZATION ON)
        endif()
    endif()
endfunction()
