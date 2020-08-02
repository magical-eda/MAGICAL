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
xm20 vdd ibias vdd vdd pch_lvt_mac l=3.6e-6 w=2.8e-6 multi=1 nf=1 sd=140e-9 ad=308e-15 as=308e-15 pd=5.82e-6 ps=5.82e-6 nrd=16.368e-3 nrs=16.368e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=3e-6 spa1=3e-6 spa2=3e-6 spa3=3e-6 sap=1.00025e-6 spba=1.83303e-6 sapb=577.831e-9 spba1=2.13766e-6
xm18 vdd ibias vdd vdd pch_lvt_mac l=3.6e-6 w=2.8e-6 multi=1 nf=1 sd=140e-9 ad=308e-15 as=308e-15 pd=5.82e-6 ps=5.82e-6 nrd=16.368e-3 nrs=16.368e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=3e-6 spa1=3e-6 spa2=3e-6 spa3=3e-6 sap=1.00025e-6 spba=1.83303e-6 sapb=577.831e-9 spba1=2.13766e-6
xc0 ntail2 vop cfmom_2t nr=46 lr=1e-6 w=70e-9 s=70e-9 stm=2 spm=6 multi=1 ftip=140e-9
xc1 ntail2 vom cfmom_2t nr=46 lr=1e-6 w=70e-9 s=70e-9 stm=2 spm=6 multi=1 ftip=140e-9
xm0 ibias ibias vdd vdd pch_mac l=160e-9 w=700e-9 multi=1 nf=2 sd=160e-9 ad=56e-15 as=77e-15 pd=1.02e-6 ps=1.84e-6 nrd=87.521e-3 nrs=87.521e-3 sa=196.857e-9 sb=196.857e-9 sa1=141.575e-9 sa2=190.476e-9 sa3=316.284e-9 sa4=175.574e-9 sb1=141.575e-9 sb2=190.476e-9 sb3=316.284e-9 spa=526.132e-9 spa1=356.828e-9 spa2=205.731e-9 spa3=258.385e-9 sap=280.768e-9 spba=836.217e-9 sapb=685.762e-9 spba1=838.359e-9
xm24 ibias ibias vdd vdd pch_mac l=160e-9 w=700e-9 multi=1 nf=2 sd=160e-9 ad=56e-15 as=77e-15 pd=1.02e-6 ps=1.84e-6 nrd=87.521e-3 nrs=87.521e-3 sa=196.857e-9 sb=196.857e-9 sa1=141.575e-9 sa2=190.476e-9 sa3=316.284e-9 sa4=175.574e-9 sb1=141.575e-9 sb2=190.476e-9 sb3=316.284e-9 spa=526.132e-9 spa1=356.828e-9 spa2=205.731e-9 spa3=258.385e-9 sap=280.768e-9 spba=836.217e-9 sapb=685.762e-9 spba1=838.359e-9
xm23 vop ibias vdd vdd pch_mac l=160e-9 w=1.4e-6 multi=1 nf=4 sd=160e-9 ad=112e-15 as=133e-15 pd=2.04e-6 ps=2.86e-6 nrd=40.813e-3 nrs=40.813e-3 sa=350.185e-9 sb=350.185e-9 sa1=182.144e-9 sa2=316.736e-9 sa3=531.884e-9 sa4=293.341e-9 sb1=182.144e-9 sb2=316.736e-9 sb3=531.884e-9 spa=293.52e-9 spa1=234.251e-9 spa2=177.709e-9 spa3=196.848e-9 sap=282.393e-9 spba=266.358e-9 sapb=476.716e-9 spba1=272.932e-9
xm14 vom ibias vdd vdd pch_mac l=160e-9 w=1.4e-6 multi=1 nf=4 sd=160e-9 ad=112e-15 as=133e-15 pd=2.04e-6 ps=2.86e-6 nrd=40.813e-3 nrs=40.813e-3 sa=350.185e-9 sb=350.185e-9 sa1=182.144e-9 sa2=316.736e-9 sa3=531.884e-9 sa4=293.341e-9 sb1=182.144e-9 sb2=316.736e-9 sb3=531.884e-9 spa=293.52e-9 spa1=234.251e-9 spa2=177.709e-9 spa3=196.848e-9 sap=282.393e-9 spba=266.358e-9 sapb=476.716e-9 spba1=272.932e-9
**Series configuration of R11
xr11 vom ntail2 vss  rppolywo_m lr=6.6e-6 wr=400e-9 multi=1 m=1 series=9 segspace=250e-9
**End of R11

**Series configuration of R12
xr12 ntail2 vop vss  rppolywo_m lr=6.6e-6 wr=400e-9 multi=1 m=1 series=9 segspace=250e-9
**End of R12

xm22 net100 ntail2 vss vss nch_mac l=160e-9 w=3.12e-6 multi=1 nf=4 sd=160e-9 ad=249.6e-15 as=296.4e-15 pd=3.76e-6 ps=5.44e-6 nrd=11.785e-3 nrs=11.785e-3 sa=350.185e-9 sb=350.185e-9 sa1=182.144e-9 sa2=316.736e-9 sa3=531.884e-9 sa4=293.341e-9 sb1=182.144e-9 sb2=316.736e-9 sb3=531.884e-9 spa=293.52e-9 spa1=234.251e-9 spa2=177.709e-9 spa3=196.848e-9 sap=282.393e-9 spba=266.358e-9 sapb=476.716e-9 spba1=272.932e-9
xm12 vss ntail2 vss vss nch_lvt_mac l=2.2e-6 w=2.1e-6 multi=1 nf=1 sd=140e-9 ad=231e-15 as=231e-15 pd=4.42e-6 ps=4.42e-6 nrd=15.024e-3 nrs=15.024e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=3e-6 spa1=3e-6 spa2=3e-6 spa3=3e-6 sap=1.00025e-6 spba=1.79444e-6 sapb=577.831e-9 spba1=1.98202e-6
xm11 vss ntail2 vss vss nch_lvt_mac l=2.2e-6 w=2.1e-6 multi=1 nf=1 sd=140e-9 ad=231e-15 as=231e-15 pd=4.42e-6 ps=4.42e-6 nrd=15.024e-3 nrs=15.024e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=3e-6 spa1=3e-6 spa2=3e-6 spa3=3e-6 sap=1.00025e-6 spba=1.79444e-6 sapb=577.831e-9 spba1=1.98202e-6
xm13 vop vim net100 vss nch_lvt_mac l=160e-9 w=1.16e-6 multi=1 nf=4 sd=160e-9 ad=92.8e-15 as=110.2e-15 pd=1.8e-6 ps=2.5e-6 nrd=27.385e-3 nrs=27.385e-3 sa=350.185e-9 sb=350.185e-9 sa1=182.144e-9 sa2=316.736e-9 sa3=531.884e-9 sa4=293.341e-9 sb1=182.144e-9 sb2=316.736e-9 sb3=531.884e-9 spa=293.52e-9 spa1=234.251e-9 spa2=177.709e-9 spa3=196.848e-9 sap=282.393e-9 spba=266.358e-9 sapb=476.716e-9 spba1=272.932e-9
xm21 vom vip net100 vss nch_lvt_mac l=160e-9 w=1.16e-6 multi=1 nf=4 sd=160e-9 ad=92.8e-15 as=110.2e-15 pd=1.8e-6 ps=2.5e-6 nrd=27.385e-3 nrs=27.385e-3 sa=350.185e-9 sb=350.185e-9 sa1=182.144e-9 sa2=316.736e-9 sa3=531.884e-9 sa4=293.341e-9 sb1=182.144e-9 sb2=316.736e-9 sb3=531.884e-9 spa=293.52e-9 spa1=234.251e-9 spa2=177.709e-9 spa3=196.848e-9 sap=282.393e-9 spba=266.358e-9 sapb=476.716e-9 spba1=272.932e-9
.ends Gm2_v5_Practice_schematic
** End of subcircuit definition.

** Library name: 2019_CTDSM_MAGICAL_TAPEOUT
** Cell name: Gm1_v5_Practice
** View name: schematic
.subckt Gm1_v5_Practice_schematic ibias vdd vim vip vom vop vss
xm8 net08 ntail1 vss vss nch_hvt_mac l=120e-9 w=3.22e-6 multi=1 nf=4 sd=140e-9 ad=225.4e-15 as=289.8e-15 pd=3.78e-6 ps=5.55e-6 nrd=13.746e-3 nrs=13.746e-3 sa=313.488e-9 sb=313.488e-9 sa1=179.031e-9 sa2=290.688e-9 sa3=485.728e-9 sa4=276.641e-9 sb1=179.031e-9 sb2=290.688e-9 sb3=485.728e-9 spa=269.255e-9 spa1=211.397e-9 spa2=155.295e-9 spa3=170.776e-9 sap=244.923e-9 spba=223.131e-9 sapb=396.954e-9 spba1=228.22e-9
xm2 vdd ibias vdd vdd pch_lvt_mac l=3.3e-6 w=2.95e-6 multi=1 nf=1 sd=140e-9 ad=324.5e-15 as=324.5e-15 pd=6.12e-6 ps=6.12e-6 nrd=15.641e-3 nrs=15.641e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=3e-6 spa1=3e-6 spa2=3e-6 spa3=3e-6 sap=1.00025e-6 spba=1.82483e-6 sapb=577.831e-9 spba1=2.1045e-6
xm4 vdd ibias vdd vdd pch_lvt_mac l=3.3e-6 w=2.95e-6 multi=1 nf=1 sd=140e-9 ad=324.5e-15 as=324.5e-15 pd=6.12e-6 ps=6.12e-6 nrd=15.641e-3 nrs=15.641e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=3e-6 spa1=3e-6 spa2=3e-6 spa3=3e-6 sap=1.00025e-6 spba=1.82483e-6 sapb=577.831e-9 spba1=2.1045e-6
xm12 ibias ibias vdd vdd pch_lvt_mac l=120e-9 w=585e-9 multi=1 nf=1 sd=140e-9 ad=64.35e-15 as=64.35e-15 pd=1.39e-6 ps=1.39e-6 nrd=85.492e-3 nrs=85.492e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=3e-6 spa1=3e-6 spa2=3e-6 spa3=3e-6 sap=1.00025e-6 spba=1.73551e-6 sapb=577.831e-9 spba1=1.74589e-6
xm11 vom ibias vdd vdd pch_lvt_mac l=120e-9 w=2.34e-6 multi=1 nf=4 sd=140e-9 ad=163.8e-15 as=210.6e-15 pd=2.9e-6 ps=4.23e-6 nrd=19.88e-3 nrs=19.88e-3 sa=313.488e-9 sb=313.488e-9 sa1=179.031e-9 sa2=290.688e-9 sa3=485.728e-9 sa4=276.641e-9 sb1=179.031e-9 sb2=290.688e-9 sb3=485.728e-9 spa=269.255e-9 spa1=211.397e-9 spa2=155.295e-9 spa3=170.776e-9 sap=244.923e-9 spba=223.131e-9 sapb=396.954e-9 spba1=228.22e-9
xm15 ibias ibias vdd vdd pch_lvt_mac l=120e-9 w=585e-9 multi=1 nf=1 sd=140e-9 ad=64.35e-15 as=64.35e-15 pd=1.39e-6 ps=1.39e-6 nrd=85.492e-3 nrs=85.492e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=3e-6 spa1=3e-6 spa2=3e-6 spa3=3e-6 sap=1.00025e-6 spba=1.73551e-6 sapb=577.831e-9 spba1=1.74589e-6
xm14 vop ibias vdd vdd pch_lvt_mac l=120e-9 w=2.34e-6 multi=1 nf=4 sd=140e-9 ad=163.8e-15 as=210.6e-15 pd=2.9e-6 ps=4.23e-6 nrd=19.88e-3 nrs=19.88e-3 sa=313.488e-9 sb=313.488e-9 sa1=179.031e-9 sa2=290.688e-9 sa3=485.728e-9 sa4=276.641e-9 sb1=179.031e-9 sb2=290.688e-9 sb3=485.728e-9 spa=269.255e-9 spa1=211.397e-9 spa2=155.295e-9 spa3=170.776e-9 sap=244.923e-9 spba=223.131e-9 sapb=396.954e-9 spba1=228.22e-9
**Series configuration of R12
xr12 ntail1 vop vss  rppolywo_m lr=6.6e-6 wr=400e-9 multi=1 m=1 series=9 segspace=250e-9
**End of R12

