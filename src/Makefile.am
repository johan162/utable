# ===============================================================================
#
# =========================================================================

AM_CFLAGS = -pedantic -Wall -Werror -Wpointer-arith -Wstrict-prototypes \
-Wextra -Wshadow -Wno-error=unknown-pragmas -Wno-error=unused-parameter -Werror=format -Wformat=2 -fstack-protector --param=ssp-buffer-size=4 -Wstack-protector \
 -D_FORTIFY_SOURCE=2

# The name of the tstprogram
bin_PROGRAMS = test_table

# Recurse into there directories
SUBDIRS = libunitbl .

if DISABLE_PIE
    AM_CFLAGS += -fno-pie -no-pie
endif

# For development builds, as indicated by the stack trace flag
# We need to include all symbols for the back trace to get proper function names
if STACKTRACE
    AM_CFLAGS += -rdynamic
endif

# All sources
test_table_SOURCES = test_table.c 

test_table_LDADD =  libunitbl/libunitbl.a
test_table_DEPENDENCIES= libunitbl/libunitbl.a

# In Linux iconv() exists in glibc but in OSX we must add the iconv.dylib library to get that function
if is_osx
#    test_table_LDADD += /usr/lib/libiconv.2.dylib
    test_table_LDFLAGS = "-L/usr/local/opt/libiconv/lib"
endif

DISTCLEANFILES=config.h

CLEANFILES=*~ test_table 

unit-test:
	make
	@echo " "
	@echo "Unit tests:"
	@echo "============================="
	@(cd test && ./ut.sh)
	@echo ""

.PHONY: unit-test


