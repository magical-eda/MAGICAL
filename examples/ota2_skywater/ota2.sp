subckt OTA_2 GND IBIAS VCM VDD VIM VIP VOM VOP
    M36 (net0134 PCAS VDD VDD) pch l=200n w=2.4u multi=1 nf=2 
    C7 (vtail VOP) cap l=3u w=3u stm=3
    C6 (vtail VOM) cap l=3u w=3u stm=3
    C5 (VOM net0101) cap l=3u w=3u stm=3
    C4 (VOP net0101) cap l=3u w=3u stm=3
    C1 (VO1P net0118) cap l=8u w=6u stm=3
    C0 (VO1M net0118) cap l=8u w=6u stm=3
    M33 (net0138 net077 VDD VDD) pch l=200n w=4.8u multi=1 nf=4 
    M32 (net0136 net077 VDD VDD) pch l=200n w=7.2u multi=1 nf=6 
    M21 (net0104 net0101 CMFBTAIL GND) nch_lvt l=200n w=5.4u multi=1 nf=6 
    M20 (vtail VCM CMFBTAIL GND) nch_lvt l=200n w=5.4u multi=1 nf=6 
    M18 (VO1P IBIAS net0131 GND) nch_lvt l=240.0n w=118.8u multi=1 nf=33 
    M17 (VO1M IBIAS net0133 GND) nch_lvt l=240.0n w=118.8u multi=1 nf=33 
    M7 (CMFBTAIL NBIAS_TAIL GND GND) nch_lvt l=200n w=11.7u multi=1 nf=13 
    M8 (net0133 VIP NTAIL GND) nch_lvt l=240.0n w=115.2u multi=1 nf=32 
    M9 (VOM net092 vs GND) nch_lvt l=200n w=108.000000u multi=1 nf=30 
    M10 (net0131 VIM NTAIL GND) nch_lvt l=240.0n w=115.2u multi=1 nf=32 
    M12 (VOP net096 vs GND) nch_lvt l=200n w=108.000000u multi=1 nf=30 
    M1 (NTAIL NBIAS_TAIL GND GND) nch_lvt l=200n w=93.6u multi=1 nf=52
    M19 (IBIAS IBIAS NBIAS_TAIL GND) nch_lvt l=200n w=9.9u multi=1 nf=11 
    M16 (PCAS VCM VN1 GND) nch_lvt l=200n w=11.7u multi=1 nf=13 
    M15 (NBIAS_TAIL VCM VN2 GND) nch_lvt l=200n w=11.7u multi=1 nf=13 
    M14 (INCM2 INCM2 net0137 GND) nch_lvt l=240.0n w=1.2u multi=1 nf=2 
    M13 (net077 VCM net0135 GND) nch_lvt l=200n w=4.5u multi=1 nf=5 
    M3 (net0135 NBIAS_TAIL GND GND) nch_lvt l=200n w=4.5u multi=1 nf=5 
    M4 (VN2 NBIAS_TAIL GND GND) nch_lvt l=200n w=11.7u multi=1 nf=13 
    M2 (VN1 NBIAS_TAIL GND GND) nch_lvt l=200n w=11.7u multi=1 nf=13 
    M74 (PTAIL net0118 VDD VDD) pch l=200n w=48.0u multi=1 nf=40 
    M71 (VOP VO1M VDD VDD) pch l=200n w=48.0u multi=1 nf=20 
    M70 (VOM VO1P VDD VDD) pch l=200n w=48.0u multi=1 nf=20 
    M69 (net0104 net0104 VDD VDD) pch l=200n w=7.2u multi=1 nf=6 
    M68 (vtail net0104 VDD VDD) pch l=200n w=7.2u multi=1 nf=6 
    M35 (net0132 VIP PTAIL VDD) pch l=240.0n w=86.4u multi=1 nf=24 
    M34 (net0130 VIM PTAIL VDD) pch l=240.0n w=86.4u multi=1 nf=24 
    M27 (VO1P PCAS net0130 VDD) pch l=240.0n w=124.8u multi=1 nf=26 
    M26 (VO1M PCAS net0132 VDD) pch l=240.0n w=124.8u multi=1 nf=26 
    M28 (PCAS PCAS net0134 VDD) pch l=200n w=1.2u multi=1 nf=1 
    M25 (INCM2 PCAS net0138 VDD) pch l=200n w=4.8u multi=1 nf=4 
    M24 (net077 PCAS net0136 VDD) pch l=200n w=7.2u multi=1 nf=6 
    M50 (vs vtail GND GND) nch_lvt l=200n w=144.000000u multi=1 nf=40 
    M6 (net0137 INCM2 GND GND) nch_lvt l=240.0n w=1.2u multi=1 nf=2 
    R11 (net0101 VOM) rpoly lr=15u wr=0.4u series=6
    R14 (net0101 VOP) rpoly lr=15u wr=0.4u series=6
    R5 (VO1M net0118) rpoly lr=20u wr=0.4u series=18
    R12 (VO1P net0118) rpoly lr=20u wr=0.4u series=18
    R13 (net096 INCM2) rpoly lr=20u wr=0.4u series=10

ends OTA_2