**Series configuration of R11
xr11 vom ntail1 vss  rppolywo_m lr=6.6e-6 wr=400e-9 multi=1 m=1 series=9 segspace=250e-9
**End of R11

xm3 vss ntail1 vss vss nch_lvt_mac l=2.2e-6 w=2.5e-6 multi=1 nf=1 sd=140e-9 ad=275e-15 as=275e-15 pd=5.22e-6 ps=5.22e-6 nrd=12.421e-3 nrs=12.421e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=3e-6 spa1=3e-6 spa2=3e-6 spa3=3e-6 sap=1.00025e-6 spba=1.79444e-6 sapb=577.831e-9 spba1=1.98202e-6
xm0 vss ntail1 vss vss nch_lvt_mac l=2.2e-6 w=2.5e-6 multi=1 nf=1 sd=140e-9 ad=275e-15 as=275e-15 pd=5.22e-6 ps=5.22e-6 nrd=12.421e-3 nrs=12.421e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=3e-6 spa1=3e-6 spa2=3e-6 spa3=3e-6 sap=1.00025e-6 spba=1.79444e-6 sapb=577.831e-9 spba1=1.98202e-6
xm26 vop vim net08 vss nch_lvt_mac l=240e-9 w=3.4e-6 multi=1 nf=8 sd=160e-9 ad=272e-15 as=297.5e-15 pd=4.68e-6 ps=5.65e-6 nrd=8.828e-3 nrs=8.828e-3 sa=739.25e-9 sb=739.25e-9 sa1=242.174e-9 sa2=569.979e-9 sa3=810.357e-9 sa4=546.21e-9 sb1=242.174e-9 sb2=569.979e-9 sb3=810.357e-9 spa=218.802e-9 spa1=193.239e-9 spa2=168.373e-9 spa3=179.161e-9 sap=332.625e-9 spba=284.603e-9 sapb=590.623e-9 spba1=293.014e-9
xm27 vom vip net08 vss nch_lvt_mac l=240e-9 w=3.4e-6 multi=1 nf=8 sd=160e-9 ad=272e-15 as=297.5e-15 pd=4.68e-6 ps=5.65e-6 nrd=8.828e-3 nrs=8.828e-3 sa=739.25e-9 sb=739.25e-9 sa1=242.174e-9 sa2=569.979e-9 sa3=810.357e-9 sa4=546.21e-9 sb1=242.174e-9 sb2=569.979e-9 sb3=810.357e-9 spa=218.802e-9 spa1=193.239e-9 spa2=168.373e-9 spa3=179.161e-9 sap=332.625e-9 spba=284.603e-9 sapb=590.623e-9 spba1=293.014e-9
xc1 ntail1 vop cfmom_2t nr=46 lr=1e-6 w=70e-9 s=70e-9 stm=2 spm=6 multi=1 ftip=140e-9
xc0 ntail1 vom cfmom_2t nr=46 lr=1e-6 w=70e-9 s=70e-9 stm=2 spm=6 multi=1 ftip=140e-9
.ends Gm1_v5_Practice_schematic
** End of subcircuit definition.

