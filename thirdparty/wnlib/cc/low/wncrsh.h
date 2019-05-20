/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

AUTHOR:

  Will Naylor

****************************************************************************/


#define wn_assert(_cond)\
{\
  if(!(_cond))\
  {\
    wn_assert_routine(__FILE__,__LINE__);\
  }\
}
       

#define wn_assertmsg(_cond,_msg)\
{\
  if(!(_cond))\
  {\
    wn_assertmsg_routine(__FILE__,__LINE__,(_msg));\
  }\
}
       

#define wn_crash()             wn_crash_routine(__FILE__,__LINE__)


#define wn_crashmsg(_msg)      wn_crashmsg_routine(__FILE__,__LINE__,(_msg))


#define wn_assert_nc(_cond)\
{\
  if(!(_cond))\
  {\
    wn_assert_nc_routine(__FILE__,__LINE__);\
  }\
}
       

#define wn_assertmsg_nc(_cond,_msg)\
{\
  if(!(_cond))\
  {\
    wn_assertmsg_nc_routine(__FILE__,__LINE__,(_msg));\
  }\
}
       

#define wn_crash_nc()          wn_crash_nc_routine(__FILE__,__LINE__)


#define wn_crashmsg_nc(_msg)   wn_crashmsg_nc_routine(__FILE__,__LINE__,(_msg))


