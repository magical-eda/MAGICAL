** Generated for: hspiceD
** Generated on: Nov 12 20:24:11 2019
** Design library name: CTDSM_TOP
** Design cell name: CTDSM_TOP
** Design view name: schematic

.subckt INPUT_RES VINP VINN OTA1_INP OTA1_INN GND
xr13 OTA1_INP VINP GND  rppolywo_m lr=10e-6 wr=600e-9 multi=1 m=1 series=5 segspace=250e-9
xr14 VINN OTA1_INN GND  rppolywo_m lr=10e-6 wr=600e-9 multi=1 m=1 series=5 segspace=250e-9
.ends INPUT_RES


** .TEMP 25.0
** .OPTION
** +    ARTIST=2
** +    INGOLD=2
** +    PARHIER=LOCAL
** +    PSF=2
** .LIB "/usr/local/packages/tsmc_40/pdk/models/hspice/toplevel.l" top_tt

** Library name: STDLIB
** Cell name: INVD4BWP_LVT
** View name: schematic
** terminal mapping: I	= i
**                   ZN	= zn
**                   VDD	= vdd
**                   VSS	= vss
.subckt INVD4BWP_LVT i zn vdd vss

** nch_lvt_mac Instance MU1_0-M_u2 = hspiceD device x0
x0 zn i vss vss nch_lvt_mac l=80e-9 w=620e-9 multi=1 nf=1 

** nch_lvt_mac Instance MU1_3-M_u2 = hspiceD device x1
x1 zn i vss vss nch_lvt_mac l=80e-9 w=620e-9 multi=1 nf=1 

** nch_lvt_mac Instance MU1_1-M_u2 = hspiceD device x2
x2 zn i vss vss nch_lvt_mac l=80e-9 w=620e-9 multi=1 nf=1 

** nch_lvt_mac Instance MU1_2-M_u2 = hspiceD device x3
x3 zn i vss vss nch_lvt_mac l=80e-9 w=620e-9 multi=1 nf=1 

** pch_lvt_mac Instance MU1_0-M_u3 = hspiceD device x4
x4 zn i vdd vdd pch_lvt_mac l=80e-9 w=820e-9 multi=1 nf=1 

** pch_lvt_mac Instance MU1_1-M_u3 = hspiceD device x5
x5 zn i vdd vdd pch_lvt_mac l=80e-9 w=820e-9 multi=1 nf=1 

** pch_lvt_mac Instance MU1_3-M_u3 = hspiceD device x6
x6 zn i vdd vdd pch_lvt_mac l=80e-9 w=820e-9 multi=1 nf=1 

** pch_lvt_mac Instance MU1_2-M_u3 = hspiceD device x7
x7 zn i vdd vdd pch_lvt_mac l=80e-9 w=820e-9 multi=1 nf=1 
.ends INVD4BWP_LVT
** End of subcircuit definition.

** Library name: STDLIB
** Cell name: DFCND4BWP_LVT
** View name: schematic
** terminal mapping: D	= d
**                   CP	= cp
**                   CDN	= cdn
**                   Q	= q
**                   QN	= qn
**                   VDD	= vdd
**                   VSS	= vss
.subckt DFCND4BWP_LVT_stupid d cp q qn vdd vss

** pch_lvt_mac Instance MI14-M_u3 = hspiceD device x0
x0 net175 net149 vdd vdd pch_lvt_mac l=80e-9 w=820e-9 multi=1 nf=1 

** pch_lvt_mac Instance MI22-M_u3 = hspiceD device x1
x1 q net149 vdd vdd pch_lvt_mac l=80e-9 w=820e-9 multi=1 nf=1 

** pch_lvt_mac Instance MI28-M_u3 = hspiceD device x2
x2 qn net175 vdd vdd pch_lvt_mac l=80e-9 w=820e-9 multi=1 nf=1 

** pch_lvt_mac Instance MI23-M_u3 = hspiceD device x3
x3 qn net175 vdd vdd pch_lvt_mac l=80e-9 w=820e-9 multi=1 nf=1 

** pch_lvt_mac Instance MI43 = hspiceD device xmi43
xmi43 net12 net145 vdd vdd pch_lvt_mac l=80e-9 w=240e-9 multi=1 nf=1 

** pch_lvt_mac Instance MI39-M_u3 = hspiceD device x4
x4 net95 net11 vdd vdd pch_lvt_mac l=80e-9 w=820e-9 multi=1 nf=1 

** pch_lvt_mac Instance MI6 = hspiceD device xmi6
xmi6 net9 d net1 vdd pch_lvt_mac l=80e-9 w=680e-9 multi=1 nf=1 

** pch_lvt_mac Instance MI26-M_u3 = hspiceD device x5
x5 qn net175 vdd vdd pch_lvt_mac l=80e-9 w=820e-9 multi=1 nf=1 

** pch_lvt_mac Instance MI29-M_u3 = hspiceD device x6
x6 qn net175 vdd vdd pch_lvt_mac l=80e-9 w=820e-9 multi=1 nf=1 

** pch_lvt_mac Instance MI31-M_u3 = hspiceD device x7
x7 net11 cp vdd vdd pch_lvt_mac l=80e-9 w=820e-9 multi=1 nf=1 

** pch_lvt_mac Instance MI27-M_u3 = hspiceD device x8
x8 q net149 vdd vdd pch_lvt_mac l=80e-9 w=820e-9 multi=1 nf=1 

** pch_lvt_mac Instance MI36-M_u2 = hspiceD device x9
x9 net149 cdn vdd vdd pch_lvt_mac l=80e-9 w=820e-9 multi=1 nf=1 

** pch_lvt_mac Instance MI44 = hspiceD device xmi44
xmi44 net12 cdn vdd vdd pch_lvt_mac l=80e-9 w=240e-9 multi=1 nf=1 

