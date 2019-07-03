** Generated for: hspiceD
** Generated on: Feb 23 19:06:18 2019
** Design library name: CONST_EXTR
** Design cell name: CTDSM_CORE_NEW
** Design view name: schematic


** .TEMP 25.0
** .OPTION
** +    ARTIST=2
** +    INGOLD=2
** +    PARHIER=LOCAL
** +    PSF=2
** .LIB "/usr/local/packages/tsmc_40/pdk/models/hspice/toplevel.l" top_tt

** Library name: CTDSM_DEC2016
** Cell name: Gm1_v5_Practice
** View name: schematic
** terminal mapping: IBIAS	= ibias
**                   VDD	= vdd
**                   VIM	= vim
**                   VIP	= vip
**                   VOM	= vom
**                   VOP	= vop
**                   VSS	= vss
.subckt type:analog Gm1_v5_Practice ibias vdd vim vip vom vop vss

** nch_hvt_mac Instance M8 = hspiceD device xm8
xm8 net074 ntail1 vss vss nch_hvt_mac l=120e-9 w=3.22e-6 multi=1 nf=4 sd=140e-9 ad=225.4e-15 as=289.8e-15 pd=3.78e-6 ps=5.55e-6 nrd=13.746e-3 nrs=13.746e-3 sa=313.488e-9 sb=313.488e-9 sa1=179.031e-9 sa2=290.688e-9 sa3=485.728e-9 sa4=276.641e-9 sb1=179.031e-9 sb2=290.688e-9 sb3=485.728e-9 spa=269.255e-9 spa1=211.397e-9 spa2=155.295e-9 spa3=170.776e-9 sap=244.923e-9 spba=223.131e-9 sapb=396.954e-9 spba1=228.22e-9

** pch_lvt_mac Instance M2 = hspiceD device xm2
xm2 vdd ibias vdd vdd pch_lvt_mac l=3.3e-6 w=2.95e-6 multi=1 nf=1 sd=140e-9 ad=324.5e-15 as=324.5e-15 pd=6.12e-6 ps=6.12e-6 nrd=15.641e-3 nrs=15.641e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=3e-6 spa1=3e-6 spa2=3e-6 spa3=3e-6 sap=1.00025e-6 spba=1.82483e-6 sapb=577.831e-9 spba1=2.1045e-6

** pch_lvt_mac Instance M4 = hspiceD device xm4
xm4 vdd ibias vdd vdd pch_lvt_mac l=3.3e-6 w=2.95e-6 multi=1 nf=1 sd=140e-9 ad=324.5e-15 as=324.5e-15 pd=6.12e-6 ps=6.12e-6 nrd=15.641e-3 nrs=15.641e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=3e-6 spa1=3e-6 spa2=3e-6 spa3=3e-6 sap=1.00025e-6 spba=1.82483e-6 sapb=577.831e-9 spba1=2.1045e-6

** pch_lvt_mac Instance M12 = hspiceD device xm12
xm12 ibias ibias vdd vdd pch_lvt_mac l=120e-9 w=585e-9 multi=1 nf=1 sd=140e-9 ad=64.35e-15 as=64.35e-15 pd=1.39e-6 ps=1.39e-6 nrd=85.492e-3 nrs=85.492e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=3e-6 spa1=3e-6 spa2=3e-6 spa3=3e-6 sap=1.00025e-6 spba=1.73551e-6 sapb=577.831e-9 spba1=1.74589e-6

** pch_lvt_mac Instance M11 = hspiceD device xm11
xm11 vom ibias vdd vdd pch_lvt_mac l=120e-9 w=2.34e-6 multi=1 nf=4 sd=140e-9 ad=163.8e-15 as=210.6e-15 pd=2.9e-6 ps=4.23e-6 nrd=19.88e-3 nrs=19.88e-3 sa=313.488e-9 sb=313.488e-9 sa1=179.031e-9 sa2=290.688e-9 sa3=485.728e-9 sa4=276.641e-9 sb1=179.031e-9 sb2=290.688e-9 sb3=485.728e-9 spa=269.255e-9 spa1=211.397e-9 spa2=155.295e-9 spa3=170.776e-9 sap=244.923e-9 spba=223.131e-9 sapb=396.954e-9 spba1=228.22e-9

** pch_lvt_mac Instance M15 = hspiceD device xm15
** xm15 ibias ibias vdd vdd pch_lvt_mac l=120e-9 w=585e-9 multi=1 nf=1 sd=140e-9 ad=64.35e-15 as=64.35e-15 pd=1.39e-6 ps=1.39e-6 nrd=85.492e-3 nrs=85.492e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=3e-6 spa1=3e-6 spa2=3e-6 spa3=3e-6 sap=1.00025e-6 spba=1.73551e-6 sapb=577.831e-9 spba1=1.74589e-6

** pch_lvt_mac Instance M14 = hspiceD device xm14
xm14 vop ibias vdd vdd pch_lvt_mac l=120e-9 w=2.34e-6 multi=1 nf=4 sd=140e-9 ad=163.8e-15 as=210.6e-15 pd=2.9e-6 ps=4.23e-6 nrd=19.88e-3 nrs=19.88e-3 sa=313.488e-9 sb=313.488e-9 sa1=179.031e-9 sa2=290.688e-9 sa3=485.728e-9 sa4=276.641e-9 sb1=179.031e-9 sb2=290.688e-9 sb3=485.728e-9 spa=269.255e-9 spa1=211.397e-9 spa2=155.295e-9 spa3=170.776e-9 sap=244.923e-9 spba=223.131e-9 sapb=396.954e-9 spba1=228.22e-9

** nch_lvt_dnw_mac Instance M26 = hspiceD device xm26
xm26 vop vim net074 net074 nch_lvt_mac l=120e-9 w=1.7e-6 multi=1 nf=4 sd=160e-9 ad=136e-15 as=161.5e-15 pd=2.34e-6 ps=3.31e-6 nrd=17.837e-3 nrs=17.837e-3 sa=322.243e-9 sb=322.243e-9 sa1=179.411e-9 sa2=296.65e-9 sa3=502.879e-9 sa4=282.611e-9 sb1=179.411e-9 sb2=296.65e-9 sb3=502.879e-9 spa=293.52e-9 spa1=234.251e-9 spa2=177.307e-9 spa3=193.816e-9 sap=272.622e-9 spba=246.314e-9 sapb=424.261e-9 spba1=251.535e-9

** nch_lvt_dnw_mac Instance M27 = hspiceD device xm27
xm27 vom vip net074 net074 nch_lvt_mac l=120e-9 w=1.7e-6 multi=1 nf=4 sd=160e-9 ad=136e-15 as=161.5e-15 pd=2.34e-6 ps=3.31e-6 nrd=17.837e-3 nrs=17.837e-3 sa=322.243e-9 sb=322.243e-9 sa1=179.411e-9 sa2=296.65e-9 sa3=502.879e-9 sa4=282.611e-9 sb1=179.411e-9 sb2=296.65e-9 sb3=502.879e-9 spa=293.52e-9 spa1=234.251e-9 spa2=177.307e-9 spa3=193.816e-9 sap=272.622e-9 spba=246.314e-9 sapb=424.261e-9 spba1=251.535e-9

