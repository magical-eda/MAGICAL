topckt OTA_2 IBIAS VDD VIM VIP VOM VOP VSS

M20 (VDD IBIAS VDD VDD) pfet_lvt l=4u w=3u nf=1 

M18 (VDD IBIAS VDD VDD) pfet_lvt l=4u w=3u nf=1 

M13 (VOP VIM net100 net100) nfet_lvt l=200n w=1u nf=4 

M21 (VOM VIP net100 net100) nfet_lvt l=200n w=1u nf=4

M0 (IBIAS IBIAS VDD VDD) pfet l=200n w=1u nf=2 

M24 (IBIAS IBIAS VDD VDD) pfet l=200n w=1u nf=2 

M23 (VOP IBIAS VDD VDD) pfet l=200n w=2u nf=4 

M14 (VOM IBIAS VDD VDD) pfet l=200n w=2u nf=4 

C22 (VOP NTAIL2 VSS) cap w=100n s=100n stm=1 spm=6 ftip=200n dmflag=0 lr=5u nr=40

C21 (NTAIL2 VOM VSS) cap  w=100n s=100n stm=1 spm=6 ftip=200n dmflag=0 lr=5u nr=40

R11  (VOM NTAIL2 VSS ) res lr=8u wr=400n series=10 segspace=250n

R12  (NTAIL2 VOP VSS ) res lr=8u wr=400n series=10 segspace=250n

M22 (net100 NTAIL2 VSS VSS) nfet l=200n w=3u nf=4

M12 (VSS NTAIL2 VSS VSS) nfet_lvt l=3u w=2u nf=1

M11 (VSS NTAIL2 VSS VSS) nfet_lvt l=3u w=2u nf=1 

ends OTA_2
