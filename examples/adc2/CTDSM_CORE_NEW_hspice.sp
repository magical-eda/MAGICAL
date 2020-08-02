** Generated for: hspiceD
** Generated on: Mar 24 14:26:23 2020
** Design library name: 2019_CTDSM_MAGICAL_TAPEOUT
** Design cell name: CTDSM_CORE_NEW
** Design view name: schematic


** .TEMP 25.0
** .OPTION
** +    ARTIST=2
** +    INGOLD=2
** +    PARHIER=LOCAL
** +    PSF=2
** .LIB "/usr/local/packages/tsmc_40/pdk/models/hspice/toplevel.l" top_tt

** Library name: 2019_CTDSM_MAGICAL_TAPEOUT
** Cell name: Gm2_v5_Practice
** View name: schematic
.subckt Gm2_v5_Practice_schematic ibias vdd vim vip vom vop vss
xm20 vdd ibias vdd vdd pch_lvt_mac l=3.6e-6 w=2.8e-6 multi=1 nf=1 
xm18 vdd ibias vdd vdd pch_lvt_mac l=3.6e-6 w=2.8e-6 multi=1 nf=1 
xc0 ntail2 vop cfmom_2t nr=46 lr=1e-6 w=70e-9 s=70e-9 stm=2 spm=6 multi=1 ftip=140e-9
xc1 ntail2 vom cfmom_2t nr=46 lr=1e-6 w=70e-9 s=70e-9 stm=2 spm=6 multi=1 ftip=140e-9
xm0 ibias ibias vdd vdd pch_mac l=160e-9 w=700e-9 multi=1 nf=2 
xm24 ibias ibias vdd vdd pch_mac l=160e-9 w=700e-9 multi=1 nf=2 
xm23 vop ibias vdd vdd pch_mac l=160e-9 w=1.4e-6 multi=1 nf=4 
xm14 vom ibias vdd vdd pch_mac l=160e-9 w=1.4e-6 multi=1 nf=4 
**Series configuration of R11
xr11 vom ntail2 vss  rppolywo_m lr=6.6e-6 wr=400e-9 multi=1 m=1 series=9 segspace=250e-9
**End of R11

**Series configuration of R12
xr12 ntail2 vop vss  rppolywo_m lr=6.6e-6 wr=400e-9 multi=1 m=1 series=9 segspace=250e-9
**End of R12

xm22 net100 ntail2 vss vss nch_mac l=160e-9 w=3.12e-6 multi=1 nf=4 
xm12 vss ntail2 vss vss nch_lvt_mac l=2.2e-6 w=2.1e-6 multi=1 nf=1 
xm11 vss ntail2 vss vss nch_lvt_mac l=2.2e-6 w=2.1e-6 multi=1 nf=1 
xm13 vop vim net100 vss nch_lvt_mac l=160e-9 w=1.16e-6 multi=1 nf=4 
xm21 vom vip net100 vss nch_lvt_mac l=160e-9 w=1.16e-6 multi=1 nf=4 
.ends Gm2_v5_Practice_schematic
** End of subcircuit definition.


** Library name: 2019_CTDSM_MAGICAL_TAPEOUT
** Cell name: Gm1_v5_Practice
** View name: schematic
.subckt Gm1_v5_Practice_schematic ibias vdd vim vip vom vop vss
xm8 net08 ntail1 vss vss nch_hvt_mac l=120e-9 w=3.22e-6 multi=1 nf=4 
xm2 vdd ibias vdd vdd pch_lvt_mac l=3.3e-6 w=2.95e-6 multi=1 nf=1 
xm4 vdd ibias vdd vdd pch_lvt_mac l=3.3e-6 w=2.95e-6 multi=1 nf=1 
xm12 ibias ibias vdd vdd pch_lvt_mac l=120e-9 w=585e-9 multi=1 nf=1 
xm11 vom ibias vdd vdd pch_lvt_mac l=120e-9 w=2.34e-6 multi=1 nf=4 
xm15 ibias ibias vdd vdd pch_lvt_mac l=120e-9 w=585e-9 multi=1 nf=1 
xm14 vop ibias vdd vdd pch_lvt_mac l=120e-9 w=2.34e-6 multi=1 nf=4 
**Series configuration of R12
xr12 ntail1 vop vss  rppolywo_m lr=6.6e-6 wr=400e-9 multi=1 m=1 series=9 segspace=250e-9
**End of R12