** pch_lvt_mac Instance MI17 = hspiceD device xmi17
xmi17 net175 net95 net24 vdd pch_lvt_mac l=80e-9 w=240e-9 multi=1 nf=1 

** pch_lvt_mac Instance MI36-M_u1 = hspiceD device x10
x10 net149 net24 vdd vdd pch_lvt_mac l=80e-9 w=820e-9 multi=1 nf=1 

** pch_lvt_mac Instance MI13-M_u3 = hspiceD device x11
x11 net145 net9 vdd vdd pch_lvt_mac l=80e-9 w=360e-9 multi=1 nf=1 

** pch_lvt_mac Instance MI24-M_u3 = hspiceD device x12
x12 q net149 vdd vdd pch_lvt_mac l=80e-9 w=820e-9 multi=1 nf=1 

** pch_lvt_mac Instance MI16 = hspiceD device xmi16
xmi16 net145 net11 net24 vdd pch_lvt_mac l=80e-9 w=360e-9 multi=1 nf=1 

** pch_lvt_mac Instance MI30-M_u1 = hspiceD device x13
x13 net149 net24 vdd vdd pch_lvt_mac l=80e-9 w=560e-9 multi=1 nf=1 

** pch_lvt_mac Instance MI30-M_u2 = hspiceD device x14
x14 net149 cdn vdd vdd pch_lvt_mac l=80e-9 w=560e-9 multi=1 nf=1 

** pch_lvt_mac Instance MI45 = hspiceD device xmi45
xmi45 net9 net11 net12 vdd pch_lvt_mac l=80e-9 w=240e-9 multi=1 nf=1 

** pch_lvt_mac Instance MI25-M_u3 = hspiceD device x15
x15 q net149 vdd vdd pch_lvt_mac l=80e-9 w=820e-9 multi=1 nf=1 

** pch_lvt_mac Instance MI7 = hspiceD device xmi7
xmi7 net1 net95 vdd vdd pch_lvt_mac l=80e-9 w=680e-9 multi=1 nf=1 

** nch_lvt_mac Instance MI26-M_u2 = hspiceD device x16
x16 qn net175 vss vss nch_lvt_mac l=80e-9 w=620e-9 multi=1 nf=1 

** nch_lvt_mac Instance MI24-M_u2 = hspiceD device x17
x17 q net149 vss vss nch_lvt_mac l=80e-9 w=620e-9 multi=1 nf=1 

** nch_lvt_mac Instance MI29-M_u2 = hspiceD device x18
x18 qn net175 vss vss nch_lvt_mac l=80e-9 w=620e-9 multi=1 nf=1 

** nch_lvt_mac Instance MI30-M_u4 = hspiceD device x19
x19 net169 cdn vss vss nch_lvt_mac l=80e-9 w=400e-9 multi=1 nf=1 

** nch_lvt_mac Instance MI4 = hspiceD device xmi4
xmi4 net128 net11 vss vss nch_lvt_mac l=80e-9 w=620e-9 multi=1 nf=1 

** nch_lvt_mac Instance MI23-M_u2 = hspiceD device x20
x20 qn net175 vss vss nch_lvt_mac l=80e-9 w=620e-9 multi=1 nf=1 

** nch_lvt_mac Instance MI18 = hspiceD device xmi18
xmi18 net175 net11 net24 vss nch_lvt_mac l=80e-9 w=240e-9 multi=1 nf=1 

** nch_lvt_mac Instance MI13-M_u2 = hspiceD device x21
x21 net145 net9 vss vss nch_lvt_mac l=80e-9 w=300e-9 multi=1 nf=1 

** nch_lvt_mac Instance MI30-M_u3 = hspiceD device x22
x22 net149 net24 net169 vss nch_lvt_mac l=80e-9 w=400e-9 multi=1 nf=1 

** nch_lvt_mac Instance MI15 = hspiceD device xmi15
xmi15 net145 net95 net24 vss nch_lvt_mac l=80e-9 w=300e-9 multi=1 nf=1 

** nch_lvt_mac Instance MI14-M_u2 = hspiceD device x23
x23 net175 net149 vss vss nch_lvt_mac l=80e-9 w=380e-9 multi=1 nf=1 

** nch_lvt_mac Instance MI25-M_u2 = hspiceD device x24
x24 q net149 vss vss nch_lvt_mac l=80e-9 w=620e-9 multi=1 nf=1 

** nch_lvt_mac Instance MI39-M_u2 = hspiceD device x25
x25 net95 net11 vss vss nch_lvt_mac l=80e-9 w=620e-9 multi=1 nf=1 

** nch_lvt_mac Instance MI36-M_u3 = hspiceD device x26
x26 net149 net24 net132 vss nch_lvt_mac l=80e-9 w=400e-9 multi=1 nf=1 

** nch_lvt_mac Instance MI5 = hspiceD device xmi5
xmi5 net9 d net128 vss nch_lvt_mac l=80e-9 w=620e-9 multi=1 nf=1 

** nch_lvt_mac Instance MI31-M_u2 = hspiceD device x27
x27 net11 cp vss vss nch_lvt_mac l=80e-9 w=620e-9 multi=1 nf=1 

** nch_lvt_mac Instance MI49 = hspiceD device xmi49
xmi49 net112 cdn vss vss nch_lvt_mac l=80e-9 w=240e-9 multi=1 nf=1 

** nch_lvt_mac Instance MI36-M_u4 = hspiceD device x28
x28 net132 cdn vss vss nch_lvt_mac l=80e-9 w=400e-9 multi=1 nf=1 

** nch_lvt_mac Instance MI48 = hspiceD device xmi48
xmi48 net96 net145 net112 vss nch_lvt_mac l=80e-9 w=240e-9 multi=1 nf=1 

