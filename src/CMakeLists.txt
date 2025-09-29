include(OpenblokProps)


# OpenBlok executable settings
add_executable(openblok WIN32 main.cpp)
openblok_add_common_props_for(openblok)

configure_file(version.h.in generated/version.h @ONLY)
target_include_directories(openblok PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated")

add_subdirectory(system)
add_subdirectory(game)
target_link_libraries(openblok module_game)


# Install
install(TARGETS openblok RUNTIME DESTINATION ${EXEDIR})
