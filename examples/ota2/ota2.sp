subckt OTA_2 GND IBIAS VCM VDD VIM VIP VOM VOP
    M36 (net0134 PCAS VDD VDD) pch_hvt_mac l=120.0n w=2.4u multi=1 nf=2 
    C7 (vtail VOP GND) cfmom nr=32 lr=2.7u w=70n s=70n stm=1 spm=6 multi=1 ftip=140.0n dmflag=0
    C6 (VOM vtail GND) cfmom nr=32 lr=2.7u w=70n s=70n stm=1 spm=6 multi=1 ftip=140.0n dmflag=0
    C5 (VOM net0101 GND) cfmom nr=32 lr=1.5u w=70n s=70n stm=1 spm=6 multi=1 ftip=140.0n dmflag=0
    C4 (net0101 VOP GND) cfmom nr=32 lr=1.5u w=70n s=70n stm=1 spm=6 multi=1 ftip=140.0n dmflag=0
    C3 (VIM net096 GND) cfmom nr=120 lr=18.0u w=70n s=70n stm=1 spm=6 multi=1 ftip=140.0n dmflag=0
    C2 (VIP net092 GND) cfmom nr=120 lr=18.0u w=70n s=70n stm=1 spm=6 multi=1 ftip=140.0n dmflag=0
    C1 (VO1P net0118 GND) cfmom nr=60 lr=8.2u w=70n s=70n stm=1 spm=6 multi=1 ftip=140.0n dmflag=0
    C0 (net0118 VO1M GND) cfmom nr=60 lr=8.2u w=70n s=70n stm=1 spm=6 multi=1 ftip=140.0n dmflag=0
    M33 (net0138 net077 VDD VDD) pch l=120.0n w=4.8u multi=1 nf=4 
    M32 (net0136 net077 VDD VDD) pch l=120.0n w=7.2u multi=1 nf=6 
    M21 (net0104 net0101 CMFBTAIL GND) nch_lvt l=120.0n w=5.4u multi=1 nf=6 
    M20 (vtail VCM CMFBTAIL GND) nch_lvt l=120.0n w=5.4u multi=1 nf=6 
    M18 (VO1P IBIAS net0131 GND) nch_lvt l=240.0n w=118.8u multi=1 nf=33 
    M17 (VO1M IBIAS net0133 GND) nch_lvt l=240.0n w=118.8u multi=1 nf=33 
    M7 (CMFBTAIL NBIAS_TAIL GND GND) nch_lvt l=120.0n w=11.7u multi=1 nf=13 
    M8 (net0133 VIP NTAIL GND) nch_lvt l=240.0n w=115.2u multi=1 nf=32 
    M9 (VOM net092 vs GND) nch_lvt l=120.0n w=108.000000u multi=1 nf=30 
    M10 (net0131 VIM NTAIL GND) nch_lvt l=240.0n w=115.2u multi=1 nf=32 
    M12 (VOP net096 vs GND) nch_lvt l=120.0n w=108.000000u multi=1 nf=30 
    M1 (NTAIL NBIAS_TAIL GND GND) nch_lvt l=120.0n w=93.6u multi=1 nf=104 
    M19 (IBIAS IBIAS NBIAS_TAIL GND) nch_lvt l=120.0n w=9.9u multi=1 nf=11 
    M16 (PCAS VCM VN1 GND) nch_lvt l=120.0n w=11.7u multi=1 nf=13 
    M15 (NBIAS_TAIL VCM VN2 GND) nch_lvt l=120.0n w=11.7u multi=1 nf=13 
    M14 (INCM2 INCM2 net0137 GND) nch_lvt l=240.0n w=1.2u multi=1 nf=2 
    M13 (net077 VCM net0135 GND) nch_lvt l=120.0n w=4.5u multi=1 nf=5 
    M3 (net0135 NBIAS_TAIL GND GND) nch_lvt l=120.0n w=4.5u multi=1 nf=5 
    M4 (VN2 NBIAS_TAIL GND GND) nch_lvt l=120.0n w=11.7u multi=1 nf=13 
    M2 (VN1 NBIAS_TAIL GND GND) nch_lvt l=120.0n w=11.7u multi=1 nf=13 
    M74 (PTAIL net0118 VDD VDD) pch_lvt l=120.0n w=48.0u multi=1 nf=40 
    M71 (VOP VO1M VDD VDD) pch_lvt l=120.0n w=48.0u multi=1 nf=20 
    M70 (VOM VO1P VDD VDD) pch_lvt l=120.0n w=48.0u multi=1 nf=20 
    M69 (net0104 net0104 VDD VDD) pch_lvt l=120.0n w=7.2u multi=1 nf=6 
    M68 (vtail net0104 VDD VDD) pch_lvt l=120.0n w=7.2u multi=1 nf=6 
    M35 (net0132 VIP PTAIL VDD) pch_lvt l=240.0n w=86.4u multi=1 nf=24 
    M34 (net0130 VIM PTAIL VDD) pch_lvt l=240.0n w=86.4u multi=1 nf=24 
    M27 (VO1P PCAS net0130 VDD) pch_lvt l=240.0n w=124.8u multi=1 nf=26 
    M26 (VO1M PCAS net0132 VDD) pch_lvt l=240.0n w=124.8u multi=1 nf=26 
    M28 (PCAS PCAS net0134 VDD) pch_lvt l=120.0n w=1.2u multi=1 nf=1 
    M25 (INCM2 PCAS net0138 VDD) pch_lvt l=120.0n w=4.8u multi=1 nf=4 
    M24 (net077 PCAS net0136 VDD) pch_lvt l=120.0n w=7.2u multi=1 nf=6 
    M50 (vs vtail GND GND) nch_hvt_mac l=120.0n w=144.000000u multi=1 nf=40 
    M6 (net0137 INCM2 GND GND) nch_hvt_mac l=240.0n w=1.2u multi=1 nf=2 

R11 (net0101 VOM GND ) rppolywo lr=3.6u wr=400n series=6 segspace=250n

R14 (VOP net0101 GND ) rppolywo lr=3.6u wr=400n series=6 segspace=250n

R5 (VO1M net0118 GND ) rppolywo lr=8.185u wr=400n series=17 segspace=250n

R12 (net0118 VO1P GND ) rppolywo lr=8.185u wr=400n series=17 segspace=250n

R13 (net096 INCM2 GND ) rppolywo lr=3u wr=400n series=12 segspace=250n

R0 (net092 INCM2 GND ) rppolywo lr=3u wr=400n series=12 segspace=250n


ends OTA_2

