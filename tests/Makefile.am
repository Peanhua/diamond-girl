if WITH_BUNDLED_MTWIST
   WBM_CFLAGS = -DWITH_BUNDLED_MTWIST
endif

AM_CFLAGS = $(CWARNFLAGS) $(WBM_CFLAGS) $(mtwist_CFLAGS) $(sdl_CFLAGS)

noinst_PROGRAMS = random

random_SOURCES = random.c bundled_mtwist.c

random_LDADD = $(mtwist_LIBS) $(sdl_LIBS) $(libcrypto_LIBS)