** Library name: 2019_CTDSM_MAGICAL
** Cell name: DFCNQD2BWP_LVT
** View name: schematic
.subckt DFCNQD2BWP_LVT d cp cdn q vdd vss
x0 net63 cp vss vss nch_lvt_mac l=40e-9 w=155e-9 multi=1 nf=1 sd=140e-9 ad=17.05e-15 as=17.05e-15 pd=530e-9 ps=530e-9 nrd=451.613e-3 nrs=451.613e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi4 net61 net63 vss vss nch_lvt_mac l=40e-9 w=310e-9 multi=1 nf=1 sd=140e-9 ad=34.1e-15 as=34.1e-15 pd=840e-9 ps=840e-9 nrd=225.806e-3 nrs=225.806e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x1 net97 cdn net60 vss nch_lvt_mac l=40e-9 w=210e-9 multi=1 nf=1 sd=140e-9 ad=23.1e-15 as=23.1e-15 pd=640e-9 ps=640e-9 nrd=333.333e-3 nrs=333.333e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x2 net123 net95 vss vss nch_lvt_mac l=40e-9 w=150e-9 multi=1 nf=1 sd=140e-9 ad=16.5e-15 as=16.5e-15 pd=520e-9 ps=520e-9 nrd=466.667e-3 nrs=466.667e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi29 net49 net63 net17 vss nch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 sd=140e-9 ad=13.2e-15 as=13.2e-15 pd=460e-9 ps=460e-9 nrd=583.333e-3 nrs=583.333e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi15 net123 net81 net49 vss nch_lvt_mac l=40e-9 w=150e-9 multi=1 nf=1 sd=140e-9 ad=16.5e-15 as=16.5e-15 pd=520e-9 ps=520e-9 nrd=466.667e-3 nrs=466.667e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x3 net60 net49 vss vss nch_lvt_mac l=40e-9 w=210e-9 multi=1 nf=1 sd=140e-9 ad=23.1e-15 as=23.1e-15 pd=640e-9 ps=640e-9 nrd=333.333e-3 nrs=333.333e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x4 net97 cdn net21 vss nch_lvt_mac l=40e-9 w=210e-9 multi=1 nf=1 sd=140e-9 ad=23.1e-15 as=23.1e-15 pd=640e-9 ps=640e-9 nrd=333.333e-3 nrs=333.333e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x5 net81 net63 vss vss nch_lvt_mac l=40e-9 w=155e-9 multi=1 nf=1 sd=140e-9 ad=17.05e-15 as=17.05e-15 pd=530e-9 ps=530e-9 nrd=451.613e-3 nrs=451.613e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi5 net95 d net61 vss nch_lvt_mac l=40e-9 w=310e-9 multi=1 nf=1 sd=140e-9 ad=34.1e-15 as=34.1e-15 pd=840e-9 ps=840e-9 nrd=225.806e-3 nrs=225.806e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi49 net25 cdn vss vss nch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 sd=140e-9 ad=13.2e-15 as=13.2e-15 pd=460e-9 ps=460e-9 nrd=583.333e-3 nrs=583.333e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x6 net21 net49 vss vss nch_lvt_mac l=40e-9 w=210e-9 multi=1 nf=1 sd=140e-9 ad=23.1e-15 as=23.1e-15 pd=640e-9 ps=640e-9 nrd=333.333e-3 nrs=333.333e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi26 net17 net97 vss vss nch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 sd=140e-9 ad=13.2e-15 as=13.2e-15 pd=460e-9 ps=460e-9 nrd=583.333e-3 nrs=583.333e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi48 net13 net123 net25 vss nch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 sd=140e-9 ad=13.2e-15 as=13.2e-15 pd=460e-9 ps=460e-9 nrd=583.333e-3 nrs=583.333e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x7 q net97 vss vss nch_lvt_mac l=40e-9 w=310e-9 multi=1 nf=1 sd=140e-9 ad=34.1e-15 as=34.1e-15 pd=840e-9 ps=840e-9 nrd=225.806e-3 nrs=225.806e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x8 q net97 vss vss nch_lvt_mac l=40e-9 w=310e-9 multi=1 nf=1 sd=140e-9 ad=34.1e-15 as=34.1e-15 pd=840e-9 ps=840e-9 nrd=225.806e-3 nrs=225.806e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi47 net95 net81 net13 vss nch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 sd=140e-9 ad=13.2e-15 as=13.2e-15 pd=460e-9 ps=460e-9 nrd=583.333e-3 nrs=583.333e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi33 net80 net97 vdd vdd pch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 sd=140e-9 ad=13.2e-15 as=13.2e-15 pd=460e-9 ps=460e-9 nrd=583.333e-3 nrs=583.333e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x9 q net97 vdd vdd pch_lvt_mac l=40e-9 w=410e-9 multi=1 nf=1 sd=140e-9 ad=45.1e-15 as=45.1e-15 pd=1.04e-6 ps=1.04e-6 nrd=170.732e-3 nrs=170.732e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x10 net97 net49 vdd vdd pch_lvt_mac l=40e-9 w=370e-9 multi=1 nf=1 sd=140e-9 ad=40.7e-15 as=40.7e-15 pd=960e-9 ps=960e-9 nrd=189.189e-3 nrs=189.189e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi43 net101 net123 vdd vdd pch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 sd=140e-9 ad=13.2e-15 as=13.2e-15 pd=460e-9 ps=460e-9 nrd=583.333e-3 nrs=583.333e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi6 net95 d net120 vdd pch_lvt_mac l=40e-9 w=340e-9 multi=1 nf=1 sd=140e-9 ad=37.4e-15 as=37.4e-15 pd=900e-9 ps=900e-9 nrd=205.882e-3 nrs=205.882e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x11 q net97 vdd vdd pch_lvt_mac l=40e-9 w=410e-9 multi=1 nf=1 sd=140e-9 ad=45.1e-15 as=45.1e-15 pd=1.04e-6 ps=1.04e-6 nrd=170.732e-3 nrs=170.732e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x12 net97 net49 vdd vdd pch_lvt_mac l=40e-9 w=370e-9 multi=1 nf=1 sd=140e-9 ad=40.7e-15 as=40.7e-15 pd=960e-9 ps=960e-9 nrd=189.189e-3 nrs=189.189e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x13 net97 cdn vdd vdd pch_lvt_mac l=40e-9 w=370e-9 multi=1 nf=1 sd=140e-9 ad=40.7e-15 as=40.7e-15 pd=960e-9 ps=960e-9 nrd=189.189e-3 nrs=189.189e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi44 net101 cdn vdd vdd pch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 sd=140e-9 ad=13.2e-15 as=13.2e-15 pd=460e-9 ps=460e-9 nrd=583.333e-3 nrs=583.333e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x14 net97 cdn vdd vdd pch_lvt_mac l=40e-9 w=370e-9 multi=1 nf=1 sd=140e-9 ad=40.7e-15 as=40.7e-15 pd=960e-9 ps=960e-9 nrd=189.189e-3 nrs=189.189e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x15 net123 net95 vdd vdd pch_lvt_mac l=40e-9 w=180e-9 multi=1 nf=1 sd=140e-9 ad=19.8e-15 as=19.8e-15 pd=580e-9 ps=580e-9 nrd=388.889e-3 nrs=388.889e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x16 net63 cp vdd vdd pch_lvt_mac l=40e-9 w=205e-9 multi=1 nf=1 sd=140e-9 ad=22.55e-15 as=22.55e-15 pd=630e-9 ps=630e-9 nrd=341.463e-3 nrs=341.463e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi16 net123 net63 net49 vdd pch_lvt_mac l=40e-9 w=180e-9 multi=1 nf=1 sd=140e-9 ad=19.8e-15 as=19.8e-15 pd=580e-9 ps=580e-9 nrd=388.889e-3 nrs=388.889e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x17 net81 net63 vdd vdd pch_lvt_mac l=40e-9 w=205e-9 multi=1 nf=1 sd=140e-9 ad=22.55e-15 as=22.55e-15 pd=630e-9 ps=630e-9 nrd=341.463e-3 nrs=341.463e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi32 net49 net81 net80 vdd pch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 sd=140e-9 ad=13.2e-15 as=13.2e-15 pd=460e-9 ps=460e-9 nrd=583.333e-3 nrs=583.333e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi45 net95 net63 net101 vdd pch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 sd=140e-9 ad=13.2e-15 as=13.2e-15 pd=460e-9 ps=460e-9 nrd=583.333e-3 nrs=583.333e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi7 net120 net81 vdd vdd pch_lvt_mac l=40e-9 w=340e-9 multi=1 nf=1 sd=140e-9 ad=37.4e-15 as=37.4e-15 pd=900e-9 ps=900e-9 nrd=205.882e-3 nrs=205.882e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
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
x0 net63 cp vss vss nch_lvt_mac l=40e-9 w=155e-9 multi=1 nf=1 sd=140e-9 ad=17.05e-15 as=17.05e-15 pd=530e-9 ps=530e-9 nrd=451.613e-3 nrs=451.613e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi4 net61 net63 vss vss nch_lvt_mac l=40e-9 w=310e-9 multi=1 nf=1 sd=140e-9 ad=34.1e-15 as=34.1e-15 pd=840e-9 ps=840e-9 nrd=225.806e-3 nrs=225.806e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x1 net97 cdn net60 vss nch_lvt_mac l=40e-9 w=210e-9 multi=1 nf=1 sd=140e-9 ad=23.1e-15 as=23.1e-15 pd=640e-9 ps=640e-9 nrd=333.333e-3 nrs=333.333e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x2 net123 net95 vss vss nch_lvt_mac l=40e-9 w=150e-9 multi=1 nf=1 sd=140e-9 ad=16.5e-15 as=16.5e-15 pd=520e-9 ps=520e-9 nrd=466.667e-3 nrs=466.667e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi29 net49 net63 net17 vss nch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 sd=140e-9 ad=13.2e-15 as=13.2e-15 pd=460e-9 ps=460e-9 nrd=583.333e-3 nrs=583.333e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi15 net123 net81 net49 vss nch_lvt_mac l=40e-9 w=150e-9 multi=1 nf=1 sd=140e-9 ad=16.5e-15 as=16.5e-15 pd=520e-9 ps=520e-9 nrd=466.667e-3 nrs=466.667e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x3 net60 net49 vss vss nch_lvt_mac l=40e-9 w=210e-9 multi=1 nf=1 sd=140e-9 ad=23.1e-15 as=23.1e-15 pd=640e-9 ps=640e-9 nrd=333.333e-3 nrs=333.333e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x4 net97 cdn net21 vss nch_lvt_mac l=40e-9 w=210e-9 multi=1 nf=1 sd=140e-9 ad=23.1e-15 as=23.1e-15 pd=640e-9 ps=640e-9 nrd=333.333e-3 nrs=333.333e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x5 net81 net63 vss vss nch_lvt_mac l=40e-9 w=155e-9 multi=1 nf=1 sd=140e-9 ad=17.05e-15 as=17.05e-15 pd=530e-9 ps=530e-9 nrd=451.613e-3 nrs=451.613e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi5 net95 d net61 vss nch_lvt_mac l=40e-9 w=310e-9 multi=1 nf=1 sd=140e-9 ad=34.1e-15 as=34.1e-15 pd=840e-9 ps=840e-9 nrd=225.806e-3 nrs=225.806e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi49 net25 cdn vss vss nch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 sd=140e-9 ad=13.2e-15 as=13.2e-15 pd=460e-9 ps=460e-9 nrd=583.333e-3 nrs=583.333e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x6 net21 net49 vss vss nch_lvt_mac l=40e-9 w=210e-9 multi=1 nf=1 sd=140e-9 ad=23.1e-15 as=23.1e-15 pd=640e-9 ps=640e-9 nrd=333.333e-3 nrs=333.333e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi26 net17 net97 vss vss nch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 sd=140e-9 ad=13.2e-15 as=13.2e-15 pd=460e-9 ps=460e-9 nrd=583.333e-3 nrs=583.333e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi48 net13 net123 net25 vss nch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 sd=140e-9 ad=13.2e-15 as=13.2e-15 pd=460e-9 ps=460e-9 nrd=583.333e-3 nrs=583.333e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x7 q net97 vss vss nch_lvt_mac l=40e-9 w=310e-9 multi=1 nf=1 sd=140e-9 ad=34.1e-15 as=34.1e-15 pd=840e-9 ps=840e-9 nrd=225.806e-3 nrs=225.806e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x8 q net97 vss vss nch_lvt_mac l=40e-9 w=310e-9 multi=1 nf=1 sd=140e-9 ad=34.1e-15 as=34.1e-15 pd=840e-9 ps=840e-9 nrd=225.806e-3 nrs=225.806e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi47 net95 net81 net13 vss nch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 sd=140e-9 ad=13.2e-15 as=13.2e-15 pd=460e-9 ps=460e-9 nrd=583.333e-3 nrs=583.333e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi33 net80 net97 vdd vdd pch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 sd=140e-9 ad=13.2e-15 as=13.2e-15 pd=460e-9 ps=460e-9 nrd=583.333e-3 nrs=583.333e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x9 q net97 vdd vdd pch_lvt_mac l=40e-9 w=410e-9 multi=1 nf=1 sd=140e-9 ad=45.1e-15 as=45.1e-15 pd=1.04e-6 ps=1.04e-6 nrd=170.732e-3 nrs=170.732e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x10 net97 net49 vdd vdd pch_lvt_mac l=40e-9 w=370e-9 multi=1 nf=1 sd=140e-9 ad=40.7e-15 as=40.7e-15 pd=960e-9 ps=960e-9 nrd=189.189e-3 nrs=189.189e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi43 net101 net123 vdd vdd pch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 sd=140e-9 ad=13.2e-15 as=13.2e-15 pd=460e-9 ps=460e-9 nrd=583.333e-3 nrs=583.333e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi6 net95 d net120 vdd pch_lvt_mac l=40e-9 w=340e-9 multi=1 nf=1 sd=140e-9 ad=37.4e-15 as=37.4e-15 pd=900e-9 ps=900e-9 nrd=205.882e-3 nrs=205.882e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x11 q net97 vdd vdd pch_lvt_mac l=40e-9 w=410e-9 multi=1 nf=1 sd=140e-9 ad=45.1e-15 as=45.1e-15 pd=1.04e-6 ps=1.04e-6 nrd=170.732e-3 nrs=170.732e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x12 net97 net49 vdd vdd pch_lvt_mac l=40e-9 w=370e-9 multi=1 nf=1 sd=140e-9 ad=40.7e-15 as=40.7e-15 pd=960e-9 ps=960e-9 nrd=189.189e-3 nrs=189.189e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x13 net97 cdn vdd vdd pch_lvt_mac l=40e-9 w=370e-9 multi=1 nf=1 sd=140e-9 ad=40.7e-15 as=40.7e-15 pd=960e-9 ps=960e-9 nrd=189.189e-3 nrs=189.189e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi44 net101 cdn vdd vdd pch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 sd=140e-9 ad=13.2e-15 as=13.2e-15 pd=460e-9 ps=460e-9 nrd=583.333e-3 nrs=583.333e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x14 net97 cdn vdd vdd pch_lvt_mac l=40e-9 w=370e-9 multi=1 nf=1 sd=140e-9 ad=40.7e-15 as=40.7e-15 pd=960e-9 ps=960e-9 nrd=189.189e-3 nrs=189.189e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x15 net123 net95 vdd vdd pch_lvt_mac l=40e-9 w=180e-9 multi=1 nf=1 sd=140e-9 ad=19.8e-15 as=19.8e-15 pd=580e-9 ps=580e-9 nrd=388.889e-3 nrs=388.889e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x16 net63 cp vdd vdd pch_lvt_mac l=40e-9 w=205e-9 multi=1 nf=1 sd=140e-9 ad=22.55e-15 as=22.55e-15 pd=630e-9 ps=630e-9 nrd=341.463e-3 nrs=341.463e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi16 net123 net63 net49 vdd pch_lvt_mac l=40e-9 w=180e-9 multi=1 nf=1 sd=140e-9 ad=19.8e-15 as=19.8e-15 pd=580e-9 ps=580e-9 nrd=388.889e-3 nrs=388.889e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
x17 net81 net63 vdd vdd pch_lvt_mac l=40e-9 w=205e-9 multi=1 nf=1 sd=140e-9 ad=22.55e-15 as=22.55e-15 pd=630e-9 ps=630e-9 nrd=341.463e-3 nrs=341.463e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi32 net49 net81 net80 vdd pch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 sd=140e-9 ad=13.2e-15 as=13.2e-15 pd=460e-9 ps=460e-9 nrd=583.333e-3 nrs=583.333e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi45 net95 net63 net101 vdd pch_lvt_mac l=40e-9 w=120e-9 multi=1 nf=1 sd=140e-9 ad=13.2e-15 as=13.2e-15 pd=460e-9 ps=460e-9 nrd=583.333e-3 nrs=583.333e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xmi7 net120 net81 vdd vdd pch_lvt_mac l=40e-9 w=340e-9 multi=1 nf=1 sd=140e-9 ad=37.4e-15 as=37.4e-15 pd=900e-9 ps=900e-9 nrd=205.882e-3 nrs=205.882e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
.ends DFCNQD2BWP_LVT_schematic
** End of subcircuit definition.