** nch_lvt_mac Instance MI27-M_u2 = hspiceD device x29
x29 q net149 vss vss nch_lvt_mac l=80e-9 w=620e-9 multi=1 nf=1 

** nch_lvt_mac Instance MI28-M_u2 = hspiceD device x30
x30 qn net175 vss vss nch_lvt_mac l=80e-9 w=620e-9 multi=1 nf=1 

** nch_lvt_mac Instance MI22-M_u2 = hspiceD device x31
x31 q net149 vss vss nch_lvt_mac l=80e-9 w=620e-9 multi=1 nf=1 

** nch_lvt_mac Instance MI47 = hspiceD device xmi47
xmi47 net9 net95 net96 vss nch_lvt_mac l=80e-9 w=240e-9 multi=1 nf=1 
.ends DFCND4BWP_LVT_stupid
** End of subcircuit definition.

** Library name: CTDSM_TOP
** Cell name: OTA_XT_MAGICAL
** View name: schematic
** terminal mapping: GND	= gnd
**                   NCAS	= ncas
**                   VCM	= vcm
**                   VDD	= vdd
**                   VIM	= vim
**                   VIP	= vip
**                   VOM	= vom
**                   VOP	= vop
.subckt OTA_XT_MAGICAL gnd ncas vcm vdd vim vip vom vop

** nch_hvt_mac Instance M29 = hspiceD device xm29
xm29 vs vcmon gnd gnd nch_hvt_mac l=120e-9 w=40.5e-6 multi=1 nf=15 

** nch_lvt_mac Instance M5 = hspiceD device xm5
xm5 pcas vcm bias2 gnd nch_lvt_mac l=120e-9 w=4.8e-6 multi=1 nf=5 

** nch_lvt_mac Instance M30 = hspiceD device xm30
xm30 tail1 nbias_tail gnd gnd nch_lvt_mac l=120e-9 w=20e-6 multi=1 nf=25 

** nch_lvt_mac Instance M53 = hspiceD device xm53
xm53 vcmop net0108 vs2 gnd nch_lvt_mac l=120e-9 w=7.2e-6 multi=1 nf=8 

** nch_lvt_mac Instance M12 = hspiceD device xm12
xm12 vcmon vcm vs2 gnd nch_lvt_mac l=120e-9 w=7.2e-6 multi=1 nf=8 

** nch_lvt_mac Instance M50 = hspiceD device xm50
xm50 vo1p ncas casn gnd nch_lvt_mac l=120e-9 w=9e-6 multi=1 nf=10 

** nch_lvt_mac Instance M49 = hspiceD device xm49
xm49 vo1m ncas casp gnd nch_lvt_mac l=120e-9 w=9e-6 multi=1 nf=10 

** nch_lvt_mac Instance M51 = hspiceD device xm51
xm51 ncas ncas nbias_tail gnd nch_lvt_mac l=120e-9 w=4e-6 multi=1 nf=5 

** nch_lvt_mac Instance M47 = hspiceD device xm47
xm47 nbias_tail vcm bias1 gnd nch_lvt_mac l=120e-9 w=4.8e-6 multi=1 nf=5 

** nch_lvt_mac Instance M38 = hspiceD device xm38
xm38 bias1 nbias_tail gnd gnd nch_lvt_mac l=120e-9 w=4e-6 multi=1 nf=5 

** nch_lvt_mac Instance M7 = hspiceD device xm7
xm7 vop vim vs gnd nch_lvt_mac l=120e-9 w=36e-6 multi=1 nf=15 

** nch_lvt_mac Instance M43 = hspiceD device xm43
xm43 casn vim tail1 gnd nch_lvt_mac l=120e-9 w=13.5e-6 multi=1 nf=5 

** nch_lvt_mac Instance M10 = hspiceD device xm10
xm10 vom vip vs gnd nch_lvt_mac l=120e-9 w=36e-6 multi=1 nf=15 

** nch_lvt_mac Instance M40 = hspiceD device xm40
xm40 casp vip tail1 gnd nch_lvt_mac l=120e-9 w=13.5e-6 multi=1 nf=5 

** nch_lvt_mac Instance M41 = hspiceD device xm41
xm41 vs2 nbias_tail gnd gnd nch_lvt_mac l=120e-9 w=14.4e-6 multi=1 nf=10 

** nch_lvt_mac Instance M31 = hspiceD device xm31
xm31 bias2 nbias_tail gnd gnd nch_lvt_mac l=120e-9 w=4e-6 multi=1 nf=5 

** cfmom_2t Instance C0 = hspiceD device xc0
xc0 vo1p net096 cfmom_2t nr=26 lr=1.9e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9

** cfmom_2t Instance C1 = hspiceD device xc1
xc1 vo1m net096 cfmom_2t nr=26 lr=1.9e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9

** cfmom_2t Instance C4 = hspiceD device xc4
xc4 vcmon vop cfmom_2t nr=36 lr=4.17e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9

** cfmom_2t Instance C7 = hspiceD device xc7
xc7 net0108 vop cfmom_2t nr=18 lr=1.91e-6 w=70e-9 s=70e-9 stm=2 spm=4 multi=1 ftip=140e-9

** cfmom_2t Instance C3 = hspiceD device xc3
xc3 vcmon vom cfmom_2t nr=36 lr=4.17e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9

** cfmom_2t Instance C6 = hspiceD device xc6
xc6 net0108 vom cfmom_2t nr=18 lr=1.91e-6 w=70e-9 s=70e-9 stm=2 spm=4 multi=1 ftip=140e-9
**Series configuration of R10
xr10 net0108 vom gnd  rppolywo_m lr=3.6e-6 wr=400e-9 multi=1 m=1 series=6 segspace=250e-9
**End of R10

