AC_INIT([dconf],
        [0.26.0],
        [https://bugzilla.gnome.org/enter_bug.cgi?product=dconf],
        [dconf])

AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11.2 foreign -Wno-portability no-dist-gzip dist-xz])
AM_SILENT_RULES([yes])

# Set default CFLAGS before AC_PROG_CC does
if test "${CFLAGS+yes}" != "yes"; then
  CFLAGS='-O2 -g -Wall -Wwrite-strings -Wmissing-prototypes -fno-common'
fi

# Check for programs
AC_PROG_CC
AC_CHECK_LIB(c, dlsym, LIBDL="", [AC_CHECK_LIB(dl, dlsym, LIBDL="-ldl")])
AC_SUBST(LIBDL)
AM_PROG_VALAC([0.18.0])
AC_PROG_RANLIB
AC_ARG_ENABLE(man,
              [AS_HELP_STRING([--enable-man],
                              [generate man pages [default=yes]])],,
              enable_man=yes)

AS_IF([test "$enable_man" != no], [
  AC_PATH_PROG([XSLTPROC], [xsltproc])
  AS_IF([test -z "$XSLTPROC"], [
    AC_MSG_ERROR([xsltproc is required for --enable-man])
  ])
])
AM_CONDITIONAL(ENABLE_MAN, test "$enable_man" != no)

# Use GLib resources
AC_PATH_PROG(GLIB_COMPILE_RESOURCES, glib-compile-resources)

# Use GSettings
GLIB_GSETTINGS

# Gtk-doc support
GTK_DOC_CHECK([1.15])

# Dependencies
PKG_CHECK_MODULES(glib, glib-2.0 >= 2.44.0)
PKG_CHECK_MODULES(gio, gio-unix-2.0)

AC_ARG_WITH(gio_modules_dir, [  --with-gio-modules-dir=PATH choose directory for the GIO module, [default=LIBDIR/gio/modules]], giomodulesdir="$withval", giomodulesdir=${libdir}/gio/modules)
AC_SUBST(giomodulesdir)

AC_ARG_WITH(dbus_service_dir, [  --with-dbus-service-dir=PATH choose directory for dbus service files, [default=PREFIX/share/dbus-1/services]], dbusservicedir="$withval", dbusservicedir=${datadir}/dbus-1/services)
AC_SUBST(dbusservicedir)

AC_ARG_WITH(dbus_system_service_dir, [  --with-dbus-system-service-dir=PATH choose directory for dbus system service files, [default=PREFIX/share/dbus-1/system-services]], dbussystemservicedir="$withval", dbussystemservicedir=${datadir}/dbus-1/system-services)
AC_SUBST(dbussystemservicedir)

AC_SUBST(dconfincludedir, ${includedir}/dconf)

AC_PATH_PROG(gio_QUERYMODULES, gio-querymodules, no)

AC_ARG_ENABLE(gcov,
              AC_HELP_STRING([--enable-gcov],
                             [enable generation of code coverage information]))

if test "x$enable_gcov" = "xyes"; then
  CFLAGS="$CFLAGS -O0 -fprofile-arcs -ftest-coverage"
  LDFLAGS="$LDFLAGS -lgcov"
fi

dnl Gettext support
GETTEXT_PACKAGE=dconf
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, ["$GETTEXT_PACKAGE"], [Define the gettext package to be used])

AC_CONFIG_FILES([
  common/Makefile
  shm/Makefile
  gvdb/Makefile
  engine/Makefile
  gdbus/Makefile
  gsettings/Makefile
  client/dconf.pc
  client/Makefile
  service/Makefile
  bin/Makefile
  tests/Makefile
  docs/Makefile
  Makefile
])
AC_OUTPUT