**Series configuration of R11
xr11 vom ntail1 vss  rppolywo_m lr=6.6e-6 wr=400e-9 multi=1 m=1 series=9 segspace=250e-9
**End of R11

xm3 vss ntail1 vss vss nch_lvt_mac l=2.2e-6 w=2.5e-6 multi=1 nf=1 
xm0 vss ntail1 vss vss nch_lvt_mac l=2.2e-6 w=2.5e-6 multi=1 nf=1 
xm26 vop vim net08 vss nch_lvt_mac l=240e-9 w=3.4e-6 multi=1 nf=8 
xm27 vom vip net08 vss nch_lvt_mac l=240e-9 w=3.4e-6 multi=1 nf=8 
xc1 ntail1 vop cfmom_2t nr=46 lr=1e-6 w=70e-9 s=70e-9 stm=2 spm=6 multi=1 ftip=140e-9
xc0 ntail1 vom cfmom_2t nr=46 lr=1e-6 w=70e-9 s=70e-9 stm=2 spm=6 multi=1 ftip=140e-9
.ends Gm1_v5_Practice_schematic
** End of subcircuit definition.
** Library name: 2019_CTDSM_MAGICAL
** Cell name: DFCNQD2BWP_LVT
** View name: schematic
.subckt DFCNQD2BWP_LVT d cp cdn q vdd vss
x0 net63 cp vss vss nch_lvt_mac l=40e-9 w=155e-9 multi=1 nf=1 
xmi4 net61 net63 vss vss nch_lvt_mac l=40e-9 w=310e-9 multi=1 nf=1 
x1 net97 cdn net60 vss nch_lvt_mac l=40e-9 w=210e-9 multi=1 nf=1 
x2 net123 net95 vss vss nch_lvt_mac l=40e-9 w=150e-9 multi=1 nf=1 
xmi29 net49 net63 net17 vss nch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 
xmi15 net123 net81 net49 vss nch_lvt_mac l=40e-9 w=150e-9 multi=1 nf=1 
x3 net60 net49 vss vss nch_lvt_mac l=40e-9 w=210e-9 multi=1 nf=1 
x4 net97 cdn net21 vss nch_lvt_mac l=40e-9 w=210e-9 multi=1 nf=1 
x5 net81 net63 vss vss nch_lvt_mac l=40e-9 w=155e-9 multi=1 nf=1 
xmi5 net95 d net61 vss nch_lvt_mac l=40e-9 w=310e-9 multi=1 nf=1 
xmi49 net25 cdn vss vss nch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 
x6 net21 net49 vss vss nch_lvt_mac l=40e-9 w=210e-9 multi=1 nf=1 
xmi26 net17 net97 vss vss nch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 
xmi48 net13 net123 net25 vss nch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 
x7 q net97 vss vss nch_lvt_mac l=40e-9 w=310e-9 multi=1 nf=1 
x8 q net97 vss vss nch_lvt_mac l=40e-9 w=310e-9 multi=1 nf=1 
xmi47 net95 net81 net13 vss nch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 
xmi33 net80 net97 vdd vdd pch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 
x9 q net97 vdd vdd pch_lvt_mac l=40e-9 w=410e-9 multi=1 nf=1 
x10 net97 net49 vdd vdd pch_lvt_mac l=40e-9 w=370e-9 multi=1 nf=1 
xmi43 net101 net123 vdd vdd pch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 
xmi6 net95 d net120 vdd pch_lvt_mac l=40e-9 w=340e-9 multi=1 nf=1 
x11 q net97 vdd vdd pch_lvt_mac l=40e-9 w=410e-9 multi=1 nf=1 
x12 net97 net49 vdd vdd pch_lvt_mac l=40e-9 w=370e-9 multi=1 nf=1 
x13 net97 cdn vdd vdd pch_lvt_mac l=40e-9 w=370e-9 multi=1 nf=1 
xmi44 net101 cdn vdd vdd pch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 
x14 net97 cdn vdd vdd pch_lvt_mac l=40e-9 w=370e-9 multi=1 nf=1 
x15 net123 net95 vdd vdd pch_lvt_mac l=40e-9 w=180e-9 multi=1 nf=1 
x16 net63 cp vdd vdd pch_lvt_mac l=40e-9 w=205e-9 multi=1 nf=1 
xmi16 net123 net63 net49 vdd pch_lvt_mac l=40e-9 w=180e-9 multi=1 nf=1 
x17 net81 net63 vdd vdd pch_lvt_mac l=40e-9 w=205e-9 multi=1 nf=1 
xmi32 net49 net81 net80 vdd pch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 
xmi45 net95 net63 net101 vdd pch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 
xmi7 net120 net81 vdd vdd pch_lvt_mac l=40e-9 w=340e-9 multi=1 nf=1 
.ends DFCNQD2BWP_LVT
** End of subcircuit definition.