** crtmom Instance C21 = hspiceD device xc21
xc21 ntail1 vom vss crtmom nv=16 nh=16 w=70e-9 s=70e-9 stm=1 spm=6 multi=1 ftip=140e-9 dmflag=0

** crtmom Instance C22 = hspiceD device xc22
xc22 vop ntail1 vss crtmom nv=16 nh=16 w=70e-9 s=70e-9 stm=1 spm=6 multi=1 ftip=140e-9 dmflag=0
**Series configuration of R12
xr12  ntail1 vop vss  rppolywo_m lr=6.6e-6 wr=400e-9 multi=1 m=1
**End of R12

**Series configuration of R11
xr11  vom ntail1 vss  rppolywo_m lr=6.6e-6 wr=400e-9 multi=1 m=1
**End of R11


** nch_lvt_mac Instance M3 = hspiceD device xm3
xm3 vss ntail1 vss vss nch_lvt_mac l=2.2e-6 w=2.5e-6 multi=1 nf=1 sd=140e-9 ad=275e-15 as=275e-15 pd=5.22e-6 ps=5.22e-6 nrd=12.421e-3 nrs=12.421e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=3e-6 spa1=3e-6 spa2=3e-6 spa3=3e-6 sap=1.00025e-6 spba=1.79444e-6 sapb=577.831e-9 spba1=1.98202e-6

** nch_lvt_mac Instance M0 = hspiceD device xm0
xm0 vss ntail1 vss vss nch_lvt_mac l=2.2e-6 w=2.5e-6 multi=1 nf=1 sd=140e-9 ad=275e-15 as=275e-15 pd=5.22e-6 ps=5.22e-6 nrd=12.421e-3 nrs=12.421e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=3e-6 spa1=3e-6 spa2=3e-6 spa3=3e-6 sap=1.00025e-6 spba=1.79444e-6 sapb=577.831e-9 spba1=1.98202e-6

** pwdnw Instance D0 = hspiceD device d0
d0 net074 vdd pwdnw area=8.3058e-12 pj=11.98e-6 m=1

** dnwpsub Instance D1 = hspiceD device d1
d1 vss vdd dnwpsub area=13.89e-12 pj=15.26e-6 m=1
.ends Gm1_v5_Practice
** End of subcircuit definition.

** Library name: tcbn40lpbwp
** Cell name: DFCNQD2BWP
** View name: schematic
** terminal mapping: D	= d
**                   CP	= cp
**                   CDN	= cdn
**                   Q	= q
**                   VDD	= vdd
**                   VSS	= vss
.subckt type:digital DFCNQD2BWP d cp cdn q vdd vss

** nch Instance MI24-M_u2 = hspiceD device m0
m0 net63 cp vss vss nch l=40e-9 w=155e-9 m=1 nf=1 sd=140e-9 ad=17.05e-15 as=17.05e-15 pd=530e-9 ps=530e-9 nrd=451.613e-3 nrs=451.613e-3 sa=110e-9 sb=110e-9

** nch Instance MI4 = hspiceD device mi4
mi4 net61 net63 vss vss nch l=40e-9 w=310e-9 m=1 nf=1 sd=140e-9 ad=34.1e-15 as=34.1e-15 pd=840e-9 ps=840e-9 nrd=225.806e-3 nrs=225.806e-3 sa=110e-9 sb=110e-9

** nch Instance MI35-M_u3 = hspiceD device m1
m1 net97 cdn net60 vss nch l=40e-9 w=210e-9 m=1 nf=1 sd=140e-9 ad=23.1e-15 as=23.1e-15 pd=640e-9 ps=640e-9 nrd=333.333e-3 nrs=333.333e-3 sa=110e-9 sb=110e-9

** nch Instance MI13-M_u2 = hspiceD device m2
m2 net123 net95 vss vss nch l=40e-9 w=150e-9 m=1 nf=1 sd=140e-9 ad=16.5e-15 as=16.5e-15 pd=520e-9 ps=520e-9 nrd=466.667e-3 nrs=466.667e-3 sa=110e-9 sb=110e-9

** nch Instance MI29 = hspiceD device mi29
mi29 net49 net63 net17 vss nch l=40e-9 w=120e-9 m=1 nf=1 sd=140e-9 ad=13.2e-15 as=13.2e-15 pd=460e-9 ps=460e-9 nrd=583.333e-3 nrs=583.333e-3 sa=110e-9 sb=110e-9

** nch Instance MI15 = hspiceD device mi15
mi15 net123 net81 net49 vss nch l=40e-9 w=150e-9 m=1 nf=1 sd=140e-9 ad=16.5e-15 as=16.5e-15 pd=520e-9 ps=520e-9 nrd=466.667e-3 nrs=466.667e-3 sa=110e-9 sb=110e-9

** nch Instance MI35-M_u4 = hspiceD device m3
m3 net60 net49 vss vss nch l=40e-9 w=210e-9 m=1 nf=1 sd=140e-9 ad=23.1e-15 as=23.1e-15 pd=640e-9 ps=640e-9 nrd=333.333e-3 nrs=333.333e-3 sa=110e-9 sb=110e-9

** nch Instance MI36-M_u3 = hspiceD device m4
m4 net97 cdn net21 vss nch l=40e-9 w=210e-9 m=1 nf=1 sd=140e-9 ad=23.1e-15 as=23.1e-15 pd=640e-9 ps=640e-9 nrd=333.333e-3 nrs=333.333e-3 sa=110e-9 sb=110e-9

** nch Instance MI34-M_u2 = hspiceD device m5
m5 net81 net63 vss vss nch l=40e-9 w=155e-9 m=1 nf=1 sd=140e-9 ad=17.05e-15 as=17.05e-15 pd=530e-9 ps=530e-9 nrd=451.613e-3 nrs=451.613e-3 sa=110e-9 sb=110e-9

** nch Instance MI5 = hspiceD device mi5
mi5 net95 d net61 vss nch l=40e-9 w=310e-9 m=1 nf=1 sd=140e-9 ad=34.1e-15 as=34.1e-15 pd=840e-9 ps=840e-9 nrd=225.806e-3 nrs=225.806e-3 sa=110e-9 sb=110e-9

** nch Instance MI49 = hspiceD device mi49
mi49 net25 cdn vss vss nch l=40e-9 w=120e-9 m=1 nf=1 sd=140e-9 ad=13.2e-15 as=13.2e-15 pd=460e-9 ps=460e-9 nrd=583.333e-3 nrs=583.333e-3 sa=110e-9 sb=110e-9

** nch Instance MI36-M_u4 = hspiceD device m6
m6 net21 net49 vss vss nch l=40e-9 w=210e-9 m=1 nf=1 sd=140e-9 ad=23.1e-15 as=23.1e-15 pd=640e-9 ps=640e-9 nrd=333.333e-3 nrs=333.333e-3 sa=110e-9 sb=110e-9

