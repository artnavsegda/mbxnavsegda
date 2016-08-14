AC_INIT([ArtNavsegda], [1.0], [artnavsegda@gmail.com],[artnavsegda],[https://github.com/artnavsegda])
AM_INIT_AUTOMAKE
AC_MSG_NOTICE([Art Navsegda])
AC_PROG_CC
# PKG_CHECK_MODULES(MODBUS, libmodbus)
AC_CHECK_HEADER(modbus.h)
AC_CHECK_LIB([modbus],[modbus_free])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_PREFIX_DEFAULT([/usr])
AC_OUTPUT
