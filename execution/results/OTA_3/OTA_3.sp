topckt OTA_3 AVDD AVSS IBIN IN IP ON OP VCAS VCMI VCMO
M35 (net036 IBIN AVDD AVDD) pfet_lvt l=200n w=8u m=1 nf=4 
M34 (CMFB VCMO net036 net036) pfet_lvt l=200n w=4u m=1 nf=2
M33 (net044 VCMI net036 net036) pfet_lvt l=200n w=4u m=1 nf=2 
M37 (OP IN net033 net033) pfet_lvt l=200n w=24u m=1 nf=12 
M23 (ON IP net033 net033) pfet_lvt l=200n w=24u m=1 nf=12 
M36 (net033 IBIN AVDD AVDD) pfet_lvt l=200n w=48.0u m=1 nf=24 
M16 (IBIN VCAS net041 net041) pfet_lvt l=200n w=8u m=1 nf=4 
M50 (on1 IP net025 net025) pfet_lvt l=200n w=8u m=1 nf=4 
M6 (net041 IBIN AVDD AVDD) pfet_lvt l=200n w=8u m=1 nf=4
M4 (net025 IBIN AVDD AVDD) pfet_lvt l=200n w=16.0u m=1 nf=8
M20 (op1 IN net025 net025) pfet_lvt l=200n w=8u m=1 nf=4 
M11 (AVSS on1 AVSS AVSS) nfet_lvt l=1u w=8u m=1 nf=4 
M10 (AVSS op1 AVSS AVSS) nfet_lvt l=1u w=8u m=1 nf=4 
M21 (ON op1 AVSS AVSS) nfet_lvt l=200n w=6u m=1 nf=6 
M19 (OP on1 AVSS AVSS) nfet_lvt l=200n w=6u m=1 nf=6 
M29 (CMFB CMFB AVSS AVSS) nfet_lvt l=200n w=1u m=1 nf=2 
M14 (op1 CMFB AVSS AVSS) nfet_lvt l=200n w=2u m=1 nf=2 
M13 (on1 CMFB AVSS AVSS) nfet_lvt l=200n w=2u m=1 nf=2 
M30 (net044 net044 AVSS AVSS) nfet_lvt l=200n w=1u m=1 nf=2 
C4 (ON VCMO AVSS) cap nr=24 lr=4u w=100n s=100n stm=2 spm=6 ftip=200n 
C5 (OP VCMO AVSS) cap nr=24 lr=4u w=100n s=100n stm=2 spm=6 ftip=200n 
ends TEST_OTA_FF_2s_v3_nodummy_schematic