** Library name: 2019_CTDSM_MAGICAL_TAPEOUT
** Cell name: myComparator_v3_CTDSM_DEC2016
** View name: schematic
.subckt myComparator_v3_CTDSM_DEC2016_schematic clk gnd outm outp vdd _net0 _net1
xm0 gnd intern gnd gnd nch_lvt_mac l=1e-6 w=1.05e-6 multi=1 nf=1 sd=140e-9 ad=115.5e-15 as=115.5e-15 pd=2.32e-6 ps=2.32e-6 nrd=34.495e-3 nrs=34.495e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=3e-6 spa1=3e-6 spa2=3e-6 spa3=3e-6 sap=1.00025e-6 spba=1.76068e-6 sapb=577.831e-9 spba1=1.84662e-6
xm22 gnd interp gnd gnd nch_lvt_mac l=1e-6 w=1.05e-6 multi=1 nf=1 sd=140e-9 ad=115.5e-15 as=115.5e-15 pd=2.32e-6 ps=2.32e-6 nrd=34.495e-3 nrs=34.495e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=3e-6 spa1=3e-6 spa2=3e-6 spa3=3e-6 sap=1.00025e-6 spba=1.76068e-6 sapb=577.831e-9 spba1=1.84662e-6
xm16 outm crossp gnd gnd nch_lvt_mac l=40e-9 w=1.44e-6 multi=1 nf=4 sd=140e-9 ad=100.8e-15 as=129.6e-15 pd=2e-6 ps=2.88e-6 nrd=38.421e-3 nrs=38.421e-3 sa=256.788e-9 sb=256.788e-9 sa1=172.516e-9 sa2=246.369e-9 sa3=404.983e-9 sa4=247.435e-9 sb1=172.516e-9 sb2=246.369e-9 sb3=404.983e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=170.416e-9 spba=168.903e-9 sapb=240.571e-9 spba1=170.77e-9
xm17 outp crossn gnd gnd nch_lvt_mac l=40e-9 w=1.44e-6 multi=1 nf=4 sd=140e-9 ad=100.8e-15 as=129.6e-15 pd=2e-6 ps=2.88e-6 nrd=38.421e-3 nrs=38.421e-3 sa=256.788e-9 sb=256.788e-9 sa1=172.516e-9 sa2=246.369e-9 sa3=404.983e-9 sa4=247.435e-9 sb1=172.516e-9 sb2=246.369e-9 sb3=404.983e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=170.416e-9 spba=168.903e-9 sapb=240.571e-9 spba1=170.77e-9
xm4 crossn crossp intern gnd nch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 sd=140e-9 ad=134.4e-15 as=172.8e-15 pd=2.48e-6 ps=3.6e-6 nrd=22.768e-3 nrs=22.768e-3 sa=256.788e-9 sb=256.788e-9 sa1=172.516e-9 sa2=246.369e-9 sa3=404.983e-9 sa4=247.435e-9 sb1=172.516e-9 sb2=246.369e-9 sb3=404.983e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=170.416e-9 spba=168.903e-9 sapb=240.571e-9 spba1=170.77e-9
xm3 crossp crossn interp gnd nch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 sd=140e-9 ad=134.4e-15 as=172.8e-15 pd=2.48e-6 ps=3.6e-6 nrd=22.768e-3 nrs=22.768e-3 sa=256.788e-9 sb=256.788e-9 sa1=172.516e-9 sa2=246.369e-9 sa3=404.983e-9 sa4=247.435e-9 sb1=172.516e-9 sb2=246.369e-9 sb3=404.983e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=170.416e-9 spba=168.903e-9 sapb=240.571e-9 spba1=170.77e-9
xm7 net069 clk gnd gnd nch_lvt_mac l=40e-9 w=6.9e-6 multi=1 nf=15 sd=140e-9 ad=501.4e-15 as=501.4e-15 pd=9.54e-6 ps=9.54e-6 nrd=5.906e-3 nrs=5.906e-3 sa=675.603e-9 sb=675.603e-9 sa1=269.666e-9 sa2=545.885e-9 sa3=818.344e-9 sa4=642.475e-9 sb1=269.666e-9 sb2=545.885e-9 sb3=818.344e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=234.015e-9 spba=162.859e-9 sapb=280.924e-9 spba1=164.658e-9
xm5 intern _net0 net069 gnd nch_lvt_mac l=40e-9 w=14.4e-6 multi=1 nf=15 sd=140e-9 ad=1.0464e-12 as=1.0464e-12 pd=17.54e-6 ps=17.54e-6 nrd=2.264e-3 nrs=2.264e-3 sa=675.603e-9 sb=675.603e-9 sa1=269.666e-9 sa2=545.885e-9 sa3=818.344e-9 sa4=642.475e-9 sb1=269.666e-9 sb2=545.885e-9 sb3=818.344e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=234.015e-9 spba=162.859e-9 sapb=280.924e-9 spba1=164.658e-9
xm6 interp _net1 net069 gnd nch_lvt_mac l=40e-9 w=14.4e-6 multi=1 nf=15 sd=140e-9 ad=1.0464e-12 as=1.0464e-12 pd=17.54e-6 ps=17.54e-6 nrd=2.264e-3 nrs=2.264e-3 sa=675.603e-9 sb=675.603e-9 sa1=269.666e-9 sa2=545.885e-9 sa3=818.344e-9 sa4=642.475e-9 sb1=269.666e-9 sb2=545.885e-9 sb3=818.344e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=234.015e-9 spba=162.859e-9 sapb=280.924e-9 spba1=164.658e-9
xm8 outm crossp vdd vdd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 sd=140e-9 ad=134.4e-15 as=172.8e-15 pd=2.48e-6 ps=3.6e-6 nrd=28.999e-3 nrs=28.999e-3 sa=256.788e-9 sb=256.788e-9 sa1=172.516e-9 sa2=246.369e-9 sa3=404.983e-9 sa4=247.435e-9 sb1=172.516e-9 sb2=246.369e-9 sb3=404.983e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=170.416e-9 spba=168.903e-9 sapb=240.571e-9 spba1=170.77e-9
xm18 intern clk vdd vdd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 sd=140e-9 ad=134.4e-15 as=172.8e-15 pd=2.48e-6 ps=3.6e-6 nrd=28.999e-3 nrs=28.999e-3 sa=256.788e-9 sb=256.788e-9 sa1=172.516e-9 sa2=246.369e-9 sa3=404.983e-9 sa4=247.435e-9 sb1=172.516e-9 sb2=246.369e-9 sb3=404.983e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=170.416e-9 spba=168.903e-9 sapb=240.571e-9 spba1=170.77e-9
xm15 outp crossn vdd vdd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 sd=140e-9 ad=134.4e-15 as=172.8e-15 pd=2.48e-6 ps=3.6e-6 nrd=28.999e-3 nrs=28.999e-3 sa=256.788e-9 sb=256.788e-9 sa1=172.516e-9 sa2=246.369e-9 sa3=404.983e-9 sa4=247.435e-9 sb1=172.516e-9 sb2=246.369e-9 sb3=404.983e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=170.416e-9 spba=168.903e-9 sapb=240.571e-9 spba1=170.77e-9
xm2 interp clk vdd vdd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 sd=140e-9 ad=134.4e-15 as=172.8e-15 pd=2.48e-6 ps=3.6e-6 nrd=28.999e-3 nrs=28.999e-3 sa=256.788e-9 sb=256.788e-9 sa1=172.516e-9 sa2=246.369e-9 sa3=404.983e-9 sa4=247.435e-9 sb1=172.516e-9 sb2=246.369e-9 sb3=404.983e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=170.416e-9 spba=168.903e-9 sapb=240.571e-9 spba1=170.77e-9
xm1 crossn clk vdd vdd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 sd=140e-9 ad=134.4e-15 as=172.8e-15 pd=2.48e-6 ps=3.6e-6 nrd=28.999e-3 nrs=28.999e-3 sa=256.788e-9 sb=256.788e-9 sa1=172.516e-9 sa2=246.369e-9 sa3=404.983e-9 sa4=247.435e-9 sb1=172.516e-9 sb2=246.369e-9 sb3=404.983e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=170.416e-9 spba=168.903e-9 sapb=240.571e-9 spba1=170.77e-9
xm12 crossp clk vdd vdd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 sd=140e-9 ad=134.4e-15 as=172.8e-15 pd=2.48e-6 ps=3.6e-6 nrd=28.999e-3 nrs=28.999e-3 sa=256.788e-9 sb=256.788e-9 sa1=172.516e-9 sa2=246.369e-9 sa3=404.983e-9 sa4=247.435e-9 sb1=172.516e-9 sb2=246.369e-9 sb3=404.983e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=170.416e-9 spba=168.903e-9 sapb=240.571e-9 spba1=170.77e-9
xm14 crossn crossp vdd vdd pch_lvt_mac l=40e-9 w=3.84e-6 multi=1 nf=8 sd=140e-9 ad=268.8e-15 as=307.2e-15 pd=4.96e-6 ps=6.08e-6 nrd=14.333e-3 nrs=14.333e-3 sa=420.901e-9 sb=420.901e-9 sa1=218.025e-9 sa2=376.982e-9 sa3=612.324e-9 sa4=401.816e-9 sb1=218.025e-9 sb2=376.982e-9 sb3=612.324e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=200.193e-9 spba=164.62e-9 sapb=257.228e-9 spba1=166.439e-9
xm13 crossp crossn vdd vdd pch_lvt_mac l=40e-9 w=3.84e-6 multi=1 nf=8 sd=140e-9 ad=268.8e-15 as=307.2e-15 pd=4.96e-6 ps=6.08e-6 nrd=14.333e-3 nrs=14.333e-3 sa=420.901e-9 sb=420.901e-9 sa1=218.025e-9 sa2=376.982e-9 sa3=612.324e-9 sa4=401.816e-9 sb1=218.025e-9 sb2=376.982e-9 sb3=612.324e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=200.193e-9 spba=164.62e-9 sapb=257.228e-9 spba1=166.439e-9
.ends myComparator_v3_CTDSM_DEC2016_schematic
** End of subcircuit definition.

