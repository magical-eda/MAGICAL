subckt COMPARATOR_PRE_AMP_2018_Modify_test_flow CLK CROSSN CROSSP GND \
        INTERN INTERP OUTM OUTP VDD VI+ VI-
M0 (GND INTERN GND GND) nch_lvt_mac l=1u w=1.05u multi=1 nf=1 
M22 (GND INTERP GND GND) nch_lvt_mac l=1u w=1.05u multi=1 nf=1 
M16 (OUTM CROSSP GND GND) nch_lvt_mac l=40n w=1.44u multi=1 nf=3 
M17 (OUTP CROSSN GND GND) nch_lvt_mac l=40n w=1.44u multi=1 nf=3 
M4 (CROSSN CROSSP INTERN GND) nch_lvt_mac l=40n w=1.92u multi=1 nf=4 
M3 (CROSSP CROSSN INTERP GND) nch_lvt_mac l=40n w=1.92u multi=1 nf=4 
M7 (net050 CLK GND GND) nch_lvt_mac l=40n w=8.64u multi=1 nf=18 
M5 (INTERN VI+ net050 GND) nch_lvt_mac l=40n w=9.6u multi=1 nf=10 
M6 (INTERP VI- net050 GND) nch_lvt_mac l=40n w=9.6u multi=1 nf=10 
M8 (OUTM CROSSP VDD VDD) pch_lvt_mac l=40n w=2.88u multi=1 nf=3 
M18 (INTERN CLK VDD VDD) pch_lvt_mac l=40n w=1.92u multi=1 nf=2 
M15 (OUTP CROSSN VDD VDD) pch_lvt_mac l=40n w=2.88u multi=1 nf=3 
M19 (INTERP CLK VDD VDD) pch_lvt_mac l=40n w=1.92u multi=1 nf=2 
M10 (CROSSN CLK VDD VDD) pch_lvt_mac l=40n w=1.92u multi=1 nf=2 
M12 (CROSSP CLK VDD VDD) pch_lvt_mac l=40n w=1.92u multi=1 nf=2 
M14 (CROSSN CROSSP VDD VDD) pch_lvt_mac l=40n w=3.84u multi=1 nf=4 
M13 (CROSSP CROSSN VDD VDD) pch_lvt_mac l=40n w=3.84u multi=1 nf=4 
ends COMPARATOR_PRE_AMP_2018_Modify_test_flow