** Library name: 2019_CTDSM_MAGICAL_TAPEOUT
** Cell name: C_DAC_CTDSM_DEC2016
** View name: schematic
.subckt C_DAC_CTDSM_DEC2016_schematic clkb in r3 r4 rstb vdd vss
**Series configuration of R27
xr27 r3 net10 vss  rppolywo_m lr=19.92e-6 wr=1.2e-6 multi=1 m=1 series=18 segspace=250e-9
**End of R27

**Series configuration of R64
xr64 r4 in vss  rppolywo_m lr=18.67e-6 wr=1.2e-6 multi=1 m=1 series=4 segspace=250e-9
**End of R64

xi94 in clkb rstb net10 vdd vss DFCNQD2BWP_LVT
.ends C_DAC_CTDSM_DEC2016_schematic
** End of subcircuit definition.

** Library name: 2019_CTDSM_MAGICAL_TAPEOUT
** Cell name: DFCNQD2BWP_LVT
** View name: schematic
.subckt DFCNQD2BWP_LVT_schematic d cp cdn q vdd vss
x0 net63 cp vss vss nch_lvt_mac l=40e-9 w=155e-9 multi=1 nf=1 
xmi4 net61 net63 vss vss nch_lvt_mac l=40e-9 w=310e-9 multi=1 nf=1 
x1 net97 cdn net60 vss nch_lvt_mac l=40e-9 w=210e-9 multi=1 nf=1 
x2 net123 net95 vss vss nch_lvt_mac l=40e-9 w=150e-9 multi=1 nf=1 
xmi29 net49 net63 net17 vss nch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 
xmi15 net123 net81 net49 vss nch_lvt_mac l=40e-9 w=150e-9 multi=1 nf=1 
x3 net60 net49 vss vss nch_lvt_mac l=40e-9 w=210e-9 multi=1 nf=1 
x4 net97 cdn net21 vss nch_lvt_mac l=40e-9 w=210e-9 multi=1 nf=1 
x5 net81 net63 vss vss nch_lvt_mac l=40e-9 w=155e-9 multi=1 nf=1 
xmi5 net95 d net61 vss nch_lvt_mac l=40e-9 w=310e-9 multi=1 nf=1 
xmi49 net25 cdn vss vss nch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 
x6 net21 net49 vss vss nch_lvt_mac l=40e-9 w=210e-9 multi=1 nf=1 
xmi26 net17 net97 vss vss nch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 
xmi48 net13 net123 net25 vss nch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 
x7 q net97 vss vss nch_lvt_mac l=40e-9 w=310e-9 multi=1 nf=1 
x8 q net97 vss vss nch_lvt_mac l=40e-9 w=310e-9 multi=1 nf=1 
xmi47 net95 net81 net13 vss nch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 
xmi33 net80 net97 vdd vdd pch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 
x9 q net97 vdd vdd pch_lvt_mac l=40e-9 w=410e-9 multi=1 nf=1 
x10 net97 net49 vdd vdd pch_lvt_mac l=40e-9 w=370e-9 multi=1 nf=1 
xmi43 net101 net123 vdd vdd pch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 
xmi6 net95 d net120 vdd pch_lvt_mac l=40e-9 w=340e-9 multi=1 nf=1 
x11 q net97 vdd vdd pch_lvt_mac l=40e-9 w=410e-9 multi=1 nf=1 
x12 net97 net49 vdd vdd pch_lvt_mac l=40e-9 w=370e-9 multi=1 nf=1 
x13 net97 cdn vdd vdd pch_lvt_mac l=40e-9 w=370e-9 multi=1 nf=1 
xmi44 net101 cdn vdd vdd pch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 
x14 net97 cdn vdd vdd pch_lvt_mac l=40e-9 w=370e-9 multi=1 nf=1 
x15 net123 net95 vdd vdd pch_lvt_mac l=40e-9 w=180e-9 multi=1 nf=1 
x16 net63 cp vdd vdd pch_lvt_mac l=40e-9 w=205e-9 multi=1 nf=1 
xmi16 net123 net63 net49 vdd pch_lvt_mac l=40e-9 w=180e-9 multi=1 nf=1 
x17 net81 net63 vdd vdd pch_lvt_mac l=40e-9 w=205e-9 multi=1 nf=1 
xmi32 net49 net81 net80 vdd pch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 
xmi45 net95 net63 net101 vdd pch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 
xmi7 net120 net81 vdd vdd pch_lvt_mac l=40e-9 w=340e-9 multi=1 nf=1 
.ends DFCNQD2BWP_LVT_schematic
** End of subcircuit definition.