** nch Instance MI26 = hspiceD device mi26
mi26 net17 net97 vss vss nch l=40e-9 w=120e-9 m=1 nf=1 sd=140e-9 ad=13.2e-15 as=13.2e-15 pd=460e-9 ps=460e-9 nrd=583.333e-3 nrs=583.333e-3 sa=110e-9 sb=110e-9

** nch Instance MI48 = hspiceD device mi48
mi48 net13 net123 net25 vss nch l=40e-9 w=120e-9 m=1 nf=1 sd=140e-9 ad=13.2e-15 as=13.2e-15 pd=460e-9 ps=460e-9 nrd=583.333e-3 nrs=583.333e-3 sa=110e-9 sb=110e-9

** nch Instance MI27-M_u2 = hspiceD device m7
m7 q net97 vss vss nch l=40e-9 w=310e-9 m=1 nf=1 sd=140e-9 ad=34.1e-15 as=34.1e-15 pd=840e-9 ps=840e-9 nrd=225.806e-3 nrs=225.806e-3 sa=110e-9 sb=110e-9

** nch Instance MI22-M_u2 = hspiceD device m8
m8 q net97 vss vss nch l=40e-9 w=310e-9 m=1 nf=1 sd=140e-9 ad=34.1e-15 as=34.1e-15 pd=840e-9 ps=840e-9 nrd=225.806e-3 nrs=225.806e-3 sa=110e-9 sb=110e-9

** nch Instance MI47 = hspiceD device mi47
mi47 net95 net81 net13 vss nch l=40e-9 w=120e-9 m=1 nf=1 sd=140e-9 ad=13.2e-15 as=13.2e-15 pd=460e-9 ps=460e-9 nrd=583.333e-3 nrs=583.333e-3 sa=110e-9 sb=110e-9

** pch Instance MI33 = hspiceD device mi33
mi33 net80 net97 vdd vdd pch l=40e-9 w=120e-9 m=1 nf=1 sd=140e-9 ad=13.2e-15 as=13.2e-15 pd=460e-9 ps=460e-9 nrd=583.333e-3 nrs=583.333e-3 sa=110e-9 sb=110e-9

** pch Instance MI22-M_u3 = hspiceD device m9
m9 q net97 vdd vdd pch l=40e-9 w=410e-9 m=1 nf=1 sd=140e-9 ad=45.1e-15 as=45.1e-15 pd=1.04e-6 ps=1.04e-6 nrd=170.732e-3 nrs=170.732e-3 sa=110e-9 sb=110e-9

** pch Instance MI35-M_u2 = hspiceD device m10
m10 net97 net49 vdd vdd pch l=40e-9 w=370e-9 m=1 nf=1 sd=140e-9 ad=40.7e-15 as=40.7e-15 pd=960e-9 ps=960e-9 nrd=189.189e-3 nrs=189.189e-3 sa=110e-9 sb=110e-9

** pch Instance MI43 = hspiceD device mi43
mi43 net101 net123 vdd vdd pch l=40e-9 w=120e-9 m=1 nf=1 sd=140e-9 ad=13.2e-15 as=13.2e-15 pd=460e-9 ps=460e-9 nrd=583.333e-3 nrs=583.333e-3 sa=110e-9 sb=110e-9

** pch Instance MI6 = hspiceD device mi6
mi6 net95 d net120 vdd pch l=40e-9 w=340e-9 m=1 nf=1 sd=140e-9 ad=37.4e-15 as=37.4e-15 pd=900e-9 ps=900e-9 nrd=205.882e-3 nrs=205.882e-3 sa=110e-9 sb=110e-9

** pch Instance MI27-M_u3 = hspiceD device m11
m11 q net97 vdd vdd pch l=40e-9 w=410e-9 m=1 nf=1 sd=140e-9 ad=45.1e-15 as=45.1e-15 pd=1.04e-6 ps=1.04e-6 nrd=170.732e-3 nrs=170.732e-3 sa=110e-9 sb=110e-9

** pch Instance MI36-M_u2 = hspiceD device m12
m12 net97 net49 vdd vdd pch l=40e-9 w=370e-9 m=1 nf=1 sd=140e-9 ad=40.7e-15 as=40.7e-15 pd=960e-9 ps=960e-9 nrd=189.189e-3 nrs=189.189e-3 sa=110e-9 sb=110e-9

** pch Instance MI35-M_u1 = hspiceD device m13
m13 net97 cdn vdd vdd pch l=40e-9 w=370e-9 m=1 nf=1 sd=140e-9 ad=40.7e-15 as=40.7e-15 pd=960e-9 ps=960e-9 nrd=189.189e-3 nrs=189.189e-3 sa=110e-9 sb=110e-9

** pch Instance MI44 = hspiceD device mi44
mi44 net101 cdn vdd vdd pch l=40e-9 w=120e-9 m=1 nf=1 sd=140e-9 ad=13.2e-15 as=13.2e-15 pd=460e-9 ps=460e-9 nrd=583.333e-3 nrs=583.333e-3 sa=110e-9 sb=110e-9

** pch Instance MI36-M_u1 = hspiceD device m14
m14 net97 cdn vdd vdd pch l=40e-9 w=370e-9 m=1 nf=1 sd=140e-9 ad=40.7e-15 as=40.7e-15 pd=960e-9 ps=960e-9 nrd=189.189e-3 nrs=189.189e-3 sa=110e-9 sb=110e-9

** pch Instance MI13-M_u3 = hspiceD device m15
m15 net123 net95 vdd vdd pch l=40e-9 w=180e-9 m=1 nf=1 sd=140e-9 ad=19.8e-15 as=19.8e-15 pd=580e-9 ps=580e-9 nrd=388.889e-3 nrs=388.889e-3 sa=110e-9 sb=110e-9

** pch Instance MI24-M_u3 = hspiceD device m16
m16 net63 cp vdd vdd pch l=40e-9 w=205e-9 m=1 nf=1 sd=140e-9 ad=22.55e-15 as=22.55e-15 pd=630e-9 ps=630e-9 nrd=341.463e-3 nrs=341.463e-3 sa=110e-9 sb=110e-9

** pch Instance MI16 = hspiceD device mi16
mi16 net123 net63 net49 vdd pch l=40e-9 w=180e-9 m=1 nf=1 sd=140e-9 ad=19.8e-15 as=19.8e-15 pd=580e-9 ps=580e-9 nrd=388.889e-3 nrs=388.889e-3 sa=110e-9 sb=110e-9

** pch Instance MI34-M_u3 = hspiceD device m17
m17 net81 net63 vdd vdd pch l=40e-9 w=205e-9 m=1 nf=1 sd=140e-9 ad=22.55e-15 as=22.55e-15 pd=630e-9 ps=630e-9 nrd=341.463e-3 nrs=341.463e-3 sa=110e-9 sb=110e-9