** Library name: 2019_CTDSM_MAGICAL
** Cell name: NR2D8BWP_LVT_RESIZE
** View name: schematic
.subckt NR2D8BWP_LVT a1 a2 zn vdd vss
x0 zn a2 vss vss nch_lvt_mac l=80e-9 w=4.96e-6 multi=1 nf=2 sd=140e-9 ad=347.2e-15 as=545.6e-15 pd=5.24e-6 ps=10.36e-6 nrd=7.181e-3 nrs=7.181e-3 sa=173.462e-9 sb=173.462e-9 sa1=138.62e-9 sa2=170.683e-9 sa3=255.739e-9 sa4=110e-9 sb1=138.62e-9 sb2=170.683e-9 sb3=255.739e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=201.982e-9 sapb=1.59028e-6 spba1=745.933e-9
x1 zn a1 vss vss nch_lvt_mac l=80e-9 w=4.96e-6 multi=1 nf=2 sd=140e-9 ad=347.2e-15 as=545.6e-15 pd=5.24e-6 ps=10.36e-6 nrd=7.181e-3 nrs=7.181e-3 sa=173.462e-9 sb=173.462e-9 sa1=138.62e-9 sa2=170.683e-9 sa3=255.739e-9 sa4=110e-9 sb1=138.62e-9 sb2=170.683e-9 sb3=255.739e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=201.982e-9 sapb=1.59028e-6 spba1=745.933e-9
x2 net13 a2 vdd vdd pch_lvt_mac l=80e-9 w=6.56e-6 multi=1 nf=2 sd=140e-9 ad=459.2e-15 as=721.6e-15 pd=6.84e-6 ps=13.56e-6 nrd=6.903e-3 nrs=6.903e-3 sa=173.462e-9 sb=173.462e-9 sa1=138.62e-9 sa2=170.683e-9 sa3=255.739e-9 sa4=110e-9 sb1=138.62e-9 sb2=170.683e-9 sb3=255.739e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=201.982e-9 sapb=1.59028e-6 spba1=745.933e-9
x3 zn a1 net13 vdd pch_lvt_mac l=80e-9 w=6.56e-6 multi=1 nf=2 sd=140e-9 ad=459.2e-15 as=721.6e-15 pd=6.84e-6 ps=13.56e-6 nrd=6.903e-3 nrs=6.903e-3 sa=173.462e-9 sb=173.462e-9 sa1=138.62e-9 sa2=170.683e-9 sa3=255.739e-9 sa4=110e-9 sb1=138.62e-9 sb2=170.683e-9 sb3=255.739e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=201.982e-9 sapb=1.59028e-6 spba1=745.933e-9
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
m0 zn i vss vss nch l=40e-9 w=310e-9 m=1 nf=1 sd=140e-9 ad=34.1e-15 as=34.1e-15 pd=840e-9 ps=840e-9 nrd=225.806e-3 nrs=225.806e-3 sa=110e-9 sb=110e-9
m1 zn i vss vss nch l=40e-9 w=310e-9 m=1 nf=1 sd=140e-9 ad=34.1e-15 as=34.1e-15 pd=840e-9 ps=840e-9 nrd=225.806e-3 nrs=225.806e-3 sa=110e-9 sb=110e-9
m2 zn i vdd vdd pch l=40e-9 w=410e-9 m=1 nf=1 sd=140e-9 ad=45.1e-15 as=45.1e-15 pd=1.04e-6 ps=1.04e-6 nrd=170.732e-3 nrs=170.732e-3 sa=110e-9 sb=110e-9
m3 zn i vdd vdd pch l=40e-9 w=410e-9 m=1 nf=1 sd=140e-9 ad=45.1e-15 as=45.1e-15 pd=1.04e-6 ps=1.04e-6 nrd=170.732e-3 nrs=170.732e-3 sa=110e-9 sb=110e-9
.ends INVD4BWP_LVT
** End of subcircuit definition.

