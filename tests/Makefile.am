include $(top_srcdir)/build/Makefile.am.gtester

NULL =
EXTRA_DIST =

noinst_PROGRAMS = $(TEST_PROGS) test-config test-keys

progs_ldadd =						\
	$(top_builddir)/mex/libmex-@MEX_API_VERSION@.la	\
	$(MEX_LIBS)

AM_CPPFLAGS = 					\
	-I$(top_srcdir)				\
	-I$(top_srcdir)/mex			\
	-DTESTDATADIR=\"$(srcdir)\"		\
	-DABS_TESTDATADIR=\"$(abs_srcdir)\"	\
	$(MEX_CFLAGS)				\
	$(MEX_WARNING_CFLAGS)			\
	$(NULL)

AM_CFLAGS = -g

TESTS_ENVIRONMENT = srcdir=$(srcdir)

TEST_PROGS       += test-core
test_core_SOURCES  = test-core.c
test_core_LDADD    = $(progs_ldadd)

TEST_PROGS           += test-channel
test_channel_SOURCES  = test-channel.c
test_channel_LDADD    = $(progs_ldadd)
EXTRA_DIST           += channels-uri.conf

TEST_PROGS       += test-epg
test_epg_SOURCES  = test-epg.c
test_epg_LDADD    = $(progs_ldadd)
EXTRA_DIST       += channels-epg.conf channels.dat 1961.dat 94.dat

test_config_SOURCES = test-config.c
test_config_LDADD   = $(progs_ldadd)

test_keys_SOURCES  = test-keys.c
test_keys_LDADD    = $(progs_ldadd)

TEST_PROGS        += test-view
test_view_SOURCES  = test-view.c
test_view_LDADD    = $(progs_ldadd)
