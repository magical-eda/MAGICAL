subckt inv I ZN VSS VDD
MU1_0_M_u2 (ZN I VSS VSS) nch l=40n w=600.0n m=1 nf=2 
MU1_1_M_u2 (ZN I VSS VSS) nch l=40n w=600.0n m=1 nf=2 
MU1_0_M_u3 (ZN I VDD VDD) pch l=40n w=800.0n m=1 nf=2 
MU1_1_M_u3 (ZN I VDD VDD) pch l=40n w=800.0n m=1 nf=2 
ends inv

subckt buff A1 A2 Z VSS VDD
I0 (A1 Z VSS VDD) inv
I1 (A2 Z VSS VDD) inv
ends buff