** pch Instance MI32 = hspiceD device mi32
mi32 net49 net81 net80 vdd pch l=40e-9 w=120e-9 m=1 nf=1 sd=140e-9 ad=13.2e-15 as=13.2e-15 pd=460e-9 ps=460e-9 nrd=583.333e-3 nrs=583.333e-3 sa=110e-9 sb=110e-9

** pch Instance MI45 = hspiceD device mi45
mi45 net95 net63 net101 vdd pch l=40e-9 w=120e-9 m=1 nf=1 sd=140e-9 ad=13.2e-15 as=13.2e-15 pd=460e-9 ps=460e-9 nrd=583.333e-3 nrs=583.333e-3 sa=110e-9 sb=110e-9

** pch Instance MI7 = hspiceD device mi7
mi7 net120 net81 vdd vdd pch l=40e-9 w=340e-9 m=1 nf=1 sd=140e-9 ad=37.4e-15 as=37.4e-15 pd=900e-9 ps=900e-9 nrd=205.882e-3 nrs=205.882e-3 sa=110e-9 sb=110e-9
.ends DFCNQD2BWP
** End of subcircuit definition.

** Library name: CTDSM_DEC2016
** Cell name: C_DAC
** View name: schematic
** terminal mapping: CLKB	= clkb
**                   IN	= in
**                   R3	= r3
**                   R4	= r4
**                   RSTB	= rstb
**                   VDD	= vdd
**                   VSS	= vss
.subckt type:analog C_DAC clkb in r3 r4 rstb vdd vss
**Series configuration of R27
xr27  r3 net10 vss  rppolywo_m lr=19.92e-6 wr=1.2e-6 multi=1 m=1
**End of R27

**Series configuration of R64
xr64  r4 in vss  rppolywo_m lr=18.67e-6 wr=1.2e-6 multi=1 m=1
**End of R64


** DFCNQD2BWP Instance I94 = hspiceD device xi94
** Instance of Lib: tcbn40lpbwp,  Cell: DFCNQD2BWP, View: schematic
xi94 in clkb rstb net10 vdd vss DFCNQD2BWP
.ends C_DAC
** End of subcircuit definition.

** Library name: CTDSM_DEC2016
** Cell name: FIR_DAC
** View name: schematic
** terminal mapping: CLK	= clk
**                   IN	= in
**                   R1	= r1
**                   R2	= r2
**                   RSTB	= rstb
**                   VDD	= vdd
**                   VSS	= vss
.subckt type:mixed FIR_DAC clk in r1 r2 rstb vdd vss
xr19  r1 net3 vss   rppolywo_m lr=19.92e-6 wr=1.2e-6 multi=1 m=1

xr48  r2 in vss   rppolywo_m lr=19.92e-6 wr=1.2e-6 multi=1 m=1


** DFCNQD2BWP Instance I86 = hspiceD device xi86
** Instance of Lib: tcbn40lpbwp,  Cell: DFCNQD2BWP, View: schematic
xi86 in clk rstb net3 vdd vss DFCNQD2BWP
.ends FIR_DAC
** End of subcircuit definition.

** Library name: CTDSM_DEC2016
** Cell name: C1
** View name: schematic
** terminal mapping: A	= a
**                   B	= b
**                   VSS	= vss
.subckt type:analog C1 a b vss

** crtmom Instance C0<3:0> = hspiceD device xc0<3>
xc0<3> a b vss crtmom nv=204 nh=204 w=70e-9 s=70e-9 stm=1 spm=6 multi=1 ftip=140e-9 dmflag=0

** crtmom Instance C0<3:0> = hspiceD device xc0<2>
xc0<2> a b vss crtmom nv=204 nh=204 w=70e-9 s=70e-9 stm=1 spm=6 multi=1 ftip=140e-9 dmflag=0

** crtmom Instance C0<3:0> = hspiceD device xc0<1>
xc0<1> a b vss crtmom nv=204 nh=204 w=70e-9 s=70e-9 stm=1 spm=6 multi=1 ftip=140e-9 dmflag=0

** crtmom Instance C0<3:0> = hspiceD device xc0<0>
xc0<0> a b vss crtmom nv=204 nh=204 w=70e-9 s=70e-9 stm=1 spm=6 multi=1 ftip=140e-9 dmflag=0
.ends C1
** End of subcircuit definition.

** Library name: tcbn40lpbwp
** Cell name: NR2D8BWP
** View name: schematic
** terminal mapping: A1	= a1
**                   A2	= a2
**                   ZN	= zn
**                   VDD	= vdd
**                   VSS	= vss
.subckt type:digital NR2D8BWP a1 a2 zn vdd vss

** nch Instance MI1-M_u3 = hspiceD device m0
m0 zn a2 vss vss nch l=40e-9 w=2.48e-6 m=1 nf=1 sd=140e-9 ad=272.8e-15 as=272.8e-15 pd=5.18e-6 ps=5.18e-6 nrd=28.2258e-3 nrs=28.2258e-3 sa=110e-9 sb=110e-9

** nch Instance MI1-M_u4 = hspiceD device m1
m1 zn a1 vss vss nch l=40e-9 w=2.48e-6 m=1 nf=1 sd=140e-9 ad=272.8e-15 as=272.8e-15 pd=5.18e-6 ps=5.18e-6 nrd=28.2258e-3 nrs=28.2258e-3 sa=110e-9 sb=110e-9

** pch Instance MI1-M_u1 = hspiceD device m2
m2 net13 a2 vdd vdd pch l=40e-9 w=3.28e-6 m=1 nf=1 sd=140e-9 ad=360.8e-15 as=360.8e-15 pd=6.78e-6 ps=6.78e-6 nrd=21.3415e-3 nrs=21.3415e-3 sa=110e-9 sb=110e-9

** pch Instance MI1-M_u2 = hspiceD device m3
m3 zn a1 net13 vdd pch l=40e-9 w=3.28e-6 m=1 nf=1 sd=140e-9 ad=360.8e-15 as=360.8e-15 pd=6.78e-6 ps=6.78e-6 nrd=21.3415e-3 nrs=21.3415e-3 sa=110e-9 sb=110e-9
.ends NR2D8BWP
** End of subcircuit definition.

** Library name: CTDSM_DEC2016
** Cell name: SR_Latch
** View name: schematic
** terminal mapping: Q	= q
**                   QB	= qb
**                   R	= r
**                   S	= s
**                   VDD	= vdd
**                   VSS	= vss
.subckt type:digital SR_Latch q qb r s vdd vss

** NR2D8BWP Instance I1 = hspiceD device xi1
** Instance of Lib: tcbn40lpbwp,  Cell: NR2D8BWP, View: schematic
xi1 r qb q vdd vss NR2D8BWP

** NR2D8BWP Instance I0 = hspiceD device xi0
** Instance of Lib: tcbn40lpbwp,  Cell: NR2D8BWP, View: schematic
xi0 s q qb vdd vss NR2D8BWP
.ends SR_Latch
** End of subcircuit definition.

** Library name: CTDSM_DEC2016
** Cell name: Gm2_v5_Practice
** View name: schematic
** terminal mapping: IBIAS	= ibias
**                   VDD	= vdd
**                   VIM	= vim
**                   VIP	= vip
**                   VOM	= vom
**                   VOP	= vop
**                   VSS	= vss
.subckt type:analog Gm2_v5_Practice ibias vdd vim vip vom vop vss