** Library name: 2019_CTDSM_MAGICAL_TAPEOUT
** Cell name: myComparator_v3_CTDSM_DEC2016
** View name: schematic
.subckt myComparator_v3_CTDSM_DEC2016_schematic clk gnd outm outp vdd _net0 _net1
xm0 gnd intern gnd gnd nch_lvt_mac l=1e-6 w=1.05e-6 multi=1 nf=1 
xm22 gnd interp gnd gnd nch_lvt_mac l=1e-6 w=1.05e-6 multi=1 nf=1 
xm16 outm crossp gnd gnd nch_lvt_mac l=40e-9 w=1.44e-6 multi=1 nf=4 
xm17 outp crossn gnd gnd nch_lvt_mac l=40e-9 w=1.44e-6 multi=1 nf=4 
xm4 crossn crossp intern gnd nch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 
xm3 crossp crossn interp gnd nch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 
xm7 net069 clk gnd gnd nch_lvt_mac l=40e-9 w=6.9e-6 multi=1 nf=15 
xm5 intern _net0 net069 gnd nch_lvt_mac l=40e-9 w=14.4e-6 multi=1 nf=15 
xm6 interp _net1 net069 gnd nch_lvt_mac l=40e-9 w=14.4e-6 multi=1 nf=15 
xm8 outm crossp vdd vdd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 
xm18 intern clk vdd vdd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 
xm15 outp crossn vdd vdd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 
xm2 interp clk vdd vdd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 
xm1 crossn clk vdd vdd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 
xm12 crossp clk vdd vdd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 
xm14 crossn crossp vdd vdd pch_lvt_mac l=40e-9 w=3.84e-6 multi=1 nf=8 
xm13 crossp crossn vdd vdd pch_lvt_mac l=40e-9 w=3.84e-6 multi=1 nf=8 
.ends myComparator_v3_CTDSM_DEC2016_schematic
** End of subcircuit definition.

** Library name: 2019_CTDSM_MAGICAL
** Cell name: NR2D8BWP_LVT_RESIZE
** View name: schematic
.subckt NR2D8BWP_LVT a1 a2 zn vdd vss
x0 zn a2 vss vss nch_lvt_mac l=80e-9 w=4.96e-6 multi=1 nf=2 
x1 zn a1 vss vss nch_lvt_mac l=80e-9 w=4.96e-6 multi=1 nf=2 
x2 net13 a2 vdd vdd pch_lvt_mac l=80e-9 w=6.56e-6 multi=1 nf=2 
x3 zn a1 net13 vdd pch_lvt_mac l=80e-9 w=6.56e-6 multi=1 nf=2 
.ends NR2D8BWP_LVT
** End of subcircuit definition.

