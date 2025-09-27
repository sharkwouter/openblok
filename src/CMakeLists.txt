# OpenBlok executable settings
add_executable(openblok WIN32 main.cpp)

configure_file(version.h.in generated/version.h @ONLY)
target_include_directories(openblok PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated")

add_subdirectory(system)
add_subdirectory(game)
target_link_libraries(openblok module_game)
target_compile_features(openblok PRIVATE cxx_std_17)

include(EnableWarnings)
enable_warnings(openblok)

if(WIN32 OR CYGWIN)
    include(TryAddingCompilerFlag)
    try_adding_compiler_flag(openblok -Wl,--subsystem,windows)
endif()


# Install
install(TARGETS openblok RUNTIME DESTINATION ${EXEDIR})