** pch_lvt_mac Instance M20 = hspiceD device xm20
xm20 vdd ibias vdd vdd pch_lvt_mac l=3.6e-6 w=2.8e-6 multi=1 nf=1 sd=140e-9 ad=308e-15 as=308e-15 pd=5.82e-6 ps=5.82e-6 nrd=16.368e-3 nrs=16.368e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=3e-6 spa1=3e-6 spa2=3e-6 spa3=3e-6 sap=1.00025e-6 spba=1.83303e-6 sapb=577.831e-9 spba1=2.13766e-6

** pch_lvt_mac Instance M18 = hspiceD device xm18
xm18 vdd ibias vdd vdd pch_lvt_mac l=3.6e-6 w=2.8e-6 multi=1 nf=1 sd=140e-9 ad=308e-15 as=308e-15 pd=5.82e-6 ps=5.82e-6 nrd=16.368e-3 nrs=16.368e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=3e-6 spa1=3e-6 spa2=3e-6 spa3=3e-6 sap=1.00025e-6 spba=1.83303e-6 sapb=577.831e-9 spba1=2.13766e-6

** nch_lvt_dnw_mac Instance M13 = hspiceD device xm13
xm13 vop vim net100 net100 nch_lvt_mac l=160e-9 w=1.16e-6 multi=1 nf=4 sd=160e-9 ad=92.8e-15 as=110.2e-15 pd=1.8e-6 ps=2.5e-6 nrd=27.385e-3 nrs=27.385e-3 sa=350.185e-9 sb=350.185e-9 sa1=182.144e-9 sa2=316.736e-9 sa3=531.884e-9 sa4=293.341e-9 sb1=182.144e-9 sb2=316.736e-9 sb3=531.884e-9 spa=293.52e-9 spa1=234.251e-9 spa2=177.709e-9 spa3=196.848e-9 sap=282.393e-9 spba=266.358e-9 sapb=476.716e-9 spba1=272.932e-9

** nch_lvt_dnw_mac Instance M21 = hspiceD device xm21
xm21 vom vip net100 net100 nch_lvt_mac l=160e-9 w=1.16e-6 multi=1 nf=4 sd=160e-9 ad=92.8e-15 as=110.2e-15 pd=1.8e-6 ps=2.5e-6 nrd=27.385e-3 nrs=27.385e-3 sa=350.185e-9 sb=350.185e-9 sa1=182.144e-9 sa2=316.736e-9 sa3=531.884e-9 sa4=293.341e-9 sb1=182.144e-9 sb2=316.736e-9 sb3=531.884e-9 spa=293.52e-9 spa1=234.251e-9 spa2=177.709e-9 spa3=196.848e-9 sap=282.393e-9 spba=266.358e-9 sapb=476.716e-9 spba1=272.932e-9

** pch_mac Instance M0 = hspiceD device xm0
xm0 ibias ibias vdd vdd pch_mac l=160e-9 w=700e-9 multi=1 nf=2 sd=160e-9 ad=56e-15 as=77e-15 pd=1.02e-6 ps=1.84e-6 nrd=87.521e-3 nrs=87.521e-3 sa=196.857e-9 sb=196.857e-9 sa1=141.575e-9 sa2=190.476e-9 sa3=316.284e-9 sa4=175.574e-9 sb1=141.575e-9 sb2=190.476e-9 sb3=316.284e-9 spa=526.132e-9 spa1=356.828e-9 spa2=205.731e-9 spa3=258.385e-9 sap=280.768e-9 spba=836.217e-9 sapb=685.762e-9 spba1=838.359e-9

** pch_mac Instance M24 = hspiceD device xm24
xm24 ibias ibias vdd vdd pch_mac l=160e-9 w=700e-9 multi=1 nf=2 sd=160e-9 ad=56e-15 as=77e-15 pd=1.02e-6 ps=1.84e-6 nrd=87.521e-3 nrs=87.521e-3 sa=196.857e-9 sb=196.857e-9 sa1=141.575e-9 sa2=190.476e-9 sa3=316.284e-9 sa4=175.574e-9 sb1=141.575e-9 sb2=190.476e-9 sb3=316.284e-9 spa=526.132e-9 spa1=356.828e-9 spa2=205.731e-9 spa3=258.385e-9 sap=280.768e-9 spba=836.217e-9 sapb=685.762e-9 spba1=838.359e-9

** pch_mac Instance M23 = hspiceD device xm23
xm23 vop ibias vdd vdd pch_mac l=160e-9 w=1.4e-6 multi=1 nf=4 sd=160e-9 ad=112e-15 as=133e-15 pd=2.04e-6 ps=2.86e-6 nrd=40.813e-3 nrs=40.813e-3 sa=350.185e-9 sb=350.185e-9 sa1=182.144e-9 sa2=316.736e-9 sa3=531.884e-9 sa4=293.341e-9 sb1=182.144e-9 sb2=316.736e-9 sb3=531.884e-9 spa=293.52e-9 spa1=234.251e-9 spa2=177.709e-9 spa3=196.848e-9 sap=282.393e-9 spba=266.358e-9 sapb=476.716e-9 spba1=272.932e-9

** pch_mac Instance M14 = hspiceD device xm14
xm14 vom ibias vdd vdd pch_mac l=160e-9 w=1.4e-6 multi=1 nf=4 sd=160e-9 ad=112e-15 as=133e-15 pd=2.04e-6 ps=2.86e-6 nrd=40.813e-3 nrs=40.813e-3 sa=350.185e-9 sb=350.185e-9 sa1=182.144e-9 sa2=316.736e-9 sa3=531.884e-9 sa4=293.341e-9 sb1=182.144e-9 sb2=316.736e-9 sb3=531.884e-9 spa=293.52e-9 spa1=234.251e-9 spa2=177.709e-9 spa3=196.848e-9 sap=282.393e-9 spba=266.358e-9 sapb=476.716e-9 spba1=272.932e-9

** crtmom Instance C22 = hspiceD device xc22
xc22 vop ntail2 vss crtmom nv=16 nh=16 w=70e-9 s=70e-9 stm=1 spm=6 multi=1 ftip=140e-9 dmflag=0

** crtmom Instance C21 = hspiceD device xc21
xc21 ntail2 vom vss crtmom nv=16 nh=16 w=70e-9 s=70e-9 stm=1 spm=6 multi=1 ftip=140e-9 dmflag=0
**Series configuration of R11
xr11  vom ntail2 vss  rppolywo_m lr=6.6e-6 wr=400e-9 multi=1 m=1
**End of R11

**Series configuration of R12
xr12  ntail2 vop vss  rppolywo_m lr=6.6e-6 wr=400e-9 multi=1 m=1
**End of R12


