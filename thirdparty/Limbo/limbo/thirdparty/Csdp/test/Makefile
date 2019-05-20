#
# Currently, we have only two tests to complete.  The first is the solution
# of the SDPA format problem theta1.dat-s.  The makefile will run CSDP on 
# this problem and store the output in theta1.out.  The second test is the
# computation of the Lovasz theta number of the graph g50.  
#
# Once the tests have run, examine the .out files and compare them with
# theta1.correct and g50.correct.  The optimal values should agree to 
# six digits or more, and the DIMACS errors should all be smaller than
# 1.0e-6.  
#

all: theta1.out g50.out 
 
#
# Test the solver on theta1.dat-s
#

theta1.out: 
	../solver/csdp theta1.dat-s >theta1.out

#
# Test theta on the g50 graph.
#

g50.out: 
	../theta/theta g50 >g50.out

clean:
	rm -f *.out
