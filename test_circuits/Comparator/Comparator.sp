topckt Comparator CLK CROSSN CROSSP GND INTERN INTERP OUTM OUTP VDD VI+ VI-
M0 (GND INTERN GND GND) nfet_lvt l=1u w=1u nf=1
M22 (GND INTERP GND GND) nfet_lvt l=1u w=1u nf=1
M16 (OUTM CROSSP GND GND) nfet_lvt l=40n w=2u nf=3
M17 (OUTP CROSSN GND GND) nfet_lvt l=40n w=2u nf=3 
M4 (CROSSN CROSSP INTERN GND) nfet_lvt l=40n w=2u nf=4
M3 (CROSSP CROSSN INTERP GND) nfet_lvt l=40n w=2u nf=4 
M7 (net050 CLK GND GND) nfet_lvt l=40n w=10u nf=18 
M5 (INTERN VI+ net050 GND) nfet_lvt l=40n w=2u nf=10 
M6 (INTERP VI- net050 GND) nfet_lvt l=40n w=2u nf=10 
M8 (OUTM CROSSP VDD VDD) pfet_lvt l=40n w=2u nf=3 
M18 (INTERN CLK VDD VDD) pfet_lvt l=40n w=2u nf=2
M15 (OUTP CROSSN VDD VDD) pfet_lvt l=40n w=2u nf=3 
M19 (INTERP CLK VDD VDD) pfet_lvt l=40n w=2u nf=2 
M10 (CROSSN CLK VDD VDD) pfet_lvt l=40n w=2u nf=2 
M12 (CROSSP CLK VDD VDD) pfet_lvt l=40n w=2u nf=2 
M14 (CROSSN CROSSP VDD VDD) pfet_lvt l=40n w=4u nf=4 
M13 (CROSSP CROSSN VDD VDD) pfet_lvt l=40n w=4u nf=4 
ends Comparator