** nch_mac Instance M22 = hspiceD device xm22
xm22 net100 ntail2 vss vss nch_mac l=160e-9 w=3.12e-6 multi=1 nf=4 sd=160e-9 ad=249.6e-15 as=296.4e-15 pd=3.76e-6 ps=5.44e-6 nrd=11.785e-3 nrs=11.785e-3 sa=350.185e-9 sb=350.185e-9 sa1=182.144e-9 sa2=316.736e-9 sa3=531.884e-9 sa4=293.341e-9 sb1=182.144e-9 sb2=316.736e-9 sb3=531.884e-9 spa=293.52e-9 spa1=234.251e-9 spa2=177.709e-9 spa3=196.848e-9 sap=282.393e-9 spba=266.358e-9 sapb=476.716e-9 spba1=272.932e-9

** dnwpsub Instance D1 = hspiceD device d1
d1 vss vdd dnwpsub area=15.12e-12 pj=16.08e-6 m=1

** nch_lvt_mac Instance M12 = hspiceD device xm12
xm12 vss ntail2 vss vss nch_lvt_mac l=2.2e-6 w=2.1e-6 multi=1 nf=1 sd=140e-9 ad=231e-15 as=231e-15 pd=4.42e-6 ps=4.42e-6 nrd=15.024e-3 nrs=15.024e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=3e-6 spa1=3e-6 spa2=3e-6 spa3=3e-6 sap=1.00025e-6 spba=1.79444e-6 sapb=577.831e-9 spba1=1.98202e-6

** nch_lvt_mac Instance M11 = hspiceD device xm11
xm11 vss ntail2 vss vss nch_lvt_mac l=2.2e-6 w=2.1e-6 multi=1 nf=1 sd=140e-9 ad=231e-15 as=231e-15 pd=4.42e-6 ps=4.42e-6 nrd=15.024e-3 nrs=15.024e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=3e-6 spa1=3e-6 spa2=3e-6 spa3=3e-6 sap=1.00025e-6 spba=1.79444e-6 sapb=577.831e-9 spba1=1.98202e-6

** pwdnw Instance D0 = hspiceD device d0
d0 net100 vdd pwdnw area=9.328e-12 pj=12.88e-6 m=1
.ends Gm2_v5_Practice
** End of subcircuit definition.

** Library name: CTDSM_DEC2016
** Cell name: myComparator_v3
** View name: schematic
** terminal mapping: CLK	= clk
**                   GND	= gnd
**                   OUTM	= outm
**                   OUTP	= outp
**                   VDD	= vdd
**                   VI+	= _net0
**                   VI-	= _net1
.subckt type:analog myComparator_v3 clk gnd outm outp vdd _net0 _net1

** nch_lvt_mac Instance M0 = hspiceD device xm0
xm0 gnd intern gnd gnd nch_lvt_mac l=1e-6 w=1.05e-6 multi=1 nf=1 sd=140e-9 ad=115.5e-15 as=115.5e-15 pd=2.32e-6 ps=2.32e-6 nrd=34.495e-3 nrs=34.495e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=3e-6 spa1=3e-6 spa2=3e-6 spa3=3e-6 sap=1.00025e-6 spba=1.76068e-6 sapb=577.831e-9 spba1=1.84662e-6

** nch_lvt_mac Instance M22 = hspiceD device xm22
xm22 gnd interp gnd gnd nch_lvt_mac l=1e-6 w=1.05e-6 multi=1 nf=1 sd=140e-9 ad=115.5e-15 as=115.5e-15 pd=2.32e-6 ps=2.32e-6 nrd=34.495e-3 nrs=34.495e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=3e-6 spa1=3e-6 spa2=3e-6 spa3=3e-6 sap=1.00025e-6 spba=1.76068e-6 sapb=577.831e-9 spba1=1.84662e-6

** nch_lvt_mac Instance M16 = hspiceD device xm16
xm16 outm crossp gnd gnd nch_lvt_mac l=40e-9 w=1.44e-6 multi=1 nf=4 sd=140e-9 ad=100.8e-15 as=129.6e-15 pd=2e-6 ps=2.88e-6 nrd=38.421e-3 nrs=38.421e-3 sa=256.788e-9 sb=256.788e-9 sa1=172.516e-9 sa2=246.369e-9 sa3=404.983e-9 sa4=247.435e-9 sb1=172.516e-9 sb2=246.369e-9 sb3=404.983e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=170.416e-9 spba=168.903e-9 sapb=240.571e-9 spba1=170.77e-9

** nch_lvt_mac Instance M17 = hspiceD device xm17
xm17 outp crossn gnd gnd nch_lvt_mac l=40e-9 w=1.44e-6 multi=1 nf=4 sd=140e-9 ad=100.8e-15 as=129.6e-15 pd=2e-6 ps=2.88e-6 nrd=38.421e-3 nrs=38.421e-3 sa=256.788e-9 sb=256.788e-9 sa1=172.516e-9 sa2=246.369e-9 sa3=404.983e-9 sa4=247.435e-9 sb1=172.516e-9 sb2=246.369e-9 sb3=404.983e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=170.416e-9 spba=168.903e-9 sapb=240.571e-9 spba1=170.77e-9

** nch_lvt_mac Instance M4 = hspiceD device xm4
xm4 crossn crossp intern gnd nch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 sd=140e-9 ad=134.4e-15 as=172.8e-15 pd=2.48e-6 ps=3.6e-6 nrd=22.768e-3 nrs=22.768e-3 sa=256.788e-9 sb=256.788e-9 sa1=172.516e-9 sa2=246.369e-9 sa3=404.983e-9 sa4=247.435e-9 sb1=172.516e-9 sb2=246.369e-9 sb3=404.983e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=170.416e-9 spba=168.903e-9 sapb=240.571e-9 spba1=170.77e-9

** nch_lvt_mac Instance M3 = hspiceD device xm3
xm3 crossp crossn interp gnd nch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 sd=140e-9 ad=134.4e-15 as=172.8e-15 pd=2.48e-6 ps=3.6e-6 nrd=22.768e-3 nrs=22.768e-3 sa=256.788e-9 sb=256.788e-9 sa1=172.516e-9 sa2=246.369e-9 sa3=404.983e-9 sa4=247.435e-9 sb1=172.516e-9 sb2=246.369e-9 sb3=404.983e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=170.416e-9 spba=168.903e-9 sapb=240.571e-9 spba1=170.77e-9

** nch_lvt_mac Instance M7 = hspiceD device xm7
xm7 net069 clk gnd gnd nch_lvt_mac l=40e-9 w=6.9e-6 multi=1 nf=15 sd=140e-9 ad=501.4e-15 as=501.4e-15 pd=9.54e-6 ps=9.54e-6 nrd=5.906e-3 nrs=5.906e-3 sa=675.603e-9 sb=675.603e-9 sa1=269.666e-9 sa2=545.885e-9 sa3=818.344e-9 sa4=642.475e-9 sb1=269.666e-9 sb2=545.885e-9 sb3=818.344e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=234.015e-9 spba=162.859e-9 sapb=280.924e-9 spba1=164.658e-9

