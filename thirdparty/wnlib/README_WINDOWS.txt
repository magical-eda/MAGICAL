LEGAL:

  BY DOWNLOADING OR COMPILING THIS LIBRARY, YOU ACCEPT AND AGREE TO THE TERMS 
  AND CONDITIONS PRINTED BELOW.  IF YOU DO NOT AGREE, DO NOT DOWNLOAD OR 
  COMPILE THIS LIBRARY.

  The author provides this C code in the hope
  that it will be helpful, however, we assume no responsibility 
  for the use of this code, nor any responsibility for its support.
  The software is distributed on an "AS IS" basis, without warranty.
  Neither the authors nor the software developers
  make any representation, or warranty, either express or implied, with
  respect to the software programs and subroutines, their quality, accuracy, 
  or fitness for a specific purpose.  Therefore, neither the authors nor the 
  software developers shall have any liability to 
  you or any other person or entity with respect to any liability, loss, 
  or damage caused or alleged to have been caused directly or indirectly by 
  the programs and subroutines contained in this library.  This includes, but 
  is not limited to, interruption of service, loss of data, loss of classroom 
  time, loss of consulting or anticipatory profits, or consequential damages 
  from the use of these programs.  

COPYRIGHT NOTICE:

  The source code in this directory and all its subdirectories is
  provided free of charge to anyone who wants it.  It is in the public
  domain and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

  This copyright notice and the above legal notice may not be removed.

AUTHORS:

  Will Naylor
    email: 
      pub@willnaylor.com
        start subject line with "wnlib: " to help distinguish it from spam
    web: 
      http://www.willnaylor.com

  Bill Chapman
    email:
      WNLIB@CCJJ.INFO
        start subject line with "wnlib: " to help distinguish it from spam


RELEASE:

  17 Sep 2004        Release 9.1

  12 Feb 2003        Release 8.0

  27 Nov 2002        Release 7.0

  16 May 1996        Release 6.0

CONTENTS OF SUBDIRECTORIES:

  acc     -- ANSI C source for useful subroutines to link to applications.
             Final library is acc/text.a

  cc      -- Old k&r source, kept for old users but no longer updated.  Note
             this no longer builds cleanly on Linux, and no attempt has been
             made to port it to Windows.  It is recommended that new users
             ignore or even delete this.

  doc     -- documentation pages for these subroutines (initially empty,
             builds elsewhere copy the documentation into these directories)

  command -- useful shell scripts, programs and login procedures

  archive -- source for command directory

SUMMARY OF USE:

  ARCHIVE is built as:          wnlib/acc/text.a
  H FILES are linked to:        wnlib/acc/h/*.h
  MAN PAGES are linked to:      wnlib/doc/man/*.man
                                wnlib/doc/mantext/*.txt
  Main makefile                 wnlib/acc/makefile

  Downloading, installation and building instructions are in the file
  INSTALL in this directory.

  Virtually everything important is in the "acc" subdirectory, which is
  principally the ANSI C source for the subroutine library.  The directories
  "cc" and "command" contain optional information that may be ignored.

PAST PORTING TARGETS:

  WNLIB has been successfully ported to

      Sun Solaris: cc, acc, CC, gcc, g++
      SuSE Linux 9.0: cc == gcc 3.3.1, g++ 3.3.1
      Cygwin
      Windows ME: Microsoft Visual C++ 4.0 /* see 'windows' directory */
      Windows XP: Microsoft Visual C++ 6.0 /* see 'windows' directory */
      VMS (though not for several years)
      HPUX

  Generally, we restrict ourselves to using C as of the ANSI/ISO
  9899-1990 Standard, which was finalized in December 1989.  The code
  builds without errors according to the ANSI 89, 94, and 99 standards
  in pedantic mode, and in C++.

DOCUMENTATION

  If you want to see all the documentation before doing a complete
  build, you must do a 'make man' from this directory first.  'make man'
  is automatically done if you do a complete build from here.

  After you 'make man', the documentation for the individual functions will
  be in the ./doc/mantext directory, the master document is
  main_man.html (the text version is main_man.txt).

NOTE (warning):

  This library defines a few identifiers without a "wn_" prefix.  They
  are all in wnlib/acc/low/wncons.h and wnlib/acc/low/wnport.h.  They
  are "local", "ptr", "bool" (except on C++, where it is left alone),
  "TRUE", "FALSE", "NULL" (unless NULL is previously defined), and
  EXTERN.

DISCUSSION:

  This directory contains numerous tools that I have found to be very
  useful in my programming practice.  The subroutines in acc form a
  useful base for quickly writing large, efficient, reliable C/UNIX
  programs for wide variety of applications.  These routines form
  an integrated package:  they rely upon each other, they use the
  same naming conventions, etc.  

  Particularly useful packages in this directory include:

    mem  -- improved memory allocator, useful in almost everything.
            see acc/mem/wnmem.man and acc/mem/wnmemd.man

    list -- singly linked list package
            see acc/list/wnsll.man

    sort -- high quality sorting package, better than QSORT.
            see acc/sort/wnsort.man

    hash -- generic hash table package, useful for parsing and 
            compiling and other searching applications.
            see acc/hash/wnhtab.man

    mat  -- matrix inverse.
            see acc/mat/wnminv.man
         -- simplex method package for linear programming, useful
            for solving highly complex, difficult optimization 
            problems.
            see acc/mat/wnsplx.man

    conjdir -- conjugate directions optimization package for minimizing
            non-linear differentiable functions
            see acc/conjdir/wnconj.man

    anneal -- simulated annealing package, useful for attacking complex
              combinatorial optimization problems 
              see acc/anneal/wnanl.man

  The code in this directory is generally industrial quality code.
  New releases will come out periodically, and can be obtained from
  where you obtained this release, or from us.  This code is in the 
  public domain and we encourage you to give it to anybody who wants it.

  However, we ask that you consult us before modifying any of it,
  to avoid multiple versions floating around.  Obviously, if
  you are relying on a modified version, you will have trouble
  using the new releases (which won't have your modifications).

  Good luck!!
