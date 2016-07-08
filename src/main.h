#ifndef TIDY_MAIN_H
#define TIDY_MAIN_H

#ifdef HAVE_CONFIG_H
# ifdef PACKAGE
#  undef PACKAGE
# endif
# ifdef PACKAGE_NAME
#  undef PACKAGE_NAME
# endif
# ifdef PACKAGE_STRING
#  undef PACKAGE_STRING
# endif
# ifdef PACKAGE_TARNAME
#  undef PACKAGE_TARNAME
# endif
# ifdef PACKAGE_VERSION
#  undef PACKAGE_VERSION
# endif
# ifdef PACKAGE_BUGREPORT
#  undef PACKAGE_BUGREPORT
# endif
# ifdef VERSION
#  undef VERSION
# endif
# include "config.h"

# ifdef HAVE_INTTYPES_H
#  ifndef __STDC_FORMAT_MACROS
#   define __STDC_FORMAT_MACROS
#  endif
#  ifndef __STDC_LIMIT_MACROS
#   define __STDC_LIMIT_MACROS
#  endif
#  include <inttypes.h>
# else
#  error "uint64_t not defined"
# endif

# ifdef HAVE_REGEX_H
#  include <regex.h>
# endif

# ifdef HAVE_MAGIC_H
#  include <magic.h>
# endif

# ifdef HAVE_OPENSSL_SHA_H
#  include <openssl/sha.h>
# endif

# ifdef HAVE_LOCALE_H
#  include <locale.h>
# endif

#endif//HAVE_CONFIG_H

#endif//TIDY_MAIN_H