** nch_lvt_mac Instance M5 = hspiceD device xm5
xm5 intern _net0 net069 gnd nch_lvt_mac l=40e-9 w=14.4e-6 multi=1 nf=15 sd=140e-9 ad=1.0464e-12 as=1.0464e-12 pd=17.54e-6 ps=17.54e-6 nrd=2.264e-3 nrs=2.264e-3 sa=675.603e-9 sb=675.603e-9 sa1=269.666e-9 sa2=545.885e-9 sa3=818.344e-9 sa4=642.475e-9 sb1=269.666e-9 sb2=545.885e-9 sb3=818.344e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=234.015e-9 spba=162.859e-9 sapb=280.924e-9 spba1=164.658e-9

** nch_lvt_mac Instance M6 = hspiceD device xm6
xm6 interp _net1 net069 gnd nch_lvt_mac l=40e-9 w=14.4e-6 multi=1 nf=15 sd=140e-9 ad=1.0464e-12 as=1.0464e-12 pd=17.54e-6 ps=17.54e-6 nrd=2.264e-3 nrs=2.264e-3 sa=675.603e-9 sb=675.603e-9 sa1=269.666e-9 sa2=545.885e-9 sa3=818.344e-9 sa4=642.475e-9 sb1=269.666e-9 sb2=545.885e-9 sb3=818.344e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=234.015e-9 spba=162.859e-9 sapb=280.924e-9 spba1=164.658e-9

** pch_lvt_mac Instance M8 = hspiceD device xm8
xm8 outm crossp vdd vdd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 sd=140e-9 ad=134.4e-15 as=172.8e-15 pd=2.48e-6 ps=3.6e-6 nrd=28.999e-3 nrs=28.999e-3 sa=256.788e-9 sb=256.788e-9 sa1=172.516e-9 sa2=246.369e-9 sa3=404.983e-9 sa4=247.435e-9 sb1=172.516e-9 sb2=246.369e-9 sb3=404.983e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=170.416e-9 spba=168.903e-9 sapb=240.571e-9 spba1=170.77e-9

** pch_lvt_mac Instance M18 = hspiceD device xm18
xm18 intern clk vdd vdd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 sd=140e-9 ad=134.4e-15 as=172.8e-15 pd=2.48e-6 ps=3.6e-6 nrd=28.999e-3 nrs=28.999e-3 sa=256.788e-9 sb=256.788e-9 sa1=172.516e-9 sa2=246.369e-9 sa3=404.983e-9 sa4=247.435e-9 sb1=172.516e-9 sb2=246.369e-9 sb3=404.983e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=170.416e-9 spba=168.903e-9 sapb=240.571e-9 spba1=170.77e-9

** pch_lvt_mac Instance M15 = hspiceD device xm15
xm15 outp crossn vdd vdd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 sd=140e-9 ad=134.4e-15 as=172.8e-15 pd=2.48e-6 ps=3.6e-6 nrd=28.999e-3 nrs=28.999e-3 sa=256.788e-9 sb=256.788e-9 sa1=172.516e-9 sa2=246.369e-9 sa3=404.983e-9 sa4=247.435e-9 sb1=172.516e-9 sb2=246.369e-9 sb3=404.983e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=170.416e-9 spba=168.903e-9 sapb=240.571e-9 spba1=170.77e-9

** pch_lvt_mac Instance M2 = hspiceD device xm2
xm2 interp clk vdd vdd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 sd=140e-9 ad=134.4e-15 as=172.8e-15 pd=2.48e-6 ps=3.6e-6 nrd=28.999e-3 nrs=28.999e-3 sa=256.788e-9 sb=256.788e-9 sa1=172.516e-9 sa2=246.369e-9 sa3=404.983e-9 sa4=247.435e-9 sb1=172.516e-9 sb2=246.369e-9 sb3=404.983e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=170.416e-9 spba=168.903e-9 sapb=240.571e-9 spba1=170.77e-9

** pch_lvt_mac Instance M1 = hspiceD device xm1
xm1 crossn clk vdd vdd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 sd=140e-9 ad=134.4e-15 as=172.8e-15 pd=2.48e-6 ps=3.6e-6 nrd=28.999e-3 nrs=28.999e-3 sa=256.788e-9 sb=256.788e-9 sa1=172.516e-9 sa2=246.369e-9 sa3=404.983e-9 sa4=247.435e-9 sb1=172.516e-9 sb2=246.369e-9 sb3=404.983e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=170.416e-9 spba=168.903e-9 sapb=240.571e-9 spba1=170.77e-9

** pch_lvt_mac Instance M12 = hspiceD device xm12
xm12 crossp clk vdd vdd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 sd=140e-9 ad=134.4e-15 as=172.8e-15 pd=2.48e-6 ps=3.6e-6 nrd=28.999e-3 nrs=28.999e-3 sa=256.788e-9 sb=256.788e-9 sa1=172.516e-9 sa2=246.369e-9 sa3=404.983e-9 sa4=247.435e-9 sb1=172.516e-9 sb2=246.369e-9 sb3=404.983e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=170.416e-9 spba=168.903e-9 sapb=240.571e-9 spba1=170.77e-9

** pch_lvt_mac Instance M14 = hspiceD device xm14
xm14 crossn crossp vdd vdd pch_lvt_mac l=40e-9 w=3.84e-6 multi=1 nf=8 sd=140e-9 ad=268.8e-15 as=307.2e-15 pd=4.96e-6 ps=6.08e-6 nrd=14.333e-3 nrs=14.333e-3 sa=420.901e-9 sb=420.901e-9 sa1=218.025e-9 sa2=376.982e-9 sa3=612.324e-9 sa4=401.816e-9 sb1=218.025e-9 sb2=376.982e-9 sb3=612.324e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=200.193e-9 spba=164.62e-9 sapb=257.228e-9 spba1=166.439e-9

** pch_lvt_mac Instance M13 = hspiceD device xm13
xm13 crossp crossn vdd vdd pch_lvt_mac l=40e-9 w=3.84e-6 multi=1 nf=8 sd=140e-9 ad=268.8e-15 as=307.2e-15 pd=4.96e-6 ps=6.08e-6 nrd=14.333e-3 nrs=14.333e-3 sa=420.901e-9 sb=420.901e-9 sa1=218.025e-9 sa2=376.982e-9 sa3=612.324e-9 sa4=401.816e-9 sb1=218.025e-9 sb2=376.982e-9 sb3=612.324e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=200.193e-9 spba=164.62e-9 sapb=257.228e-9 spba1=166.439e-9
.ends myComparator_v3
** End of subcircuit definition.

** Library name: New_FIR
** Cell name: FIR_DAC
** View name: schematic
** terminal mapping: CLK	= clk
**                   IN	= in
**                   R1	= r1
**                   R2	= r2
**                   RSTB	= rstb
**                   VDD	= vdd
**                   VSS	= vss
.subckt type:mixed FIR_DAC_schematic clk in r1 r2 rstb vdd vss
xr19  r1 net3 vss   rppolywo_m lr=19.92e-6 wr=1.2e-6 multi=1 m=1

