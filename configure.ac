AC_INIT([Diamond Girl], [1.25.2], [joniyrjana@gmail.com], [diamond-girl], [http://www.netikka.net/joyr/diamond_girl/])
AM_INIT_AUTOMAKE([foreign -Wall -Werror dist-xz subdir-objects])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AC_CONFIG_SRCDIR([src/main.c])


# Optional opengl mode support, defaults to true.
opengl=true
AC_ARG_ENABLE(opengl,
        [AS_HELP_STRING([--enable-opengl],[Include OpenGL mode. Defaults to 'yes'.])],
        [case "${enableval}" in
          yes) opengl=true  ;;
          no)  opengl=false ;;
          *)   AC_MSG_ERROR([Bad value '${enableval}' for --enable-opengl, valid values: 'yes' and 'no'.]) ;;
        esac]
  )
AM_CONDITIONAL([WITH_OPENGL], [test x$opengl = "xtrue"])
# Optional non-opengl mode support, defaults to true.
nonopengl=true
AC_ARG_ENABLE(nonopengl,
        [AS_HELP_STRING([--enable-nonopengl],[Include non-OpenGL mode. Defaults to 'yes'.])],
        [case "${enableval}" in
          yes) nonopengl=true  ;;
          no)  nonopengl=false ;;
          *)   AC_MSG_ERROR([Bad value '${enableval}' for --enable-nonopengl, valid values: 'yes' and 'no'.]) ;;
        esac]
  )
AM_CONDITIONAL([WITH_NONOPENGL], [test x$nonopengl = "xtrue"])


#AC_USE_SYSTEM_EXTENSIONS
AM_GNU_GETTEXT_VERSION(0.19)
AM_GNU_GETTEXT([external])


# Checks for programs.
AC_PROG_AWK
AC_PROG_GREP
AC_PROG_EGREP
AC_PROG_SED
AC_CHECK_PROG([xcf2png], [xcf2png], [xcf2png])
if test "x$ac_cv_prog_xcf2png" == "x" ; then
  AC_MSG_ERROR(['xcf2png' not found error, install XCFTools.])
fi
AC_PROG_CC
AC_PROG_CC_C99
AC_PROG_CPP
AC_PROG_INSTALL

ACX_PTHREAD

AC_CHECK_PROG([optipng], [optipng], [optipng])

# Checks for libraries.
AC_CHECK_LIB(m, sinf)
PKG_CHECK_MODULES([sdl],        [sdl])
PKG_CHECK_MODULES([sdl_image],  [SDL_image], [],
  [ AC_CHECK_LIB([SDL_image], [IMG_Load], [],
    [ AC_MSG_ERROR([unable to find SDL_image library])] )]
)
PKG_CHECK_MODULES([openal],     [openal])
PKG_CHECK_MODULES([libpng],     [libpng])
PKG_CHECK_MODULES([sndfile],    [sndfile])
PKG_CHECK_MODULES([json],       [json-c >= 0.11], [], [AC_MSG_ERROR(['json-c' not found error])])
PKG_CHECK_MODULES([libmikmod],  [libmikmod], [],
  [ AM_PATH_LIBMIKMOD([], [],
    [ AC_MSG_ERROR([unable to find libmikmod])])]
)
bundled_mtwist=false
PKG_CHECK_MODULES([mtwist],     [mtwist], [], [bundled_mtwist=true])
AM_CONDITIONAL([WITH_BUNDLED_MTWIST], [test x$bundled_mtwist = "xtrue"])


# Optional support for old (1.23 and earlier) LUA settings file, defaults to true.
AC_ARG_WITH([lua],
            [AS_HELP_STRING([--without-lua], [disable support for LUA (pre 1.24 settings file)])],
            [],
            [with_lua=yes])

if test "x$with_lua" = "xyes" ; then
  PKG_CHECK_MODULES([lua], [lua], [AC_DEFINE([HAVE_LUA], [1], [Use LUA])],
                                  [ PKG_CHECK_MODULES([lua], [lua5.1], [AC_DEFINE([HAVE_LUA], [1], [Use LUA])]) ] )
fi




AC_MSG_CHECKING([for MikMod_free])
AC_EGREP_HEADER([MikMod_free], [mikmod.h], [mikmodfree=true], [mikmodfree=false])
AM_CONDITIONAL([WITH_MIKMODFREE], [test x$mikmodfree = "xtrue"])
AC_MSG_RESULT([${mikmodfree}])