** Library name: 2019_CTDSM_MAGICAL_TAPEOUT
** Cell name: C2_BANK
** View name: schematic
.subckt C2_BANK a b vss d_4_ d_3_ d_2_ d_1_ vdd
xm17 a d_3_ net027 vss nch_lvt_mac l=40e-9 w=6e-6 multi=1 nf=12 sd=140e-9 ad=420e-15 as=460e-15 pd=7.68e-6 ps=8.84e-6 nrd=8.014e-3 nrs=8.014e-3 sa=569.822e-9 sb=569.822e-9 sa1=250.073e-9 sa2=479.667e-9 sa3=742.764e-9 sa4=542.403e-9 sb1=250.073e-9 sb2=479.667e-9 sb3=742.764e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=221.196e-9 spba=163.348e-9 sapb=271.711e-9 spba1=165.153e-9
xm15 net062 db_3_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 sd=140e-9 ad=44e-15 as=44e-15 pd=1.02e-6 ps=1.02e-6 nrd=71.545e-3 nrs=71.545e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xm13 b d_3_ net062 vss nch_lvt_mac l=40e-9 w=6e-6 multi=1 nf=12 sd=140e-9 ad=420e-15 as=460e-15 pd=7.68e-6 ps=8.84e-6 nrd=8.014e-3 nrs=8.014e-3 sa=569.822e-9 sb=569.822e-9 sa1=250.073e-9 sa2=479.667e-9 sa3=742.764e-9 sa4=542.403e-9 sb1=250.073e-9 sb2=479.667e-9 sb3=742.764e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=221.196e-9 spba=163.348e-9 sapb=271.711e-9 spba1=165.153e-9
xm12 net027 db_3_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 sd=140e-9 ad=44e-15 as=44e-15 pd=1.02e-6 ps=1.02e-6 nrd=71.545e-3 nrs=71.545e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xm10 b d_2_ net063 vss nch_lvt_mac l=40e-9 w=3e-6 multi=1 nf=6 sd=140e-9 ad=210e-15 as=250e-15 pd=3.84e-6 ps=5e-6 nrd=15.918e-3 nrs=15.918e-3 sa=341.472e-9 sb=341.472e-9 sa1=197.813e-9 sa2=316.439e-9 sa3=523.981e-9 sa4=327.012e-9 sb1=197.813e-9 sb2=316.439e-9 sb3=523.981e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=186.948e-9 spba=165.964e-9 sapb=248.903e-9 spba1=167.797e-9
xm8 net063 db_2_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 sd=140e-9 ad=44e-15 as=44e-15 pd=1.02e-6 ps=1.02e-6 nrd=71.545e-3 nrs=71.545e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xm20 net061 db_4_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 sd=140e-9 ad=44e-15 as=44e-15 pd=1.02e-6 ps=1.02e-6 nrd=71.545e-3 nrs=71.545e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xm18 a d_4_ net026 vss nch_lvt_mac l=40e-9 w=12e-6 multi=1 nf=24 sd=140e-9 ad=840e-15 as=880e-15 pd=15.36e-6 ps=16.52e-6 nrd=4.021e-3 nrs=4.021e-3 sa=974.034e-9 sb=974.034e-9 sa1=316.049e-9 sa2=711.573e-9 sa3=989.306e-9 sa4=925.486e-9 sb1=316.049e-9 sb2=711.573e-9 sb3=989.306e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=264.244e-9 spba=162.143e-9 sapb=303.058e-9 spba1=163.935e-9
xm23 net026 db_4_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 sd=140e-9 ad=44e-15 as=44e-15 pd=1.02e-6 ps=1.02e-6 nrd=71.545e-3 nrs=71.545e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xm11 net028 db_2_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 sd=140e-9 ad=44e-15 as=44e-15 pd=1.02e-6 ps=1.02e-6 nrd=71.545e-3 nrs=71.545e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xm6 a d_2_ net028 vss nch_lvt_mac l=40e-9 w=3e-6 multi=1 nf=6 sd=140e-9 ad=210e-15 as=250e-15 pd=3.84e-6 ps=5e-6 nrd=15.918e-3 nrs=15.918e-3 sa=341.472e-9 sb=341.472e-9 sa1=197.813e-9 sa2=316.439e-9 sa3=523.981e-9 sa4=327.012e-9 sb1=197.813e-9 sb2=316.439e-9 sb3=523.981e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=186.948e-9 spba=165.964e-9 sapb=248.903e-9 spba1=167.797e-9
xm2 b d_1_ net041 vss nch_lvt_mac l=40e-9 w=1.5e-6 multi=1 nf=3 sd=140e-9 ad=125e-15 as=125e-15 pd=2.5e-6 ps=2.5e-6 nrd=31.41e-3 nrs=31.41e-3 sa=211.501e-9 sb=211.501e-9 sa1=156.639e-9 sa2=206.282e-9 sa3=326.246e-9 sa4=204.965e-9 sb1=156.639e-9 sb2=206.282e-9 sb3=326.246e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=160.097e-9 spba=172.251e-9 sapb=237.956e-9 spba1=174.155e-9
xm4 net029 db_1_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 sd=140e-9 ad=44e-15 as=44e-15 pd=1.02e-6 ps=1.02e-6 nrd=71.545e-3 nrs=71.545e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xm5 net041 db_1_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 sd=140e-9 ad=44e-15 as=44e-15 pd=1.02e-6 ps=1.02e-6 nrd=71.545e-3 nrs=71.545e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xm0 a d_1_ net029 vss nch_lvt_mac l=40e-9 w=1.5e-6 multi=1 nf=3 sd=140e-9 ad=125e-15 as=125e-15 pd=2.5e-6 ps=2.5e-6 nrd=31.41e-3 nrs=31.41e-3 sa=211.501e-9 sb=211.501e-9 sa1=156.639e-9 sa2=206.282e-9 sa3=326.246e-9 sa4=204.965e-9 sb1=156.639e-9 sb2=206.282e-9 sb3=326.246e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=160.097e-9 spba=172.251e-9 sapb=237.956e-9 spba1=174.155e-9
xm22 b d_4_ net061 vss nch_lvt_mac l=40e-9 w=12e-6 multi=1 nf=24 sd=140e-9 ad=840e-15 as=880e-15 pd=15.36e-6 ps=16.52e-6 nrd=4.021e-3 nrs=4.021e-3 sa=974.034e-9 sb=974.034e-9 sa1=316.049e-9 sa2=711.573e-9 sa3=989.306e-9 sa4=925.486e-9 sb1=316.049e-9 sb2=711.573e-9 sb3=989.306e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=264.244e-9 spba=162.143e-9 sapb=303.058e-9 spba1=163.935e-9
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
xm16 net027 db_3_ a vdd pch_lvt_mac l=40e-9 w=12e-6 multi=1 nf=12 sd=140e-9 ad=840e-15 as=920e-15 pd=13.68e-6 ps=15.84e-6 nrd=3.642e-3 nrs=3.642e-3 sa=569.822e-9 sb=569.822e-9 sa1=250.073e-9 sa2=479.667e-9 sa3=742.764e-9 sa4=542.403e-9 sb1=250.073e-9 sb2=479.667e-9 sb3=742.764e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=221.196e-9 spba=163.348e-9 sapb=271.711e-9 spba1=165.153e-9
xm14 net062 db_3_ b vdd pch_lvt_mac l=40e-9 w=12e-6 multi=1 nf=12 sd=140e-9 ad=840e-15 as=920e-15 pd=13.68e-6 ps=15.84e-6 nrd=3.642e-3 nrs=3.642e-3 sa=569.822e-9 sb=569.822e-9 sa1=250.073e-9 sa2=479.667e-9 sa3=742.764e-9 sa4=542.403e-9 sb1=250.073e-9 sb2=479.667e-9 sb3=742.764e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=221.196e-9 spba=163.348e-9 sapb=271.711e-9 spba1=165.153e-9
xm9 net063 db_2_ b vdd pch_lvt_mac l=40e-9 w=6e-6 multi=1 nf=6 sd=140e-9 ad=420e-15 as=500e-15 pd=6.84e-6 ps=9e-6 nrd=7.341e-3 nrs=7.341e-3 sa=341.472e-9 sb=341.472e-9 sa1=197.813e-9 sa2=316.439e-9 sa3=523.981e-9 sa4=327.012e-9 sb1=197.813e-9 sb2=316.439e-9 sb3=523.981e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=186.948e-9 spba=165.964e-9 sapb=248.903e-9 spba1=167.797e-9
xm7 net028 db_2_ a vdd pch_lvt_mac l=40e-9 w=6e-6 multi=1 nf=6 sd=140e-9 ad=420e-15 as=500e-15 pd=6.84e-6 ps=9e-6 nrd=7.341e-3 nrs=7.341e-3 sa=341.472e-9 sb=341.472e-9 sa1=197.813e-9 sa2=316.439e-9 sa3=523.981e-9 sa4=327.012e-9 sb1=197.813e-9 sb2=316.439e-9 sb3=523.981e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=186.948e-9 spba=165.964e-9 sapb=248.903e-9 spba1=167.797e-9
xm19 net026 db_4_ a vdd pch_lvt_mac l=40e-9 w=24e-6 multi=1 nf=24 sd=140e-9 ad=1.68e-12 as=1.76e-12 pd=27.36e-6 ps=29.52e-6 nrd=1.814e-3 nrs=1.814e-3 sa=974.034e-9 sb=974.034e-9 sa1=316.049e-9 sa2=711.573e-9 sa3=989.306e-9 sa4=925.486e-9 sb1=316.049e-9 sb2=711.573e-9 sb3=989.306e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=264.244e-9 spba=162.143e-9 sapb=303.058e-9 spba1=163.935e-9
xm1 net029 db_1_ a vdd pch_lvt_mac l=40e-9 w=3e-6 multi=1 nf=3 sd=140e-9 ad=250e-15 as=250e-15 pd=4.5e-6 ps=4.5e-6 nrd=14.911e-3 nrs=14.911e-3 sa=211.501e-9 sb=211.501e-9 sa1=156.639e-9 sa2=206.282e-9 sa3=326.246e-9 sa4=204.965e-9 sb1=156.639e-9 sb2=206.282e-9 sb3=326.246e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=160.097e-9 spba=172.251e-9 sapb=237.956e-9 spba1=174.155e-9
xm21 net061 db_4_ b vdd pch_lvt_mac l=40e-9 w=24e-6 multi=1 nf=24 sd=140e-9 ad=1.68e-12 as=1.76e-12 pd=27.36e-6 ps=29.52e-6 nrd=1.814e-3 nrs=1.814e-3 sa=974.034e-9 sb=974.034e-9 sa1=316.049e-9 sa2=711.573e-9 sa3=989.306e-9 sa4=925.486e-9 sb1=316.049e-9 sb2=711.573e-9 sb3=989.306e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=264.244e-9 spba=162.143e-9 sapb=303.058e-9 spba1=163.935e-9
xm3 net041 db_1_ b vdd pch_lvt_mac l=40e-9 w=3e-6 multi=1 nf=3 sd=140e-9 ad=250e-15 as=250e-15 pd=4.5e-6 ps=4.5e-6 nrd=14.911e-3 nrs=14.911e-3 sa=211.501e-9 sb=211.501e-9 sa1=156.639e-9 sa2=206.282e-9 sa3=326.246e-9 sa4=204.965e-9 sb1=156.639e-9 sb2=206.282e-9 sb3=326.246e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=160.097e-9 spba=172.251e-9 sapb=237.956e-9 spba1=174.155e-9
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
xm17 a d_3_ net027 vss nch_lvt_mac l=40e-9 w=40e-6 multi=1 nf=80 sd=140e-9 ad=2.8e-12 as=2.84e-12 pd=51.2e-6 ps=52.36e-6 nrd=1.209e-3 nrs=1.209e-3 sa=2.57769e-6 sb=2.57769e-6 sa1=474.06e-9 sa2=1.35618e-6 sa3=1.53462e-6 sa4=2.45509e-6 sb1=474.06e-9 sb2=1.35618e-6 sb3=1.53462e-6 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=365.703e-9 spba=161.335e-9 sapb=376.82e-9 spba1=163.117e-9
xm15 net062 db_3_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 sd=140e-9 ad=44e-15 as=44e-15 pd=1.02e-6 ps=1.02e-6 nrd=71.545e-3 nrs=71.545e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xm13 b d_3_ net062 vss nch_lvt_mac l=40e-9 w=40e-6 multi=1 nf=80 sd=140e-9 ad=2.8e-12 as=2.84e-12 pd=51.2e-6 ps=52.36e-6 nrd=1.209e-3 nrs=1.209e-3 sa=2.57769e-6 sb=2.57769e-6 sa1=474.06e-9 sa2=1.35618e-6 sa3=1.53462e-6 sa4=2.45509e-6 sb1=474.06e-9 sb2=1.35618e-6 sb3=1.53462e-6 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=365.703e-9 spba=161.335e-9 sapb=376.82e-9 spba1=163.117e-9
xm12 net027 db_3_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 sd=140e-9 ad=44e-15 as=44e-15 pd=1.02e-6 ps=1.02e-6 nrd=71.545e-3 nrs=71.545e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xm10 b d_2_ net063 vss nch_lvt_mac l=40e-9 w=20e-6 multi=1 nf=40 sd=140e-9 ad=1.4e-12 as=1.44e-12 pd=25.6e-6 ps=26.76e-6 nrd=2.416e-3 nrs=2.416e-3 sa=1.46361e-6 sb=1.46361e-6 sa1=375.443e-9 sa2=940.103e-9 sa3=1.19857e-6 sa4=1.39126e-6 sb1=375.443e-9 sb2=940.103e-9 sb3=1.19857e-6 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=302.663e-9 spba=161.678e-9 sapb=331.282e-9 spba1=163.464e-9
xm8 net063 db_2_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 sd=140e-9 ad=44e-15 as=44e-15 pd=1.02e-6 ps=1.02e-6 nrd=71.545e-3 nrs=71.545e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xm20 net061 db_4_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 sd=140e-9 ad=44e-15 as=44e-15 pd=1.02e-6 ps=1.02e-6 nrd=71.545e-3 nrs=71.545e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xm18 a d_4_ net026 vss nch_lvt_mac l=40e-9 w=80e-6 multi=1 nf=160 sd=140e-9 ad=5.6e-12 as=5.64e-12 pd=102.4e-6 ps=103.56e-6 nrd=605e-6 nrs=605e-6 sa=4.59897e-6 sb=4.59897e-6 sa1=598.315e-9 sa2=1.94011e-6 sa3=1.94932e-6 sa4=4.3924e-6 sb1=598.315e-9 sb2=1.94011e-6 sb3=1.94932e-6 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=443.927e-9 spba=161.165e-9 sapb=431.498e-9 spba1=162.946e-9
xm23 net026 db_4_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 sd=140e-9 ad=44e-15 as=44e-15 pd=1.02e-6 ps=1.02e-6 nrd=71.545e-3 nrs=71.545e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xm11 net028 db_2_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 sd=140e-9 ad=44e-15 as=44e-15 pd=1.02e-6 ps=1.02e-6 nrd=71.545e-3 nrs=71.545e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xm6 a d_2_ net028 vss nch_lvt_mac l=40e-9 w=20e-6 multi=1 nf=40 sd=140e-9 ad=1.4e-12 as=1.44e-12 pd=25.6e-6 ps=26.76e-6 nrd=2.416e-3 nrs=2.416e-3 sa=1.46361e-6 sb=1.46361e-6 sa1=375.443e-9 sa2=940.103e-9 sa3=1.19857e-6 sa4=1.39126e-6 sb1=375.443e-9 sb2=940.103e-9 sb3=1.19857e-6 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=302.663e-9 spba=161.678e-9 sapb=331.282e-9 spba1=163.464e-9
xm2 b d_1_ net041 vss nch_lvt_mac l=40e-9 w=10e-6 multi=1 nf=20 sd=140e-9 ad=700e-15 as=740e-15 pd=12.8e-6 ps=13.96e-6 nrd=4.821e-3 nrs=4.821e-3 sa=844.248e-9 sb=844.248e-9 sa1=297.186e-9 sa2=642.759e-9 sa3=920.859e-9 sa4=802.299e-9 sb1=297.186e-9 sb2=642.759e-9 sb3=920.859e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=251.973e-9 spba=162.379e-9 sapb=294.037e-9 spba1=164.173e-9
xm4 net029 db_1_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 sd=140e-9 ad=44e-15 as=44e-15 pd=1.02e-6 ps=1.02e-6 nrd=71.545e-3 nrs=71.545e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xm5 net041 db_1_ vss vss nch_lvt_mac l=40e-9 w=400e-9 multi=1 nf=1 sd=140e-9 ad=44e-15 as=44e-15 pd=1.02e-6 ps=1.02e-6 nrd=71.545e-3 nrs=71.545e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=130.305e-9 spba=745.547e-9 sapb=1.59028e-6 spba1=745.933e-9
xm0 a d_1_ net029 vss nch_lvt_mac l=40e-9 w=10e-6 multi=1 nf=20 sd=140e-9 ad=700e-15 as=740e-15 pd=12.8e-6 ps=13.96e-6 nrd=4.821e-3 nrs=4.821e-3 sa=844.248e-9 sb=844.248e-9 sa1=297.186e-9 sa2=642.759e-9 sa3=920.859e-9 sa4=802.299e-9 sb1=297.186e-9 sb2=642.759e-9 sb3=920.859e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=251.973e-9 spba=162.379e-9 sapb=294.037e-9 spba1=164.173e-9
xm22 b d_4_ net061 vss nch_lvt_mac l=40e-9 w=80e-6 multi=1 nf=160 sd=140e-9 ad=5.6e-12 as=5.64e-12 pd=102.4e-6 ps=103.56e-6 nrd=605e-6 nrs=605e-6 sa=4.59897e-6 sb=4.59897e-6 sa1=598.315e-9 sa2=1.94011e-6 sa3=1.94932e-6 sa4=4.3924e-6 sb1=598.315e-9 sb2=1.94011e-6 sb3=1.94932e-6 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=443.927e-9 spba=161.165e-9 sapb=431.498e-9 spba1=162.946e-9
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
xm16 net027 db_3_ a vdd pch_lvt_mac l=40e-9 w=80e-6 multi=1 nf=80 sd=140e-9 ad=5.6e-12 as=5.68e-12 pd=91.2e-6 ps=93.36e-6 nrd=543e-6 nrs=543e-6 sa=2.57769e-6 sb=2.57769e-6 sa1=474.06e-9 sa2=1.35618e-6 sa3=1.53462e-6 sa4=2.45509e-6 sb1=474.06e-9 sb2=1.35618e-6 sb3=1.53462e-6 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=365.703e-9 spba=161.335e-9 sapb=376.82e-9 spba1=163.117e-9
xm14 net062 db_3_ b vdd pch_lvt_mac l=40e-9 w=80e-6 multi=1 nf=80 sd=140e-9 ad=5.6e-12 as=5.68e-12 pd=91.2e-6 ps=93.36e-6 nrd=543e-6 nrs=543e-6 sa=2.57769e-6 sb=2.57769e-6 sa1=474.06e-9 sa2=1.35618e-6 sa3=1.53462e-6 sa4=2.45509e-6 sb1=474.06e-9 sb2=1.35618e-6 sb3=1.53462e-6 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=365.703e-9 spba=161.335e-9 sapb=376.82e-9 spba1=163.117e-9
xm9 net063 db_2_ b vdd pch_lvt_mac l=40e-9 w=40e-6 multi=1 nf=40 sd=140e-9 ad=2.8e-12 as=2.88e-12 pd=45.6e-6 ps=47.76e-6 nrd=1.087e-3 nrs=1.087e-3 sa=1.46361e-6 sb=1.46361e-6 sa1=375.443e-9 sa2=940.103e-9 sa3=1.19857e-6 sa4=1.39126e-6 sb1=375.443e-9 sb2=940.103e-9 sb3=1.19857e-6 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=302.663e-9 spba=161.678e-9 sapb=331.282e-9 spba1=163.464e-9
xm7 net028 db_2_ a vdd pch_lvt_mac l=40e-9 w=40e-6 multi=1 nf=40 sd=140e-9 ad=2.8e-12 as=2.88e-12 pd=45.6e-6 ps=47.76e-6 nrd=1.087e-3 nrs=1.087e-3 sa=1.46361e-6 sb=1.46361e-6 sa1=375.443e-9 sa2=940.103e-9 sa3=1.19857e-6 sa4=1.39126e-6 sb1=375.443e-9 sb2=940.103e-9 sb3=1.19857e-6 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=302.663e-9 spba=161.678e-9 sapb=331.282e-9 spba1=163.464e-9
xm19 net026 db_4_ a vdd pch_lvt_mac l=40e-9 w=160e-6 multi=1 nf=160 sd=140e-9 ad=11.2e-12 as=11.28e-12 pd=182.4e-6 ps=184.56e-6 nrd=271e-6 nrs=271e-6 sa=4.59897e-6 sb=4.59897e-6 sa1=598.315e-9 sa2=1.94011e-6 sa3=1.94932e-6 sa4=4.3924e-6 sb1=598.315e-9 sb2=1.94011e-6 sb3=1.94932e-6 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=443.927e-9 spba=161.165e-9 sapb=431.498e-9 spba1=162.946e-9
xm1 net029 db_1_ a vdd pch_lvt_mac l=40e-9 w=20e-6 multi=1 nf=20 sd=140e-9 ad=1.4e-12 as=1.48e-12 pd=22.8e-6 ps=24.96e-6 nrd=2.179e-3 nrs=2.179e-3 sa=844.248e-9 sb=844.248e-9 sa1=297.186e-9 sa2=642.759e-9 sa3=920.859e-9 sa4=802.299e-9 sb1=297.186e-9 sb2=642.759e-9 sb3=920.859e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=251.973e-9 spba=162.379e-9 sapb=294.037e-9 spba1=164.173e-9
xm21 net061 db_4_ b vdd pch_lvt_mac l=40e-9 w=160e-6 multi=1 nf=160 sd=140e-9 ad=11.2e-12 as=11.28e-12 pd=182.4e-6 ps=184.56e-6 nrd=271e-6 nrs=271e-6 sa=4.59897e-6 sb=4.59897e-6 sa1=598.315e-9 sa2=1.94011e-6 sa3=1.94932e-6 sa4=4.3924e-6 sb1=598.315e-9 sb2=1.94011e-6 sb3=1.94932e-6 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=443.927e-9 spba=161.165e-9 sapb=431.498e-9 spba1=162.946e-9
xm3 net041 db_1_ b vdd pch_lvt_mac l=40e-9 w=20e-6 multi=1 nf=20 sd=140e-9 ad=1.4e-12 as=1.48e-12 pd=22.8e-6 ps=24.96e-6 nrd=2.179e-3 nrs=2.179e-3 sa=844.248e-9 sb=844.248e-9 sa1=297.186e-9 sa2=642.759e-9 sa3=920.859e-9 sa4=802.299e-9 sb1=297.186e-9 sb2=642.759e-9 sb3=920.859e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=251.973e-9 spba=162.379e-9 sapb=294.037e-9 spba1=164.173e-9
xi0_4_ d_4_ db_4_ vdd vss INVD4BWP_LVT
xi0_3_ d_3_ db_3_ vdd vss INVD4BWP_LVT
xi0_2_ d_2_ db_2_ vdd vss INVD4BWP_LVT
xi0_1_ d_1_ db_1_ vdd vss INVD4BWP_LVT
.ends C1_BANK
** End of subcircuit definition.