**Series configuration of R1
xr1 vop net0108 gnd  rppolywo_m lr=3.6e-6 wr=400e-9 multi=1 m=1 series=6 segspace=250e-9
**End of R1

**Series configuration of R8
xr8 vo1m net096 gnd  rppolywo_m lr=7.86e-6 wr=400e-9 multi=1 m=1 series=10 segspace=250e-9
**End of R8

**Series configuration of R2
xr2 net096 vo1p gnd  rppolywo_m lr=7.86e-6 wr=400e-9 multi=1 m=1 series=10 segspace=250e-9
**End of R2


** pch_lvt_mac Instance M57 = hspiceD device xm57
xm57 vo1p pcas cas2n vdd pch_lvt_mac l=120e-9 w=18e-6 multi=1 nf=10 

** pch_lvt_mac Instance M64 = hspiceD device xm64
xm64 vcmon vcmop vdd vdd pch_lvt_mac l=120e-9 w=9.6e-6 multi=1 nf=10 

** pch_lvt_mac Instance M67 = hspiceD device xm67
xm67 vom vo1p vdd vdd pch_lvt_mac l=120e-9 w=28.8e-6 multi=1 nf=20 

** pch_lvt_mac Instance M66 = hspiceD device xm66
xm66 vcmop vcmop vdd vdd pch_lvt_mac l=120e-9 w=9.6e-6 multi=1 nf=10 

** pch_lvt_mac Instance M8 = hspiceD device xm8
xm8 cas2n net096 vdd vdd pch_lvt_mac l=120e-9 w=12e-6 multi=1 nf=10 

** pch_lvt_mac Instance M61 = hspiceD device xm61
xm61 cas2p net096 vdd vdd pch_lvt_mac l=120e-9 w=12e-6 multi=1 nf=10 

** pch_lvt_mac Instance M56 = hspiceD device xm56
xm56 vo1m pcas cas2p vdd pch_lvt_mac l=120e-9 w=18e-6 multi=1 nf=10 

** pch_lvt_mac Instance M9 = hspiceD device xm9
xm9 vop vo1m vdd vdd pch_lvt_mac l=120e-9 w=28.8e-6 multi=1 nf=20 

** pch_lvt_mac Instance M58 = hspiceD device xm58
xm58 pcas pcas net088 vdd pch_lvt_mac l=120e-9 w=6.4e-6 multi=1 nf=4 

** pch_hvt_mac Instance M63 = hspiceD device xm63
xm63 net088 pcas vdd vdd pch_hvt_mac l=120e-9 w=1.07e-6 multi=1 nf=1 
.ends OTA_XT_MAGICAL
** End of subcircuit definition.

.subckt OTA_XT_MAGICAL_2 gnd ncas vcm vdd vim vip vom vop

** nch_hvt_mac Instance M29 = hspiceD device xm29
xm29 vs vcmon gnd gnd nch_hvt_mac l=120e-9 w=40.5e-6 multi=1 nf=20 

** nch_lvt_mac Instance M5 = hspiceD device xm5
xm5 pcas vcm bias2 gnd nch_lvt_mac l=120e-9 w=4.8e-6 multi=1 nf=5 

** nch_lvt_mac Instance M30 = hspiceD device xm30
xm30 tail1 nbias_tail gnd gnd nch_lvt_mac l=120e-9 w=20e-6 multi=1 nf=25 

** nch_lvt_mac Instance M53 = hspiceD device xm53
xm53 vcmop net0108 vs2 gnd nch_lvt_mac l=120e-9 w=7.2e-6 multi=1 nf=8 

** nch_lvt_mac Instance M12 = hspiceD device xm12
xm12 vcmon vcm vs2 gnd nch_lvt_mac l=120e-9 w=7.2e-6 multi=1 nf=8 

** nch_lvt_mac Instance M50 = hspiceD device xm50
xm50 vo1p ncas casn gnd nch_lvt_mac l=120e-9 w=9e-6 multi=1 nf=10 

** nch_lvt_mac Instance M49 = hspiceD device xm49
xm49 vo1m ncas casp gnd nch_lvt_mac l=120e-9 w=9e-6 multi=1 nf=10 

** nch_lvt_mac Instance M51 = hspiceD device xm51
xm51 ncas ncas nbias_tail gnd nch_lvt_mac l=120e-9 w=4e-6 multi=1 nf=5 

** nch_lvt_mac Instance M47 = hspiceD device xm47
xm47 nbias_tail vcm bias1 gnd nch_lvt_mac l=120e-9 w=4.8e-6 multi=1 nf=5 

** nch_lvt_mac Instance M38 = hspiceD device xm38
xm38 bias1 nbias_tail gnd gnd nch_lvt_mac l=120e-9 w=4e-6 multi=1 nf=5 

** nch_lvt_mac Instance M7 = hspiceD device xm7
xm7 vop vim vs gnd nch_lvt_mac l=120e-9 w=36e-6 multi=1 nf=15 

** nch_lvt_mac Instance M43 = hspiceD device xm43
xm43 casn vim tail1 gnd nch_lvt_mac l=120e-9 w=13.5e-6 multi=1 nf=5 

** nch_lvt_mac Instance M10 = hspiceD device xm10
xm10 vom vip vs gnd nch_lvt_mac l=120e-9 w=36e-6 multi=1 nf=15 

** nch_lvt_mac Instance M40 = hspiceD device xm40
xm40 casp vip tail1 gnd nch_lvt_mac l=120e-9 w=13.5e-6 multi=1 nf=5 

** nch_lvt_mac Instance M41 = hspiceD device xm41
xm41 vs2 nbias_tail gnd gnd nch_lvt_mac l=120e-9 w=14.4e-6 multi=1 nf=10 

