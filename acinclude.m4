dnl Check for ALSA driver package.
AC_DEFUN(ALSA_CHECK_DRIVER, [
myprefix=$prefix
if test "$myprefix" = "NONE"; then
  myprefix=$ac_default_prefix
fi
OLD_CFLAGS="$CFLAGS"
CFLAGS="-I$myprefix/include"
AC_MSG_CHECKING(for alsa-driver package)

AC_TRY_COMPILE([
#include <linux/asound.h>
],[
void main(void)
{
#if !defined(SND_PROTOCOL_VERSION) || !defined(SND_PROTOCOL_INCOMPATIBLE)
#error not found
#else
#if !defined(SND_MIXER_IOCTL_ELEMENTS)
#error wrong version
#endif
  exit(0);
#endif
}
],
  AC_MSG_RESULT(present),
  [AC_MSG_RESULT(not found or wrong version);
   AC_MSG_ERROR([Install alsa-driver v0.3.0pre5+ package first...])]
)
CFLAGS="$OLD_CFLAGS"
])

AC_DEFUN(SAVE_LIBRARY_VERSION, [
SND_LIB_VERSION=$VERSION
echo $VERSION > $srcdir/version
AC_DEFINE_UNQUOTED(VERSION, "$SND_LIB_VERSION")
AC_SUBST(SND_LIB_VERSION)
SND_LIB_MAJOR=`echo $VERSION | cut -d . -f 1`
AC_SUBST(SND_LIB_MAJOR)
SND_LIB_MINOR=`echo $VERSION | cut -d . -f 2`
AC_SUBST(SND_LIB_MINOR)
SND_LIB_SUBMINOR=`echo $VERSION | cut -d . -f 3 | sed -e 's/pre[[0-9]]*//g' -e 's/[[[:alpha:]]]*//g'`
AC_SUBST(SND_LIB_SUBMINOR)
])