** Library name: 2019_CTDSM_MAGICAL_TAPEOUT
** Cell name: FIR_DAC_CTDSM_DEC2016
** View name: schematic
.subckt FIR_DAC_CTDSM_DEC2016_schematic clk in r1 r2 rstb vdd vss
xr19  r1 net3 vss   rppolywo_m lr=19.92e-6 wr=1.2e-6 multi=1 m=1 series=1 segspace=250e-9

xr48  r2 in vss   rppolywo_m lr=19.92e-6 wr=1.2e-6 multi=1 m=1 series=1 segspace=250e-9

xi86 in clk rstb net3 vdd vss DFCNQD2BWP_LVT
.ends FIR_DAC_CTDSM_DEC2016_schematic
** End of subcircuit definition.

.subckt wrapper vss clkb1 clkb2 net062 clk rstb net052 vdda net063 net051 outm outp vip vo1p net073 vo2m vo2p net074 vim vo1m
m1 vss clkb2 vss vss nch l=280e-9 w=280e-9 m=1 nf=1 sd=140e-9 ad=30.8e-15 as=30.8e-15 pd=780e-9 ps=780e-9 nrd=112.683e-3 nrs=112.683e-3 sa=110e-9 sb=110e-9
m0 vss clkb1 vss vss nch l=280e-9 w=280e-9 m=1 nf=1 sd=140e-9 ad=30.8e-15 as=30.8e-15 pd=780e-9 ps=780e-9 nrd=112.683e-3 nrs=112.683e-3 sa=110e-9 sb=110e-9
xi88 net062 clk rstb net052 vdda vss DFCNQD2BWP_LVT
xi92 net063 clk rstb net051 vdda vss DFCNQD2BWP_LVT
xi99 outm clkb2 rstb net063 vdda vss DFCNQD2BWP_LVT
xi97 outp clkb1 rstb net062 vdda vss DFCNQD2BWP_LVT
xr16  vip vo1p vss   rppolywo_m lr=19.92e-6 wr=1.2e-6 multi=1 m=1 para=2 segspace=250e-9
xr51  net073 vo2m vss   rppolywo_m lr=19.92e-6 wr=1.2e-6 multi=1 m=1 para=5 segspace=250e-9
xr25  vo2p net074 vss   rppolywo_m lr=19.92e-6 wr=1.2e-6 multi=1 m=1 para=5 segspace=250e-9
xr47  vim vo1m vss   rppolywo_m lr=19.92e-6 wr=1.2e-6 multi=1 m=1 para=2 segspace=250e-9
.ends wrapper

.subckt DIGITAL_TOP clk vssd net072 net071 vddd vo3p vo3m outm outp
xi146 clk vss net072 net071 vddd vo3p vo3m myComparator_v3_CTDSM_DEC2016_schematic
xi128 outm outp net072 net071 vddd vssd SR_Latch_LVT
.ends DIGITAL_TOP