** nch_lvt_mac Instance M31 = hspiceD device xm31
xm31 bias2 nbias_tail gnd gnd nch_lvt_mac l=120e-9 w=4e-6 multi=1 nf=5 

** cfmom_2t Instance C0 = hspiceD device xc0
xc0 vo1p net096 cfmom_2t nr=26 lr=1.9e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9

** cfmom_2t Instance C1 = hspiceD device xc1
xc1 vo1m net096 cfmom_2t nr=26 lr=1.9e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9

** cfmom_2t Instance C4 = hspiceD device xc4
xc4 vcmon vop cfmom_2t nr=36 lr=4.17e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9

** cfmom_2t Instance C7 = hspiceD device xc7
xc7 net0108 vop cfmom_2t nr=18 lr=1.91e-6 w=70e-9 s=70e-9 stm=2 spm=4 multi=1 ftip=140e-9

** cfmom_2t Instance C3 = hspiceD device xc3
xc3 vcmon vom cfmom_2t nr=36 lr=4.17e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9

** cfmom_2t Instance C6 = hspiceD device xc6
xc6 net0108 vom cfmom_2t nr=18 lr=1.91e-6 w=70e-9 s=70e-9 stm=2 spm=4 multi=1 ftip=140e-9
**Series configuration of R10
xr10 net0108 vom gnd  rppolywo_m lr=3.6e-6 wr=400e-9 multi=1 m=1 series=6 segspace=250e-9
**End of R10

**Series configuration of R1
xr1 vop net0108 gnd  rppolywo_m lr=3.6e-6 wr=400e-9 multi=1 m=1 series=6 segspace=250e-9
**End of R1

**Series configuration of R8
xr8 vo1m net096 gnd  rppolywo_m lr=7.86e-6 wr=400e-9 multi=1 m=1 series=10 segspace=250e-9
**End of R8

**Series configuration of R2
xr2 net096 vo1p gnd  rppolywo_m lr=7.86e-6 wr=400e-9 multi=1 m=1 series=10 segspace=250e-9
**End of R2


** pch_lvt_mac Instance M57 = hspiceD device xm57
xm57 vo1p pcas cas2n vdd pch_lvt_mac l=120e-9 w=18e-6 multi=1 nf=10 

** pch_lvt_mac Instance M64 = hspiceD device xm64
xm64 vcmon vcmop vdd vdd pch_lvt_mac l=120e-9 w=9.6e-6 multi=1 nf=10 

** pch_lvt_mac Instance M67 = hspiceD device xm67
xm67 vom vo1p vdd vdd pch_lvt_mac l=120e-9 w=28.8e-6 multi=1 nf=20 

** pch_lvt_mac Instance M66 = hspiceD device xm66
xm66 vcmop vcmop vdd vdd pch_lvt_mac l=120e-9 w=9.6e-6 multi=1 nf=10 

** pch_lvt_mac Instance M8 = hspiceD device xm8
xm8 cas2n net096 vdd vdd pch_lvt_mac l=120e-9 w=12e-6 multi=1 nf=10 

** pch_lvt_mac Instance M61 = hspiceD device xm61
xm61 cas2p net096 vdd vdd pch_lvt_mac l=120e-9 w=12e-6 multi=1 nf=10 

** pch_lvt_mac Instance M56 = hspiceD device xm56
xm56 vo1m pcas cas2p vdd pch_lvt_mac l=120e-9 w=18e-6 multi=1 nf=10 

** pch_lvt_mac Instance M9 = hspiceD device xm9
xm9 vop vo1m vdd vdd pch_lvt_mac l=120e-9 w=28.8e-6 multi=1 nf=20 

** pch_lvt_mac Instance M58 = hspiceD device xm58
xm58 pcas pcas net088 vdd pch_lvt_mac l=120e-9 w=6.4e-6 multi=1 nf=4 

** pch_hvt_mac Instance M63 = hspiceD device xm63
xm63 net088 pcas vdd vdd pch_hvt_mac l=120e-9 w=1.07e-6 multi=1 nf=1 
.ends OTA_XT_MAGICAL_2

** End of subcircuit definition.


** Library name: STDLIB
** Cell name: BUFFD4BWP_LVT
** View name: schematic
** terminal mapping: I	= i
**                   Z	= z
**                   VDD	= vdd
**                   VSS	= vss
.subckt BUFFD4BWP_LVT i z vdd vss

** pch_lvt_mac Instance M_u3_0-M_u3 = hspiceD device x0
x0 z net11 vdd vdd pch_lvt_mac l=80e-9 w=820e-9 multi=1 nf=1 

** pch_lvt_mac Instance M_u2_1-M_u3 = hspiceD device x1
x1 net11 i vdd vdd pch_lvt_mac l=80e-9 w=820e-9 multi=1 nf=1 

** pch_lvt_mac Instance M_u2_0-M_u3 = hspiceD device x2
x2 net11 i vdd vdd pch_lvt_mac l=80e-9 w=820e-9 multi=1 nf=1 

** pch_lvt_mac Instance M_u3_2-M_u3 = hspiceD device x3
x3 z net11 vdd vdd pch_lvt_mac l=80e-9 w=820e-9 multi=1 nf=1 

** pch_lvt_mac Instance M_u3_1-M_u3 = hspiceD device x4
x4 z net11 vdd vdd pch_lvt_mac l=80e-9 w=820e-9 multi=1 nf=1 

** pch_lvt_mac Instance M_u3_3-M_u3 = hspiceD device x5
x5 z net11 vdd vdd pch_lvt_mac l=80e-9 w=820e-9 multi=1 nf=1 

** nch_lvt_mac Instance M_u3_1-M_u2 = hspiceD device x6
x6 z net11 vss vss nch_lvt_mac l=80e-9 w=620e-9 multi=1 nf=1 

