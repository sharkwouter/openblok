include(FetchContent)

set(FETCHCONTENT_QUIET OFF)


function(openblok_declare_gitrepo name url tag)
    FetchContent_Declare(
        ${name}
        GIT_REPOSITORY ${url}
        GIT_TAG ${tag}
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        OVERRIDE_FIND_PACKAGE
    )
endfunction()


if(USE_BUNDLED_SDL2)
    # SDL2
    set(SDL_STATIC OFF)
    openblok_declare_gitrepo(
        SDL2
        https://github.com/libsdl-org/SDL.git
        release-2.32.10
    )

    # SDL2-image
    set(SDL2IMAGE_VENDORED ON)
    set(SDL2IMAGE_DEPS_SHARED OFF)
    set(SDL2IMAGE_AVIF OFF)
    set(SDL2IMAGE_BMP OFF)
    set(SDL2IMAGE_GIF OFF)
    set(SDL2IMAGE_JPG OFF)
    set(SDL2IMAGE_JXL OFF)
    set(SDL2IMAGE_LBM OFF)
    set(SDL2IMAGE_PCX OFF)
    set(SDL2IMAGE_PNG ON)
    set(SDL2IMAGE_PNM OFF)
    set(SDL2IMAGE_QOI OFF)
    set(SDL2IMAGE_SVG OFF)
    set(SDL2IMAGE_TGA OFF)
    set(SDL2IMAGE_TIF OFF)
    set(SDL2IMAGE_WEBP OFF)
    set(SDL2IMAGE_XCF OFF)
    set(SDL2IMAGE_XPM OFF)
    set(SDL2IMAGE_XV OFF)
    openblok_declare_gitrepo(
        SDL2_image
        https://github.com/libsdl-org/SDL_image
        release-2.8.8
    )

    # SDL2-mixer
    set(SDL2MIXER_VENDORED ON)
    set(SDL2MIXER_DEPS_SHARED OFF)
    set(SDL2MIXER_FLAC ON)
    set(SDL2MIXER_GME OFF)
    set(SDL2MIXER_MOD ON)
    set(SDL2MIXER_MP3 ON)
    set(SDL2MIXER_MIDI OFF)
    set(SDL2MIXER_OPUS ON)
    set(SDL2MIXER_WAVE OFF)
    set(SDL2MIXER_WAVPACK OFF)
    openblok_declare_gitrepo(
        SDL2_mixer
        https://github.com/libsdl-org/SDL_mixer
        release-2.8.1
    )

    # SDL2-ttf
    set(SDL2TTF_VENDORED ON)
    set(SDL2TTF_DEPS_SHARED OFF)
    openblok_declare_gitrepo(
        SDL2_ttf
        https://github.com/libsdl-org/SDL_ttf
        release-2.24.0
    )
endif()


if(USE_BUNDLED_SDL2PP)
    set(SDL2PP_WITH_IMAGE ON)
    set(SDL2PP_WITH_MIXER ON)
    set(SDL2PP_WITH_TTF ON)
    openblok_declare_gitrepo(
        SDL2pp
        https://github.com/libSDL2pp/libSDL2pp
        b00d3b9eb98be4fa2eca7ae9d88f96d28796e4f0
    )
endif()
