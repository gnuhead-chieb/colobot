#pragma once

// Macros set by CMake
#cmakedefine PLATFORM_WINDOWS @PLATFORM_WINDOWS@
#cmakedefine PLATFORM_LINUX @PLATFORM_LINUX@
#cmakedefine PLATFORM_MACOSX @PLATFORM_MACOSX@
#cmakedefine PLATFORM_OTHER @PLATFORM_OTHER@

#ifdef PLATFORM_MACOSX
// Assume we have the Mac OS X function CFLocaleCopyCurrent in the CoreFoundation framework
#define HAVE_CFLOCALECOPYCURRENT 1
#endif

#cmakedefine GLEW_STATIC

#cmakedefine OPENAL_SOUND

#cmakedefine USE_SDL_MAIN @USE_SDL_MAIN@

#ifdef USE_SDL_MAIN
#define SDL_MAIN_FUNC SDL_main
#else
#define SDL_MAIN_FUNC main
#endif

#define COLOBOT_VERSION "@COLOBOT_VERSION_FULL@"
#define COLOBOT_CODENAME "@COLOBOT_VERSION_CODENAME@"
#define COLOBOT_FULLNAME "Colobot @COLOBOT_VERSION_CODENAME@"

#define COLOBOT_DEFAULT_DATADIR "@COLOBOT_INSTALL_DATA_DIR@"
#define COLOBOT_I18N_DIR "@COLOBOT_INSTALL_I18N_DIR@"