** nch_lvt_mac Instance M_u3_2-M_u2 = hspiceD device x7
x7 z net11 vss vss nch_lvt_mac l=80e-9 w=620e-9 multi=1 nf=1 

** nch_lvt_mac Instance M_u3_3-M_u2 = hspiceD device x8
x8 z net11 vss vss nch_lvt_mac l=80e-9 w=620e-9 multi=1 nf=1 

** nch_lvt_mac Instance M_u2_1-M_u2 = hspiceD device x9
x9 net11 i vss vss nch_lvt_mac l=80e-9 w=620e-9 multi=1 nf=1 

** nch_lvt_mac Instance M_u3_0-M_u2 = hspiceD device x10
x10 z net11 vss vss nch_lvt_mac l=80e-9 w=620e-9 multi=1 nf=1 

** nch_lvt_mac Instance M_u2_0-M_u2 = hspiceD device x11
x11 net11 i vss vss nch_lvt_mac l=80e-9 w=620e-9 multi=1 nf=1 
.ends BUFFD4BWP_LVT
** End of subcircuit definition.

** Library name: STDLIB
** Cell name: NR2D8BWP_LVT
** View name: schematic
** terminal mapping: A1	= a1
**                   A2	= a2
**                   ZN	= zn
**                   VDD	= vdd
**                   VSS	= vss
.subckt NR2D8BWP_LVT a1 a2 zn vdd vss

** pch_lvt_mac Instance MI1-M_u1 = hspiceD device x0
x0 net13 a2 vdd vdd pch_lvt_mac l=80e-9 w=6.56e-6 multi=1 nf=1 

** pch_lvt_mac Instance MI1-M_u2 = hspiceD device x1
x1 zn a1 net13 vdd pch_lvt_mac l=80e-9 w=6.56e-6 multi=1 nf=1 

** nch_lvt_mac Instance MI1-M_u3 = hspiceD device x2
x2 zn a2 vss vss nch_lvt_mac l=80e-9 w=4.96e-6 multi=1 nf=1 

** nch_lvt_mac Instance MI1-M_u4 = hspiceD device x3
x3 zn a1 vss vss nch_lvt_mac l=80e-9 w=4.96e-6 multi=1 nf=1 
.ends NR2D8BWP_LVT
** End of subcircuit definition.

** Library name: 2019_CTDSM_Mingjie
** Cell name: SR_Latch_LVT
** View name: schematic
** terminal mapping: Q	= q
**                   QB	= qb
**                   R	= r
**                   S	= s
**                   VDD	= vdd
**                   VSS	= vss
.subckt SR_Latch_LVT q qb r s vdd vss

** NR2D8BWP_LVT Instance I1 = hspiceD device xi1
** Instance of Lib: STDLIB,  Cell: NR2D8BWP_LVT, View: schematic

** Port Connection: Instance  I1 of NR2D8BWP_LVT 
** a1(r) a2(qb) zn(q) vdd(vdd) vss(vss) 
xi1 r qb q vdd vss NR2D8BWP_LVT

** NR2D8BWP_LVT Instance I0 = hspiceD device xi0
** Instance of Lib: STDLIB,  Cell: NR2D8BWP_LVT, View: schematic

** Port Connection: Instance  I0 of NR2D8BWP_LVT 
** a1(s) a2(q) zn(qb) vdd(vdd) vss(vss) 
xi0 s q qb vdd vss NR2D8BWP_LVT
.ends SR_Latch_LVT
** End of subcircuit definition.

** Library name: CTDSM_TOP
** Cell name: COMPARATOR
** View name: schematic
** terminal mapping: CLK	= clk
**                   GND	= gnd
**                   OUTM	= outm
**                   OUTP	= outp
**                   VDD	= vdd
**                   VI+	= _net0
**                   VI-	= _net1
.subckt COMPARATOR_schematic clk gnd outm outp vdd _net0 _net1

** nch_lvt_mac Instance M0 = hspiceD device xm0
xm0 gnd intern gnd gnd nch_lvt_mac l=1e-6 w=1.05e-6 multi=1 nf=1 

** nch_lvt_mac Instance M22 = hspiceD device xm22
xm22 gnd interp gnd gnd nch_lvt_mac l=1e-6 w=1.05e-6 multi=1 nf=1 

** nch_lvt_mac Instance M16 = hspiceD device xm16
xm16 outm crossp gnd gnd nch_lvt_mac l=40e-9 w=1.44e-6 multi=1 nf=4 

** nch_lvt_mac Instance M17 = hspiceD device xm17
xm17 outp crossn gnd gnd nch_lvt_mac l=40e-9 w=1.44e-6 multi=1 nf=4 

** nch_lvt_mac Instance M4 = hspiceD device xm4
xm4 crossn crossp intern gnd nch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 

** nch_lvt_mac Instance M3 = hspiceD device xm3
xm3 crossp crossn interp gnd nch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 

** nch_lvt_mac Instance M7 = hspiceD device xm7
xm7 net069 clk gnd gnd nch_lvt_mac l=40e-9 w=3.68e-6 multi=1 nf=8 

** nch_lvt_mac Instance M5 = hspiceD device xm5
xm5 intern _net0 net069 gnd nch_lvt_mac l=40e-9 w=7.68e-6 multi=1 nf=8 

** nch_lvt_mac Instance M6 = hspiceD device xm6
xm6 interp _net1 net069 gnd nch_lvt_mac l=40e-9 w=7.68e-6 multi=1 nf=8 

** pch_lvt_mac Instance M8 = hspiceD device xm8
xm8 outm crossp vdd vdd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 

** pch_lvt_mac Instance M18 = hspiceD device xm18
xm18 intern clk vdd vdd pch_lvt_mac l=40e-9 w=960e-9 multi=1 nf=2 

