#
# Uncomment this line to specify the C compiler if the system default isn't
# what you want to use.  
#
#CC=cc
#
# Use this line to specify options for the C compiler.  You'll probably
# want to turn on optimizations. You may also have to use some of the 
# following flags:
#
#  -DCAPSBLAS         if BLAS routine names are capitalized.
#  -DCAPSLAPACK       if LAPACK routine names are capitalized.
#  -DNOUNDERBLAS      if BLAS routine names have no underscore.
#  -DNOUNDERLAPACK    if LAPACK routine names have no underscore.
#  -DBIT64            For I32LP64 systems.
#  -DNOSHORTS         Allow for (LP) blocks of more than 65535 variables.
#  -DUSEOPENMP        Build an OpenMP parallel version.
#  -DSETNUMTHREADS    Work with OpenMP aware BLAS.  
#  -DUSESIGTERM       Terminate nicely at the end of the next iteration
#                     after receiving a SIGTERM signal
#  -DUSEGETTIME       Use ANSI C gettime() routine to determine clock
#                     time used in different parts of the code.
#  -DUSEATLAS         Turns on some special code for use with the ATLAS BLAS.
#
# The default settings for gcc:
#
#CFLAGS=-g -O1 -ansi -Wall -DNOSHORTS -I../include
CFLAGS=-O3 -ansi -fPIC -Wall -DNOSHORTS -I../include
#
# Notes on CFLAGS.
#
# 1. The -DNOSHORTS flag should always be used.  When this is turned off,
#    it causes the code to use unsigned short integers (maximum value 65535)
#    in the data structures that describe the problem.  This can cause problems
#    when some of the size parameters are larger than 65535.  
#
# 2. -DUSEGETTIME is generally useful.  However, this functionality isn't
#    available under Windows/MSYS/MINGW, so you should remove it when compiling
#    for that system.
#
# 3. By default, we assume that BLAS routines have names like ddot_(),
#    but some systems use ddot(), DDOT(), or DDOT_() instead.  A similar issue
#    effects the LAPACK routines.  The flags -DCAPSBLAS, -DCAPSLAPACK, 
#    -DNOUNDERBLAS. and -DNOUNDERLAPACK are used to handle such situations.
# 
# 4. The code can be built on 64 bit systems that use an I32LP64 model
#    in which int's are 32 bits, long's and pointers are 64 bits.  Note that 
#    that is the model on all 64 bit Linux and Unix systems that I'm aware of,
#    but it is not the model used by MS in its 64 bit Windows!  To build a 
#    64 bit version of the code, use -DBIT64.  You may also need to add a CFLAG
#    to tell your compiler to produce 64 bit code.  For example, with gcc, 
#    you'll need to add "-m64" to CFLAGS to produce 64 bit code.  
#
# 5. If you have multiple CPU's, and if your compiler supports OpenMP, then
#    you should definitely build a parallel version of CSDP.  To do this,
#    add "-DUSEOPENMP" to CFLAGS.  If your BLAS/LAPACK library routines 
#    use OpenMP's conventions for setting the number of threads to use, then
#    you should also add "-DSETNUMTHREADS".  Note that ATLAS does not 
#    currently work with "-DSETNUMTHREADS", but you can use SETNUMTHREADS
#    on Solaris with -lsunperf and AIX systems with -lesslsmp.
#
#    You will also have to add appropriate CFLAGS to tell your compiler
#    to compile OpenMP code.  This varies from system to system.  However,
#    with gcc4.2, you can use 
#          
#      -static -fopenmp 
#
#    You may also have to add additional libraries when you link your 
#    OpenMP code.  For example, gcc 4.2 with OpenMP needs 
# 
#      -lgomp -lrt -lpthread
#    
# 6. Using gcc, you can greatly improve the efficency of the code if you 
#    specify your processor type.  Examples are given below, use the default 
#    if you are unsure.  More examples may be found at 
#
#     http://gcc.gnu.org/onlinedocs/gcc/i386-and-x86_002d64-Options.html
#
# An AMD Athlon XP  based machine:
#  CFLAGS=-march=athlon-xp -O3 -ansi -Wall -DNOSHORTS  -I../include
# An Intel Pentum 4 based machine:
#  CFLAGS=-march=pentum4 -O3 -ansi -Wall -DNOSHORTS  -I../include
#
# 7. The code can be built with Intel's icc C compiler and MKL BLAS/
#    LAPACK libraries.  This has been tested with icc 11.0 and should 
#    work with later versions as well.
#
#      CC=icc
#      CFLAGS=-O3 -m64 -openmp -DBIT64 -DUSEOPENMP -DNOSHORTS -DSETNUMTHREADS
#              -I../include
#      LIBS=-openmp -L../lib -lsdp -mkl -lm
#
# 8. The can works particularly well with the ATLAS BLAS/LAPACK routines 
#    when compiled with gcc. There are some important optimizations in the
#    code that depend on the availability of routines within ATLAS, so use
#    use -DUSEATLAS to turn these on.  e.g.  
#
#      CFLAGS=-O3  -fopenmp  -ansi -Wall -DUSEATLAS -DNOSHORTS -DBIT64 
#             -DUSEOPENMP -I../include
#
#
# 9. If you change the CFLAGS, make sure that you use the same CFLAGS
#    in the Makefiles in the solver and theta directories!
#
#  Build the library.
#
libsdp.a: readprob.o sdp.o op_o.o psd_feas.o op_a.o op_at.o  Fnorm.o calc_pobj.o calc_dobj.o trace_prod.o zero_mat.o mat_mult.o sym_mat.o copy_mat.o addscaledmat.o  user_exit.o make_i.o allocmat.o initsoln.o initparams.o add_mat.o writesol.o readsol.o easysdp.o writeprob.o solvesys.o makefill.o mat_multsp.o norms.o linesearch.o matvec.o chol.o  qreig.o tweakgap.o freeprob.o packed.o  sortentries.o
	ar cr libsdp.a  readprob.o sdp.o op_o.o psd_feas.o op_a.o op_at.o  Fnorm.o calc_pobj.o calc_dobj.o trace_prod.o zero_mat.o mat_mult.o sym_mat.o copy_mat.o addscaledmat.o  user_exit.o make_i.o allocmat.o initsoln.o initparams.o add_mat.o writesol.o readsol.o easysdp.o writeprob.o solvesys.o makefill.o mat_multsp.o norms.o linesearch.o matvec.o chol.o qreig.o tweakgap.o freeprob.o packed.o sortentries.o
#
# On some systems, you might need to add after "ar cr libsdp.a ..."
#        ranlib libsdp.a
#
#
# To clean things up.
#
clean:
	rm -f *.o
	rm -f libsdp.a