** Library name: 2019_CTDSM_MAGICAL_TAPEOUT
** Cell name: SR_Latch_CTDSM_DEC2016
** View name: schematic
.subckt SR_Latch_LVT q qb r s vdd vss
xi1 r qb q vdd vss NR2D8BWP_LVT
xi0 s q qb vdd vss NR2D8BWP_LVT
.ends SR_Latch_LVT
** End of subcircuit definition.

.subckt SR_Latch_LVT_wrapper q qb r s vdd vss
x q qb r s vdd vss SR_Latch_LVT
.ends SR_Latch_LVT_wrapper

** Library name: tcbn40lpbwp
** Cell name: INVD2BWP
** View name: schematic
.subckt INVD4BWP_LVT i zn vdd vss
m0 zn i vss vss nch l=40e-9 w=310e-9 m=1 nf=1 
m1 zn i vss vss nch l=40e-9 w=310e-9 m=1 nf=1 
m2 zn i vdd vdd pch l=40e-9 w=410e-9 m=1 nf=1 
m3 zn i vdd vdd pch l=40e-9 w=410e-9 m=1 nf=1 
.ends INVD4BWP_LVT
** End of subcircuit definition.

** Library name: 2019_CTDSM_MAGICAL_TAPEOUT
** Cell name: C2_BANK
** View name: schematic
.subckt C2_BANK a b vss d_4_ d_3_ d_2_ d_1_ vdd
xm17 a d_3_ net027 vss nch_lvt_mac l=40e-9 w=6e-6 multi=1 nf=12 
xm15 net062 db_3_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 
xm13 b d_3_ net062 vss nch_lvt_mac l=40e-9 w=6e-6 multi=1 nf=12 
xm12 net027 db_3_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 
xm10 b d_2_ net063 vss nch_lvt_mac l=40e-9 w=3e-6 multi=1 nf=6 
xm8 net063 db_2_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 
xm20 net061 db_4_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 
xm18 a d_4_ net026 vss nch_lvt_mac l=40e-9 w=12e-6 multi=1 nf=24 
xm23 net026 db_4_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 
xm11 net028 db_2_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 
xm6 a d_2_ net028 vss nch_lvt_mac l=40e-9 w=3e-6 multi=1 nf=6 
xm2 b d_1_ net041 vss nch_lvt_mac l=40e-9 w=1.5e-6 multi=1 nf=3 
xm4 net029 db_1_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 
xm5 net041 db_1_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 
xm0 a d_1_ net029 vss nch_lvt_mac l=40e-9 w=1.5e-6 multi=1 nf=3 
xm22 b d_4_ net061 vss nch_lvt_mac l=40e-9 w=12e-6 multi=1 nf=24 
xc14 net029 net041 cfmom_2t nr=14 lr=13.6e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc11 a b cfmom_2t nr=270 lr=21.02e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc16_3_ net027 net062 cfmom_2t nr=14 lr=13.6e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc16_2_ net027 net062 cfmom_2t nr=14 lr=13.6e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc16_1_ net027 net062 cfmom_2t nr=14 lr=13.6e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc16_0_ net027 net062 cfmom_2t nr=14 lr=13.6e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc15_1_ net028 net063 cfmom_2t nr=14 lr=13.6e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc15_0_ net028 net063 cfmom_2t nr=14 lr=13.6e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc17_7_ net026 net061 cfmom_2t nr=14 lr=13.6e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc17_6_ net026 net061 cfmom_2t nr=14 lr=13.6e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc17_5_ net026 net061 cfmom_2t nr=14 lr=13.6e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc17_4_ net026 net061 cfmom_2t nr=14 lr=13.6e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc17_3_ net026 net061 cfmom_2t nr=14 lr=13.6e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc17_2_ net026 net061 cfmom_2t nr=14 lr=13.6e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc17_1_ net026 net061 cfmom_2t nr=14 lr=13.6e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc17_0_ net026 net061 cfmom_2t nr=14 lr=13.6e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xm16 net027 db_3_ a vdd pch_lvt_mac l=40e-9 w=12e-6 multi=1 nf=12 
xm14 net062 db_3_ b vdd pch_lvt_mac l=40e-9 w=12e-6 multi=1 nf=12 
xm9 net063 db_2_ b vdd pch_lvt_mac l=40e-9 w=6e-6 multi=1 nf=6 
xm7 net028 db_2_ a vdd pch_lvt_mac l=40e-9 w=6e-6 multi=1 nf=6 
xm19 net026 db_4_ a vdd pch_lvt_mac l=40e-9 w=24e-6 multi=1 nf=24 
xm1 net029 db_1_ a vdd pch_lvt_mac l=40e-9 w=3e-6 multi=1 nf=3 
xm21 net061 db_4_ b vdd pch_lvt_mac l=40e-9 w=24e-6 multi=1 nf=24 
xm3 net041 db_1_ b vdd pch_lvt_mac l=40e-9 w=3e-6 multi=1 nf=3 
xi0_4_ d_4_ db_4_ vdd vss INVD4BWP_LVT
xi0_3_ d_3_ db_3_ vdd vss INVD4BWP_LVT
xi0_2_ d_2_ db_2_ vdd vss INVD4BWP_LVT
xi0_1_ d_1_ db_1_ vdd vss INVD4BWP_LVT
.ends C2_BANK
** End of subcircuit definition.