** pch_lvt_mac Instance M15 = hspiceD device xm15
xm15 outp crossn vdd vdd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 

** pch_lvt_mac Instance M2 = hspiceD device xm2
xm2 interp clk vdd vdd pch_lvt_mac l=40e-9 w=960e-9 multi=1 nf=2 

** pch_lvt_mac Instance M1 = hspiceD device xm1
xm1 crossn clk vdd vdd pch_lvt_mac l=40e-9 w=960e-9 multi=1 nf=2 

** pch_lvt_mac Instance M12 = hspiceD device xm12
xm12 crossp clk vdd vdd pch_lvt_mac l=40e-9 w=960e-9 multi=1 nf=2 

** pch_lvt_mac Instance M14 = hspiceD device xm14
xm14 crossn crossp vdd vdd pch_lvt_mac l=40e-9 w=3.84e-6 multi=1 nf=8 

** pch_lvt_mac Instance M13 = hspiceD device xm13
xm13 crossp crossn vdd vdd pch_lvt_mac l=40e-9 w=3.84e-6 multi=1 nf=8 
.ends COMPARATOR_schematic
** End of subcircuit definition.

.subckt RR1 net040 net010 VREF GND net025 SYS_CLKB VDD net035
xi24 net040 net010 VREF GND BUFFD4BWP_LVT
xi25 net025 SYS_CLKB net040 net035 VDD GND DFCND4BWP_LVT_stupid
.ends RR1

.subckt RRR1 net046 SYS_CLKB VDD DO net038 GND net025 net028 net020 SYS_CLK
xi12 net046 SYS_CLKB DO net038 VDD GND DFCND4BWP_LVT_stupid
xi6 net046 net025 net028 net020 VDD GND SR_Latch_LVT
xi4 SYS_CLK SYS_CLKB VDD GND INVD4BWP_LVT
.ends RRR1

** Library name: CTDSM_TOP
** Cell name: CTDSM_TOP
** View name: schematic
.subckt CTDSM_TOP DO GND IBIAS1 IBIAS2 SYS_CLK VCM VDD VINN VINP OTA1_INN OTA1_INP OTA2_INN OTA2_INP SUM_N SUM_P VINT1N VINT1P VINT2N VINT2P VREF
** terminal mapping: DO	= do
**                   GND	= gnd
**                   IBIAS1	= ibias1
**                   IBIAS2	= ibias2
**                   SYS_CLK	= sys_clk
**                   VCM	= vcm
**                   VDD	= vdd
**                   VINN	= vinn
**                   VINP	= vinp
**                   OTA1_INN	= ota1_inn
**                   OTA1_INP	= ota1_inp
**                   OTA2_INN	= ota2_inn
**                   OTA2_INP	= ota2_inp
**                   SUM_N	= sum_n
**                   SUM_P	= sum_p
**                   VINT1N	= vint1n
**                   VINT1P	= vint1p
**                   VINT2N	= vint2n
**                   VINT2P	= vint2p
**                   VREF	= vref

input_res  VINP VINN OTA1_INP OTA1_INN GND INPUT_RES

**Series configuration of R47
xr47 OTA1_INN VINT2P GND  rppolywo_m lr=34.8e-6 wr=400e-9 multi=1 m=1 series=46 segspace=250e-9
**End of R47

**Series configuration of R28
xr28 OTA1_INP VINT2N GND  rppolywo_m lr=34.8e-6 wr=400e-9 multi=1 m=1 series=46 segspace=250e-9
**End of R28

**Series configuration of R21
xr21 net010 OTA1_INP GND  rppolywo_m lr=10e-6 wr=600e-9 multi=1 m=1 series=5 segspace=250e-9
**End of R21

**Series configuration of R20
xr20 net012 OTA1_INN GND  rppolywo_m lr=10e-6 wr=600e-9 multi=1 m=1 series=5 segspace=250e-9
**End of R20

**Series configuration of R23
xr23 net010 OTA2_INP GND  rppolywo_m lr=10e-6 wr=600e-9 multi=1 m=1 series=10 segspace=250e-9
**End of R23

**Series configuration of R25
xr25 net012 SUM_N GND  rppolywo_m lr=10e-6 wr=600e-9 multi=1 m=1 series=15 segspace=250e-9
**End of R25

**Series configuration of R24
xr24 net010 SUM_P GND  rppolywo_m lr=10e-6 wr=600e-9 multi=1 m=1 series=15 segspace=250e-9
**End of R24

**Series configuration of R22
xr22 net012 OTA2_INN GND  rppolywo_m lr=10e-6 wr=600e-9 multi=1 m=1 series=10 segspace=250e-9
**End of R22

xr19 VINT2N SUM_N GND   rppolywo_m lr=10e-6 wr=600e-9 multi=1 m=1 series=1 segspace=250e-9

**Series configuration of R16
xr16 VINT1N OTA2_INN GND  rppolywo_m lr=10e-6 wr=600e-9 multi=1 m=1 series=5 segspace=250e-9
**End of R16

**Series configuration of R17
xr17 VINT1P OTA2_INP GND  rppolywo_m lr=10e-6 wr=600e-9 multi=1 m=1 series=5 segspace=250e-9
**End of R17


xr18  VINT2P SUM_P GND   rppolywo_m lr=10e-6 wr=600e-9 multi=1 m=1 series=1 segspace=250e-9


** INVD4BWP_LVT Instance I4 = hspiceD device xi4
** Instance of Lib: STDLIB,  Cell: INVD4BWP_LVT, View: schematic

** Port Connection: Instance  I4 of INVD4BWP_LVT 
** i(sys_clk) zn(sys_clkb) vdd(vdd) vss(gnd) 
**xi4 SYS_CLK SYS_CLKB VDD GND INVD4BWP_LVT

