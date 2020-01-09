AUTOMAKE_OPTIONS=foreign 
include_HEADERS = libacbmtree.h

#SUBDIRS     = example

# noinst_LTLIBRARIES      = libacbmtree.la
lib_LTLIBRARIES           = libacbmtree.la
libacbmtree_la_SOURCES    = src/acbmtree_keywords.c src/acbmtree_search.c src/acbmtree.c

bin_PROGRAMS     = acbmfilter
acbmfilter_SOURCES    = example/acbmfilter.c
acbmfilter_LDFLAGS    = -lz -lpthread -lacbmtree