** Library name: 2019_CTDSM_MAGICAL_TAPEOUT
** Cell name: C1_BANK
** View name: schematic
.subckt C1_BANK a b vss d_4_ d_3_ d_2_ d_1_ vdd
xm17 a d_3_ net027 vss nch_lvt_mac l=40e-9 w=40e-6 multi=1 nf=80 
xm15 net062 db_3_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 
xm13 b d_3_ net062 vss nch_lvt_mac l=40e-9 w=40e-6 multi=1 nf=80 
xm12 net027 db_3_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 
xm10 b d_2_ net063 vss nch_lvt_mac l=40e-9 w=20e-6 multi=1 nf=40 
xm8 net063 db_2_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 
xm20 net061 db_4_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 
xm18 a d_4_ net026 vss nch_lvt_mac l=40e-9 w=80e-6 multi=1 nf=160 
xm23 net026 db_4_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 
xm11 net028 db_2_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 
xm6 a d_2_ net028 vss nch_lvt_mac l=40e-9 w=20e-6 multi=1 nf=40 
xm2 b d_1_ net041 vss nch_lvt_mac l=40e-9 w=10e-6 multi=1 nf=20 
xm4 net029 db_1_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 
xm5 net041 db_1_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 
xm0 a d_1_ net029 vss nch_lvt_mac l=40e-9 w=10e-6 multi=1 nf=20 
xm22 b d_4_ net061 vss nch_lvt_mac l=40e-9 w=80e-6 multi=1 nf=160 
xc16_7_ net026 net061 cfmom_2t nr=94 lr=10e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc16_6_ net026 net061 cfmom_2t nr=94 lr=10e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc16_5_ net026 net061 cfmom_2t nr=94 lr=10e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc16_4_ net026 net061 cfmom_2t nr=94 lr=10e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc16_3_ net026 net061 cfmom_2t nr=94 lr=10e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc16_2_ net026 net061 cfmom_2t nr=94 lr=10e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc16_1_ net026 net061 cfmom_2t nr=94 lr=10e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc16_0_ net026 net061 cfmom_2t nr=94 lr=10e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc15_3_ net027 net062 cfmom_2t nr=94 lr=10e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc15_2_ net027 net062 cfmom_2t nr=94 lr=10e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc15_1_ net027 net062 cfmom_2t nr=94 lr=10e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc15_0_ net027 net062 cfmom_2t nr=94 lr=10e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc14_1_ net028 net063 cfmom_2t nr=94 lr=10e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc14_0_ net028 net063 cfmom_2t nr=94 lr=10e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc13 net029 net041 cfmom_2t nr=94 lr=10e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc1_3_ a b cfmom_2t nr=210 lr=35.7e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc1_2_ a b cfmom_2t nr=210 lr=35.7e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc1_1_ a b cfmom_2t nr=210 lr=35.7e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc1_0_ a b cfmom_2t nr=210 lr=35.7e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xm16 net027 db_3_ a vdd pch_lvt_mac l=40e-9 w=80e-6 multi=1 nf=80 
xm14 net062 db_3_ b vdd pch_lvt_mac l=40e-9 w=80e-6 multi=1 nf=80 
xm9 net063 db_2_ b vdd pch_lvt_mac l=40e-9 w=40e-6 multi=1 nf=40 
xm7 net028 db_2_ a vdd pch_lvt_mac l=40e-9 w=40e-6 multi=1 nf=40 
xm19 net026 db_4_ a vdd pch_lvt_mac l=40e-9 w=160e-6 multi=1 nf=160 
xm1 net029 db_1_ a vdd pch_lvt_mac l=40e-9 w=20e-6 multi=1 nf=20 
xm21 net061 db_4_ b vdd pch_lvt_mac l=40e-9 w=160e-6 multi=1 nf=160 
xm3 net041 db_1_ b vdd pch_lvt_mac l=40e-9 w=20e-6 multi=1 nf=20 
xi0_4_ d_4_ db_4_ vdd vss INVD4BWP_LVT
xi0_3_ d_3_ db_3_ vdd vss INVD4BWP_LVT
xi0_2_ d_2_ db_2_ vdd vss INVD4BWP_LVT
xi0_1_ d_1_ db_1_ vdd vss INVD4BWP_LVT
.ends C1_BANK
** End of subcircuit definition.