** cfmom_2t Instance C4 = hspiceD device xc4
xc4 OTA1_INP VINT1N cfmom_2t nr=96 lr=12.4e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9

** cfmom_2t Instance C2 = hspiceD device xc2
xc2 OTA2_INN VINT2P cfmom_2t nr=70 lr=9.85e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9

** cfmom_2t Instance C0 = hspiceD device xc0
xc0 OTA1_INN VINT1P cfmom_2t nr=96 lr=12.4e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9

** cfmom_2t Instance C1 = hspiceD device xc1
xc1 OTA2_INP VINT2N cfmom_2t nr=70 lr=9.85e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9

** DFCND4BWP_LVT Instance I12 = hspiceD device xi12
** Instance of Lib: STDLIB,  Cell: DFCND4BWP_LVT, View: schematic

** Port Connection: Instance  I12 of DFCND4BWP_LVT 
** d(net046) cp(sys_clkb) cdn(vdd) q(do) qn(net038) vdd(vdd) vss(gnd) 
**xi12 net046 SYS_CLKB VDD DO net038 VDD GND DFCND4BWP_LVT

** DFCND4BWP_LVT Instance I25 = hspiceD device xi25
** Instance of Lib: STDLIB,  Cell: DFCND4BWP_LVT, View: schematic

** Port Connection: Instance  I25 of DFCND4BWP_LVT 
** d(net025) cp(sys_clkb) cdn(vdd) q(net040) qn(net035) vdd(vdd) vss(gnd) 
**xi25 net025 SYS_CLKB VDD net040 net035 VDD GND DFCND4BWP_LVT

** DFCND4BWP_LVT Instance I3 = hspiceD device xi3
** Instance of Lib: STDLIB,  Cell: DFCND4BWP_LVT, View: schematic

** Port Connection: Instance  I3 of DFCND4BWP_LVT 
** d(net046) cp(sys_clkb) cdn(vdd) q(net026) qn(net022) vdd(vdd) vss(gnd) 
**xi3 net046 SYS_CLKB VDD net026 net022 VDD GND DFCND4BWP_LVT

** OTA_XT_MAGICAL Instance I22 = hspiceD device xi22
** Instance of Lib: CTDSM_TOP,  Cell: OTA_XT_MAGICAL, View: schematic

** Port Connection: Instance  I22 of OTA_XT_MAGICAL 
** gnd(gnd) ncas(ibias2) vcm(vcm) vdd(vdd) vim(ota2_inn) vip(ota2_inp) vom(vint2n) vop(vint2p) 
xi22 GND IBIAS2 VCM VDD OTA2_INN OTA2_INP VINT2N VINT2P OTA_XT_MAGICAL

** OTA_XT_MAGICAL Instance I20 = hspiceD device xi20
** Instance of Lib: CTDSM_TOP,  Cell: OTA_XT_MAGICAL, View: schematic

** Port Connection: Instance  I20 of OTA_XT_MAGICAL 
** gnd(gnd) ncas(ibias1) vcm(vcm) vdd(vdd) vim(ota1_inp) vip(ota1_inn) vom(vint1p) vop(vint1n) 
xi20 GND IBIAS1 VCM VDD OTA1_INP OTA1_INN VINT1P VINT1N OTA_XT_MAGICAL

** BUFFD4BWP_LVT Instance I24 = hspiceD device xi24
** Instance of Lib: STDLIB,  Cell: BUFFD4BWP_LVT, View: schematic

** Port Connection: Instance  I24 of BUFFD4BWP_LVT 
** i(net040) z(net010) vdd(vref) vss(gnd) 
**xi24 net040 net010 VREF GND BUFFD4BWP_LVT

** BUFFD4BWP_LVT Instance I23 = hspiceD device xi23
** Instance of Lib: STDLIB,  Cell: BUFFD4BWP_LVT, View: schematic

** Port Connection: Instance  I23 of BUFFD4BWP_LVT 
** i(net026) z(net012) vdd(vref) vss(gnd) 
**xi23 net026 net012 VREF GND BUFFD4BWP_LVT

** SR_Latch_LVT Instance I6 = hspiceD device xi6
** Instance of Lib: 2019_CTDSM_Mingjie,  Cell: SR_Latch_LVT, View: schematic

** Port Connection: Instance  I6 of SR_Latch_LVT 
** q(net046) qb(net025) r(net028) s(net020) vdd(vdd) vss(gnd) 
**xi6 net046 net025 net028 net020 VDD GND SR_Latch_LVT

** COMPARATOR Instance I19 = hspiceD device xi19
** Instance of Lib: CTDSM_TOP,  Cell: COMPARATOR, View: schematic

** Port Connection: Instance  I19 of COMPARATOR 
** clk(sys_clk) gnd(gnd) outm(net028) outp(net020) vdd(vdd) _net0(sum_p) _net1(sum_n) 
xi19 SYS_CLK GND net028 net020 VDD SUM_P SUM_N COMPARATOR_schematic

rr1 net040 net010 VREF GND net025 SYS_CLKB VDD net035 RR1
rr2 net026 net012 VREF GND net046 SYS_CLKB VDD net022 RR1

**xi24 net040 net010 VREF GND BUFFD4BWP_LVT
**xi25 net025 SYS_CLKB VDD net040 net035 VDD GND DFCND4BWP_LVT
**xi23 net026 net012 VREF GND BUFFD4BWP_LVT
**xi3  net046 SYS_CLKB VDD net026 net022 VDD GND DFCND4BWP_LVT

rrr1 net046 SYS_CLKB VDD DO net038 GND net025 net028 net020 SYS_CLK RRR1

.ends CTDSM_TOP
** End of subcircuit definition.
** .END