xr48  r2 in vss   rppolywo_m lr=19.92e-6 wr=1.2e-6 multi=1 m=1


** DFCNQD2BWP Instance I86 = hspiceD device xi86
** Instance of Lib: tcbn40lpbwp,  Cell: DFCNQD2BWP, View: schematic
xi86 in clk rstb net3 vdd vss DFCNQD2BWP
.ends FIR_DAC_schematic
** End of subcircuit definition.

** Library name: New_FIR
** Cell name: C_DAC
** View name: schematic
** terminal mapping: CLKB	= clkb
**                   IN	= in
**                   R3	= r3
**                   R4	= r4
**                   RSTB	= rstb
**                   VDD	= vdd
**                   VSS	= vss
.subckt type:analog C_DAC_schematic clkb in r3 r4 rstb vdd vss
**Series configuration of R27
xr27  r3 net10 vss  rppolywo_m lr=19.92e-6 wr=1.2e-6 multi=1 m=1
**End of R27

**Series configuration of R64
xr64 r4 in vss  rppolywo_m lr=18.67e-6 wr=1.2e-6 multi=1 m=1
**End of R64


** DFCNQD2BWP Instance I94 = hspiceD device xi94
** Instance of Lib: tcbn40lpbwp,  Cell: DFCNQD2BWP, View: schematic
xi94 in clkb rstb net10 vdd vss DFCNQD2BWP
.ends C_DAC_schematic
** End of subcircuit definition.

** Library name: CONST_EXTR
** Cell name: CTDSM_CORE_NEW
** View name: schematic
.topckt CTDSM_CORE_NEW clk clkb1 clkb2 ibias1 ibias2 outm outp rstb vdda vddd vim vip vss
** terminal mapping: CLK	= clk
**                   CLKB1	= clkb1
**                   CLKB2	= clkb2
**                   IBIAS1	= ibias1
**                   IBIAS2	= ibias2
**                   OUTM	= outm
**                   OUTP	= outp
**                   RSTB	= rstb
**                   VDDA	= vdda
**                   VDDD	= vddd
**                   VIM	= vim
**                   VIP	= vip
**                   VSS	= vss

** Gm1_v5_Practice Instance I160 = hspiceD device xi160
** Instance of Lib: CTDSM_DEC2016,  Cell: Gm1_v5_Practice, View: schematic
xi160 ibias1 vdda vo1m vo1p vo2p vo2m vss Gm1_v5_Practice

** C_DAC Instance I154 = hspiceD device xi154
** Instance of Lib: CTDSM_DEC2016,  Cell: C_DAC, View: schematic
xi154 clkb1 net062 vo3m vo3p rstb vdda vss FIR_DAC

** FIR_DAC Instance I152 = hspiceD device xi152
** Instance of Lib: CTDSM_DEC2016,  Cell: FIR_DAC, View: schematic
xi152 clk net052 vo1p vo1p rstb vdda vss FIR_DAC

** nch Instance M1 = hspiceD device m1
m1 vss clkb2 vss vss nch l=280e-9 w=280e-9 m=1 nf=1 sd=140e-9 ad=30.8e-15 as=30.8e-15 pd=780e-9 ps=780e-9 nrd=112.683e-3 nrs=112.683e-3 sa=110e-9 sb=110e-9

** nch Instance M0 = hspiceD device m0
m0 vss clkb1 vss vss nch l=280e-9 w=280e-9 m=1 nf=1 sd=140e-9 ad=30.8e-15 as=30.8e-15 pd=780e-9 ps=780e-9 nrd=112.683e-3 nrs=112.683e-3 sa=110e-9 sb=110e-9

** C1 Instance I164 = hspiceD device xi164
** Instance of Lib: CTDSM_DEC2016,  Cell: C1, View: schematic
xi164 vo1p vo1m vss C1

** SR_Latch Instance I128 = hspiceD device xi128
** Instance of Lib: CTDSM_DEC2016,  Cell: SR_Latch, View: schematic
xi128 outm outp net072 net071 vddd vss SR_Latch

** crtmom Instance C6 = hspiceD device xc6
xc6 vo3p vo3m vss crtmom nv=184 nh=182 w=70e-9 s=70e-9 stm=1 spm=6 multi=1 ftip=140e-9 dmflag=0

** crtmom Instance C3 = hspiceD device xc3
xc3 net074 net073 vss crtmom nv=178 nh=178 w=70e-9 s=70e-9 stm=1 spm=6 multi=1 ftip=140e-9 dmflag=0
xr16  vip vo1p vss   rppolywo_m lr=19.92e-6 wr=1.2e-6 multi=2 m=1

xr51  net073 vo2m vss   rppolywo_m lr=19.92e-6 wr=1.2e-6 multi=5 m=1

xr25  vo2p net074 vss   rppolywo_m lr=19.92e-6 wr=1.2e-6 multi=5 m=1

xr47  vim vo1m vss   rppolywo_m lr=19.92e-6 wr=1.2e-6 multi=2 m=1


** Gm2_v5_Practice Instance I161 = hspiceD device xi161
** Instance of Lib: CTDSM_DEC2016,  Cell: Gm2_v5_Practice, View: schematic
xi161 ibias2 vdda vo2m vo2p vo3p vo3m vss Gm1_v5_Practice

** DFCNQD2BWP Instance I88 = hspiceD device xi88
** Instance of Lib: tcbn40lpbwp,  Cell: DFCNQD2BWP, View: schematic
xi88 net062 clk rstb net052 vdda vss DFCNQD2BWP

** DFCNQD2BWP Instance I97 = hspiceD device xi97
** Instance of Lib: tcbn40lpbwp,  Cell: DFCNQD2BWP, View: schematic
xi97 outp clkb1 rstb net062 vdda vss DFCNQD2BWP

** DFCNQD2BWP Instance I92 = hspiceD device xi92
** Instance of Lib: tcbn40lpbwp,  Cell: DFCNQD2BWP, View: schematic
xi92 net063 clk rstb net051 vdda vss DFCNQD2BWP

** DFCNQD2BWP Instance I99 = hspiceD device xi99
** Instance of Lib: tcbn40lpbwp,  Cell: DFCNQD2BWP, View: schematic
xi99 outm clkb2 rstb net063 vdda vss DFCNQD2BWP

** myComparator_v3 Instance I146 = hspiceD device xi146
** Instance of Lib: CTDSM_DEC2016,  Cell: myComparator_v3, View: schematic
xi146 clk vss net072 net071 vddd vo3p vo3m myComparator_v3

** FIR_DAC Instance I153 = hspiceD device xi153
** Instance of Lib: New_FIR,  Cell: FIR_DAC_schematic, View: schematic
xi153 clk net051 vo1m vo1m rstb vdda vss FIR_DAC

** C_DAC Instance I155 = hspiceD device xi155
** Instance of Lib: New_FIR,  Cell: C_DAC_schematic, View: schematic
xi155 clkb2 net063 vo3p vo3m rstb vdda vss FIR_DAC
.ends CTDSM_CORE_NEW
** End of subcircuit definition.