** Library name: 2019_CTDSM_MAGICAL_TAPEOUT
** Cell name: FIR_DAC_CTDSM_DEC2016
** View name: schematic
** End of subcircuit definition.

.subckt DIGITAL_TOP clk vssd vddd vo3p vo3m outm outp
xi146 clk vss net072 net071 vddd vo3p vo3m myComparator_v3_CTDSM_DEC2016_schematic
xi128 outm outp net072 net071 vddd vssd SR_Latch_LVT
.ends DIGITAL_TOP


.subckt DIGITAL_TOP_flat clk vssd vddd vo3p vo3m outm outp
xi128 outm outp net072 net071 vddd vssd SR_Latch_LVT
xm0 vssd intern vssd vssd nch_lvt_mac l=1e-6 w=1.05e-6 multi=1 nf=1 
xm22 vssd interp vssd vssd nch_lvt_mac l=1e-6 w=1.05e-6 multi=1 nf=1 
xm16 net072 crossp vssd vssd nch_lvt_mac l=40e-9 w=1.44e-6 multi=1 nf=4 
xm17 net071 crossn vssd vssd nch_lvt_mac l=40e-9 w=1.44e-6 multi=1 nf=4 
xm4 crossn crossp intern vssd nch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 
xm3 crossp crossn interp vssd nch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 
xm7 net069 clk vssd vssd nch_lvt_mac l=40e-9 w=6.9e-6 multi=1 nf=15 
xm5 intern vo3p net069 vssd nch_lvt_mac l=40e-9 w=14.4e-6 multi=1 nf=15 
xm6 interp vo3m net069 vssd nch_lvt_mac l=40e-9 w=14.4e-6 multi=1 nf=15 
xm8 net072 crossp vddd vddd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 
xm18 intern clk vddd vddd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 
xm15 net071 crossn vddd vddd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 
xm2 interp clk vddd vddd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 
xm1 crossn clk vddd vddd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 
xm12 crossp clk vddd vddd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 
xm14 crossn crossp vddd vddd pch_lvt_mac l=40e-9 w=3.84e-6 multi=1 nf=8 
xm13 crossp crossn vddd vddd pch_lvt_mac l=40e-9 w=3.84e-6 multi=1 nf=8 
.ends DIGITAL_TOP_flat


.subckt INPUT_RES vss vi vo
xr16  vi vo vss   rppolywo_m lr=19.92e-6 wr=1.2e-6 multi=1 m=1 para=2 segspace=250e-9
.ends INPUT_RES