.subckt DIGITAL_TOP_flat clk gnd net072 net071 vdd vo3p vo3m outm outp
xi128 outm outp net072 net071 vdd gnd SR_Latch_LVT
xm0 gnd intern gnd gnd nch_lvt_mac l=1e-6 w=1.05e-6 multi=1 nf=1 sd=140e-9 ad=115.5e-15 as=115.5e-15 pd=2.32e-6 ps=2.32e-6 nrd=34.495e-3 nrs=34.495e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=3e-6 spa1=3e-6 spa2=3e-6 spa3=3e-6 sap=1.00025e-6 spba=1.76068e-6 sapb=577.831e-9 spba1=1.84662e-6
xm22 gnd interp gnd gnd nch_lvt_mac l=1e-6 w=1.05e-6 multi=1 nf=1 sd=140e-9 ad=115.5e-15 as=115.5e-15 pd=2.32e-6 ps=2.32e-6 nrd=34.495e-3 nrs=34.495e-3 sa=110e-9 sb=110e-9 sa1=110e-9 sa2=110e-9 sa3=110e-9 sa4=110e-9 sb1=110e-9 sb2=110e-9 sb3=110e-9 spa=3e-6 spa1=3e-6 spa2=3e-6 spa3=3e-6 sap=1.00025e-6 spba=1.76068e-6 sapb=577.831e-9 spba1=1.84662e-6
xm16 net072 crossp gnd gnd nch_lvt_mac l=40e-9 w=1.44e-6 multi=1 nf=4 sd=140e-9 ad=100.8e-15 as=129.6e-15 pd=2e-6 ps=2.88e-6 nrd=38.421e-3 nrs=38.421e-3 sa=256.788e-9 sb=256.788e-9 sa1=172.516e-9 sa2=246.369e-9 sa3=404.983e-9 sa4=247.435e-9 sb1=172.516e-9 sb2=246.369e-9 sb3=404.983e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=170.416e-9 spba=168.903e-9 sapb=240.571e-9 spba1=170.77e-9
xm17 net071 crossn gnd gnd nch_lvt_mac l=40e-9 w=1.44e-6 multi=1 nf=4 sd=140e-9 ad=100.8e-15 as=129.6e-15 pd=2e-6 ps=2.88e-6 nrd=38.421e-3 nrs=38.421e-3 sa=256.788e-9 sb=256.788e-9 sa1=172.516e-9 sa2=246.369e-9 sa3=404.983e-9 sa4=247.435e-9 sb1=172.516e-9 sb2=246.369e-9 sb3=404.983e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=170.416e-9 spba=168.903e-9 sapb=240.571e-9 spba1=170.77e-9
xm4 crossn crossp intern gnd nch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 sd=140e-9 ad=134.4e-15 as=172.8e-15 pd=2.48e-6 ps=3.6e-6 nrd=22.768e-3 nrs=22.768e-3 sa=256.788e-9 sb=256.788e-9 sa1=172.516e-9 sa2=246.369e-9 sa3=404.983e-9 sa4=247.435e-9 sb1=172.516e-9 sb2=246.369e-9 sb3=404.983e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=170.416e-9 spba=168.903e-9 sapb=240.571e-9 spba1=170.77e-9
xm3 crossp crossn interp gnd nch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 sd=140e-9 ad=134.4e-15 as=172.8e-15 pd=2.48e-6 ps=3.6e-6 nrd=22.768e-3 nrs=22.768e-3 sa=256.788e-9 sb=256.788e-9 sa1=172.516e-9 sa2=246.369e-9 sa3=404.983e-9 sa4=247.435e-9 sb1=172.516e-9 sb2=246.369e-9 sb3=404.983e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=170.416e-9 spba=168.903e-9 sapb=240.571e-9 spba1=170.77e-9
xm7 net069 clk gnd gnd nch_lvt_mac l=40e-9 w=6.9e-6 multi=1 nf=15 sd=140e-9 ad=501.4e-15 as=501.4e-15 pd=9.54e-6 ps=9.54e-6 nrd=5.906e-3 nrs=5.906e-3 sa=675.603e-9 sb=675.603e-9 sa1=269.666e-9 sa2=545.885e-9 sa3=818.344e-9 sa4=642.475e-9 sb1=269.666e-9 sb2=545.885e-9 sb3=818.344e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=234.015e-9 spba=162.859e-9 sapb=280.924e-9 spba1=164.658e-9
xm5 intern vo3p net069 gnd nch_lvt_mac l=40e-9 w=14.4e-6 multi=1 nf=15 sd=140e-9 ad=1.0464e-12 as=1.0464e-12 pd=17.54e-6 ps=17.54e-6 nrd=2.264e-3 nrs=2.264e-3 sa=675.603e-9 sb=675.603e-9 sa1=269.666e-9 sa2=545.885e-9 sa3=818.344e-9 sa4=642.475e-9 sb1=269.666e-9 sb2=545.885e-9 sb3=818.344e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=234.015e-9 spba=162.859e-9 sapb=280.924e-9 spba1=164.658e-9
xm6 interp vo3m net069 gnd nch_lvt_mac l=40e-9 w=14.4e-6 multi=1 nf=15 sd=140e-9 ad=1.0464e-12 as=1.0464e-12 pd=17.54e-6 ps=17.54e-6 nrd=2.264e-3 nrs=2.264e-3 sa=675.603e-9 sb=675.603e-9 sa1=269.666e-9 sa2=545.885e-9 sa3=818.344e-9 sa4=642.475e-9 sb1=269.666e-9 sb2=545.885e-9 sb3=818.344e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=234.015e-9 spba=162.859e-9 sapb=280.924e-9 spba1=164.658e-9
xm8 net072 crossp vdd vdd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 sd=140e-9 ad=134.4e-15 as=172.8e-15 pd=2.48e-6 ps=3.6e-6 nrd=28.999e-3 nrs=28.999e-3 sa=256.788e-9 sb=256.788e-9 sa1=172.516e-9 sa2=246.369e-9 sa3=404.983e-9 sa4=247.435e-9 sb1=172.516e-9 sb2=246.369e-9 sb3=404.983e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=170.416e-9 spba=168.903e-9 sapb=240.571e-9 spba1=170.77e-9
xm18 intern clk vdd vdd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 sd=140e-9 ad=134.4e-15 as=172.8e-15 pd=2.48e-6 ps=3.6e-6 nrd=28.999e-3 nrs=28.999e-3 sa=256.788e-9 sb=256.788e-9 sa1=172.516e-9 sa2=246.369e-9 sa3=404.983e-9 sa4=247.435e-9 sb1=172.516e-9 sb2=246.369e-9 sb3=404.983e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=170.416e-9 spba=168.903e-9 sapb=240.571e-9 spba1=170.77e-9
xm15 net071 crossn vdd vdd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 sd=140e-9 ad=134.4e-15 as=172.8e-15 pd=2.48e-6 ps=3.6e-6 nrd=28.999e-3 nrs=28.999e-3 sa=256.788e-9 sb=256.788e-9 sa1=172.516e-9 sa2=246.369e-9 sa3=404.983e-9 sa4=247.435e-9 sb1=172.516e-9 sb2=246.369e-9 sb3=404.983e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=170.416e-9 spba=168.903e-9 sapb=240.571e-9 spba1=170.77e-9
xm2 interp clk vdd vdd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 sd=140e-9 ad=134.4e-15 as=172.8e-15 pd=2.48e-6 ps=3.6e-6 nrd=28.999e-3 nrs=28.999e-3 sa=256.788e-9 sb=256.788e-9 sa1=172.516e-9 sa2=246.369e-9 sa3=404.983e-9 sa4=247.435e-9 sb1=172.516e-9 sb2=246.369e-9 sb3=404.983e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=170.416e-9 spba=168.903e-9 sapb=240.571e-9 spba1=170.77e-9
xm1 crossn clk vdd vdd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 sd=140e-9 ad=134.4e-15 as=172.8e-15 pd=2.48e-6 ps=3.6e-6 nrd=28.999e-3 nrs=28.999e-3 sa=256.788e-9 sb=256.788e-9 sa1=172.516e-9 sa2=246.369e-9 sa3=404.983e-9 sa4=247.435e-9 sb1=172.516e-9 sb2=246.369e-9 sb3=404.983e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=170.416e-9 spba=168.903e-9 sapb=240.571e-9 spba1=170.77e-9
xm12 crossp clk vdd vdd pch_lvt_mac l=40e-9 w=1.92e-6 multi=1 nf=4 sd=140e-9 ad=134.4e-15 as=172.8e-15 pd=2.48e-6 ps=3.6e-6 nrd=28.999e-3 nrs=28.999e-3 sa=256.788e-9 sb=256.788e-9 sa1=172.516e-9 sa2=246.369e-9 sa3=404.983e-9 sa4=247.435e-9 sb1=172.516e-9 sb2=246.369e-9 sb3=404.983e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=170.416e-9 spba=168.903e-9 sapb=240.571e-9 spba1=170.77e-9
xm14 crossn crossp vdd vdd pch_lvt_mac l=40e-9 w=3.84e-6 multi=1 nf=8 sd=140e-9 ad=268.8e-15 as=307.2e-15 pd=4.96e-6 ps=6.08e-6 nrd=14.333e-3 nrs=14.333e-3 sa=420.901e-9 sb=420.901e-9 sa1=218.025e-9 sa2=376.982e-9 sa3=612.324e-9 sa4=401.816e-9 sb1=218.025e-9 sb2=376.982e-9 sb3=612.324e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=200.193e-9 spba=164.62e-9 sapb=257.228e-9 spba1=166.439e-9
xm13 crossp crossn vdd vdd pch_lvt_mac l=40e-9 w=3.84e-6 multi=1 nf=8 sd=140e-9 ad=268.8e-15 as=307.2e-15 pd=4.96e-6 ps=6.08e-6 nrd=14.333e-3 nrs=14.333e-3 sa=420.901e-9 sb=420.901e-9 sa1=218.025e-9 sa2=376.982e-9 sa3=612.324e-9 sa4=401.816e-9 sb1=218.025e-9 sb2=376.982e-9 sb3=612.324e-9 spa=140e-9 spa1=140e-9 spa2=140e-9 spa3=140e-9 sap=200.193e-9 spba=164.62e-9 sapb=257.228e-9 spba1=166.439e-9
.ends DIGITAL_TOP_flat
** Library name: 2019_CTDSM_MAGICAL_TAPEOUT
** Cell name: CTDSM_CORE_NEW
** View name: schematic
.subckt CTDSM_CORE_NEW_schematic clk clkb1 clkb2 ibias1 ibias2 outm outp rstb vdda vim vip vssa vddd vssd vo1m vo1p vo2m vo2p vo3m vo3p
xi161 ibias2 vdda vo2m vo2p vo3p vo3m vssa Gm2_v5_Practice_schematic
xi160 ibias1 vdda vo1m vo1p vo2p vo2m vssa Gm1_v5_Practice_schematic
xi155 clkb2 net063 vo3p vo3m rstb vdda vssa C_DAC_CTDSM_DEC2016_schematic
xi154 clkb1 net062 vo3m vo3p rstb vdda vssa C_DAC_CTDSM_DEC2016_schematic
digital0 clk vssd net072 net071 vddd vo3p vo3m outm outp DIGITAL_TOP_flat
** xi168 vo3p vo3m vss d3_4_ d3_3_ d3_2_ d3_1_ vdda C2_BANK
** xi167 net074 net073 vss d2_4_ d2_3_ d2_2_ d2_1_ vdda C2_BANK
** xi166 vo1p vo1m vss d1_4_ d1_3_ d1_2_ d1_1_ vdda C1_BANK
xc1_3_ vo1p vo1m cfmom_2t nr=260 lr=36e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc1_2_ vo1p vo1m cfmom_2t nr=260 lr=36e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc1_1_ vo1p vo1m cfmom_2t nr=260 lr=36e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc1_0_ vo1p vo1m cfmom_2t nr=260 lr=36e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc0 net074 net073 cfmom_2t nr=210 lr=34e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xc3 vo3p vo3m cfmom_2t nr=220 lr=34e-6 w=70e-9 s=70e-9 stm=2 spm=5 multi=1 ftip=140e-9
xi153 clk net051 vo1m vo1m rstb vdda vssa FIR_DAC_CTDSM_DEC2016_schematic
xi152 clk net052 vo1p vo1p rstb vdda vssa FIR_DAC_CTDSM_DEC2016_schematic
w1 vssa clkb1 clkb2 net062 clk rstb net052 vdda net063 net051 outm outp vip vo1p net073 vo2m vo2p net074 vim vo1m wrapper
.ends CTDSM_CORE_NEW_schematic
** End of subcircuit definition.
** .END