PKG_CHECK_MODULES([sdl_gfx], [SDL_gfx], [],
  [ AC_SEARCH_LIBS([SDL_framerateDelay], [SDL_gfx], [],
    [ AC_MSG_ERROR([unable to find SDL_gfx library containing SDL_framerateDelay()]) ],
    [$sdl_LIBS]
  )]
)

AC_SEARCH_LIBS([BZ2_bzWriteOpen], [bz2], [],
  [AC_MSG_ERROR([unable to find BZ2_bzWriteOpen() function])]
)

PKG_CHECK_MODULES([vorbisfile], [vorbisfile >= 1.2.0])

# Libraries required for opengl -option.
if test "x$opengl" == "xtrue" ; then
PKG_CHECK_MODULES([gl], [gl], [],
  [AC_CHECK_LIB([GL], [glLoadIdentity], [],
    [AC_CHECK_LIB([opengl32], [main], [],
      [AC_MSG_ERROR([unable to find OpenGL library])] )] )]
)
PKG_CHECK_MODULES([glu], [glu], [],
  [AC_CHECK_LIB([GLU], [gluProject], [],
    [AC_CHECK_LIB([glu32], [main], [],
      [AC_MSG_ERROR([unable to find GLU library])] )] )]
)
PKG_CHECK_MODULES([glew], [glew], [],
  [AC_CHECK_LIB([GLEW], [glewInit], [],
    [AC_MSG_ERROR([unable to find GLEW library])] )]
)
AM_PATH_LIB3DS(2.0.0, [lib3ds_version1=false], [
  AM_PATH_LIB3DS(1, [lib3ds_version1=true], [
    AC_MSG_ERROR([unable to find lib3ds])]
  )
])
AM_CONDITIONAL([WITH_LIB3DS_V1], [test x$lib3ds_version1 = "xtrue"])
fi

# Libraries required for hash checksumming.
checksumming=true
checksumming_library_mhash=0
checksumming_library_crypto=0
AC_SEARCH_LIBS([mhash_init], [mhash], [checksumming_library_mhash=1], [
  PKG_CHECK_MODULES([libcrypto], [libcrypto], [checksumming_library_crypto=1], [
    AC_MSG_WARN([unable to find libmhash or libcrypto, hash checksumming is disabled])
    checksumming=false
  ])
])
AM_CONDITIONAL([WITH_CHECKSUMMING], [test x$checksumming = "xtrue"])
AC_SUBST([CHECKSUMMING_LIBRARY_MHASH],  $checksumming_library_mhash)
AC_SUBST([CHECKSUMMING_LIBRARY_CRYPTO], $checksumming_library_crypto)

# Checks for header files.
AC_CHECK_HEADERS([inttypes.h stdint.h stdlib.h string.h sys/time.h sys/timeb.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDC
AC_HEADER_STDBOOL
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_CHECK_FUNCS([malloc realloc gettimeofday memset mkdir sqrt strchr strdup strerror strstr strtol strtoul localtime_r])

# Check for backtrace stuff.
AC_SEARCH_LIBS([backtrace], [execinfo])

# Check for windows build.
AC_MSG_CHECKING([for win32])
AC_COMPILE_IFELSE([AC_LANG_SOURCE([
#ifdef WIN32
this is a win32 system
#endif
])], [win32=false], [win32=true])
AM_CONDITIONAL([WITH_WIN32], [test x$win32 = "xtrue"])
AC_MSG_RESULT([${win32}])

if test "x$win32" == "xtrue" ; then
   AC_CHECK_TOOL([WINDRES], [windres])
   if test "x$WINDRES" == "x" ; then
     AC_MSG_ERROR(['windres' not found error.])
   fi
   AC_CHECK_PROG([ICOTOOL], [icotool], [icotool])
   if test "x$ICOTOOL" == "x" ; then
     AC_MSG_ERROR(['icotool' not found error.])
   fi
fi

AC_SUBST(CWARNFLAGS)
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile maps/Makefile data/Makefile themes/Makefile ui/Makefile po/Makefile.in tests/Makefile themes/default/Makefile themes/default/gfx/Makefile themes/default/sfx/Makefile themes/new-year/Makefile themes/new-year/gfx/Makefile themes/new-year/sfx/Makefile themes/halloween/Makefile themes/halloween/gfx/Makefile themes/halloween/sfx/Makefile themes/x-mas/Makefile themes/x-mas/gfx/Makefile themes/easter/Makefile themes/easter/gfx/Makefile themes/easter/sfx/Makefile themes/iron-girl/Makefile themes/iron-girl/gfx/Makefile])
AC_OUTPUT