.subckt wrapper vss clkb1 clkb2
m1 vss clkb2 vss vss nch l=280e-9 w=280e-9 m=1 nf=1 
m0 vss clkb1 vss vss nch l=280e-9 w=280e-9 m=1 nf=1 
.ends wrapper

.subckt DAC3 clkb in r3 r4 rstb vdd vss out
xr27 r3 net10 vss  rppolywo_m lr=19.92e-6 wr=1.2e-6 multi=1 m=1 series=18 segspace=250e-9
xr64 r4 in vss  rppolywo_m lr=18.67e-6 wr=1.2e-6 multi=1 m=1 series=4 segspace=250e-9
xi94 in clkb rstb net10 vdd vss DFCNQD2BWP_LVT
xi97 out clkb rstb in vdd vss DFCNQD2BWP_LVT
.ends DAC3

.subckt DAC1 clk in r1 r2 rstb vdd vss out
xr19 net3 r1 vss   rppolywo_m lr=19.92e-6 wr=1.2e-6 multi=1 m=1 series=1 segspace=250e-9
xr48 in r2 vss   rppolywo_m lr=19.92e-6 wr=1.2e-6 multi=1 m=1 series=1 segspace=250e-9
xi86 in clk rstb net3 vdd vss DFCNQD2BWP_LVT
xi88 out clk rstb in vdd vss DFCNQD2BWP_LVT
.ends DAC1

.subckt CAP2_RES2 vo2m vo2p vss
xc0 net074 net073 cfmom_2t nr=210 lr=34e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xr51  vo2m net073 vss   rppolywo_m lr=19.92e-6 wr=1.2e-6 multi=1 m=1 para=5 segspace=250e-9
xr25  vo2p net074 vss   rppolywo_m lr=19.92e-6 wr=1.2e-6 multi=1 m=1 para=5 segspace=250e-9
.ends CAP2_RES2

.subckt CAP1 vo1p vo1m
xc1_3_ vo1p vo1m cfmom_2t nr=260 lr=36e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc1_2_ vo1p vo1m cfmom_2t nr=260 lr=36e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc1_1_ vo1p vo1m cfmom_2t nr=260 lr=36e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc1_0_ vo1p vo1m cfmom_2t nr=260 lr=36e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
.ends CAP1

** End of subcircuit definition.
** Library name: 2019_CTDSM_MAGICAL_TAPEOUT
** Cell name: CTDSM_CORE_NEW
** View name: schematic
.subckt CTDSM_CORE_NEW_schematic clk clkb1 clkb2 ibias1 ibias2 outm outp rstb vdda vim vip vssa vddd vssd vo1m vo1p vo2m vo2p vo3m vo3p
ota2 ibias2 vdda vo2m vo2p vo3p vo3m vssa Gm2_v5_Practice_schematic
ota1 ibias1 vdda vo1m vo1p vo2p vo2m vssa Gm1_v5_Practice_schematic
dac3b clkb2 net063 vo3p vo3m rstb vdda vssa outm DAC3
dac3a clkb1 net062 vo3m vo3p rstb vdda vssa outp DAC3
digital0 clk vssd vddd vo3p vo3m outm outp DIGITAL_TOP_flat
** xi168 vo3p vo3m vss d3_4_ d3_3_ d3_2_ d3_1_ vdda C2_BANK
** xi167 net074 net073 vss d2_4_ d2_3_ d2_2_ d2_1_ vdda C2_BANK
** xi166 vo1p vo1m vss d1_4_ d1_3_ d1_2_ d1_1_ vdda C1_BANK
cap1 vo1p vo1m CAP1
cap2_res2 vo2m vo2p vssa CAP2_RES2
cap3 vo3p vo3m cfmom_2t nr=220 lr=34e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
dac1b clk net051 vo1m vo1m rstb vdda vssa net063 DAC1
dac1a clk net052 vo1p vo1p rstb vdda vssa net062 DAC1
w1 vssa clkb1 clkb2 wrapper
input_resp vssa vip vo1p INPUT_RES
input_resm vssa vim vo1m INPUT_RES
.ends CTDSM_CORE_NEW_schematic
** End of subcircuit definition.
** .END
