** Generated for: hspiceD
** Generated on: Mar 18 15:42:36 2019
** Design library name: CTDTDSM_NSSAR
** Design cell name: BUF11
** Design view name: schematic


** .TEMP 25.0
** .OPTION
** +    ARTIST=2
** +    INGOLD=2
** +    PARHIER=LOCAL
** +    PSF=2
** .LIB "/usr/local/packages/tsmc_40/pdk/models/hspice/toplevel.l" top_tt

** Library name: CTDTDSM_NSSAR
** Cell name: INV
** View name: schematic
.subckt INV i vdd vss zn
m1 zn i vss vss nch_lvt l=40e-9 w=155e-9 m=1 nf=1 sd=140e-9 ad=17.05e-15 as=17.05e-15 pd=530e-9 ps=530e-9 nrd=249.512e-3 nrs=249.512e-3 sa=110e-9 sb=110e-9
** Some test commment
m0 zn i vdd vdd pch_lvt l=40e-9 w=205e-9 m=1 nf=1 sd=140e-9 ad=22.55e-15 as=22.55e-15 pd=630e-9 ps=630e-9 nrd=479.34e-3 nrs=479.34e-3 sa=110e-9 sb=110e-9
.ends INV
** End of subcircuit definition.

** Library name: CTDTDSM_NSSAR
** Cell name: BUF11
** View name: schematic
.topckt BUFF in out vdd vss
xi1 net11 vdd vss out INV
xi0 in vdd vss net11 INV
.ends BUF11
** End of subcircuit definition.
