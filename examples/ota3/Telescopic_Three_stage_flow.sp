// Generated for: spectre
// Generated on: May  3 15:01:57 2019
// Design library name: 2019_Synthesizing_OTA_copy
// Design cell name: Telescopic_Three_stage_flow
// Design view name: schematic
//simulator lang=spectre
//global 0
//include "/usr/local/packages/tsmc_40/pdk/tsmcN40/../models/spectre/toplevel.scs" section=top_tt

// Library name: 2019_Synthesizing_OTA_copy
// Cell name: Telescopic_Three_stage_flow
// View name: schematic
// terminal mapping: INM	= INM
//                   INP	= INP
//                   OUTM	= OUTM
//                   OUTP	= OUTP
//                   VBN1	= VBN1
//                   VDD	= VDD
//                   VREF	= VREF
//                   VSS	= VSS
topckt Telescopic_Three_stage_flow INM INP OUTM OUTP VBN1 VDD VREF VSS

// nch Instance M60 = spectre device M60
M60 (net057 net056 net058 VSS) nch l=200n w=8.8u m=1 nf=4

// nch Instance M105 = spectre device M105
M105 (net058 VBN VSS VSS) nch l=400n w=88.0u m=1 nf=8

// nch Instance M93 = spectre device M93
M93 (VCMFB3 VREF net058 VSS) nch l=200n w=8.8u m=1 nf=4

// nch Instance M110 = spectre device M110
M110 (VOM2 VCMFB2 VSS VSS) nch l=400n w=20u m=1 nf=8

// nch Instance M75 = spectre device M75
M75 (VOP2 VCMFB2 VSS VSS) nch l=400n w=20u m=1 nf=8

// nch Instance M97 = spectre device M97
M97 (VBP VBN VSS VSS) nch l=400n w=44.0u m=1 nf=4

// nch Instance M24 = spectre device M24
M24 (VBN1 VBN1 VBN VSS) nch l=1u w=60u m=1 nf=10

// nch Instance M96 = spectre device M96
M96 (VBP1 VBN VSS VSS) nch l=400n w=44.0u m=1 nf=4

// nch Instance M21 = spectre device M21
M21 (VBN VBN VSS VSS) nch l=400n w=44.0u m=1 nf=4

// nch Instance M113 = spectre device M113
M113 (OUTP VOM2 VSS VSS) nch l=400n w=40u m=1 nf=16

// nch Instance M13 = spectre device M13
M13 (OUTM VOP2 VSS VSS) nch l=400n w=40u m=1 nf=16

// nch Instance M102 = spectre device M102
M102 (VOM2 VBN VSS VSS) nch l=400n w=44.0u m=1 nf=4

// nch Instance M18 = spectre device M18
M18 (VOP2 VBN VSS VSS) nch l=400n w=44.0u m=1 nf=4

// nch Instance M106 = spectre device M106
M106 (VOP1 VBN1 net2 VSS) nch l=200n w=60u m=1 nf=10

// nch Instance M4 = spectre device M4
M4 (net1 VBN VSS VSS) nch l=400n w=308.000000u m=1 nf=28

// nch Instance M3 = spectre device M3
M3 (VOM1 VBN1 net4 VSS) nch l=200n w=60u m=1 nf=10

// pch Instance M63 = spectre device M63
M63 (net057 net057 VDD VDD) pch l=200n w=10u m=1 nf=4

// pch Instance M112 = spectre device M112
M112 (OUTP VCMFB3 VDD VDD) pch l=800n w=400.0000u m=1 nf=40

// pch Instance M73 = spectre device M73
M73 (OUTM VCMFB3 VDD VDD) pch l=800n w=400.0000u m=1 nf=40

// pch Instance M114 = spectre device M114
M114 (VCMFB3 net057 VDD VDD) pch l=200n w=10u m=1 nf=4

// pch Instance M26 = spectre device M26
M26 (VBP1 VBP1 VDD VDD) pch l=1.6u w=50u m=1 nf=10

// pch Instance M90 = spectre device M90
M90 (VBP VBP VDD VDD) pch l=800n w=40u m=1 nf=4

// pch Instance M111 = spectre device M111
M111 (OUTP VBP VDD VDD) pch l=800n w=400.0000u m=1 nf=40

// pch Instance M14 = spectre device M14
M14 (OUTM VBP VDD VDD) pch l=800n w=400.0000u m=1 nf=40

// pch Instance M109 = spectre device M109
M109 (VOM2 VOP1 VDD VDD) pch l=400n w=800.0000u m=1 nf=80

// pch Instance M9 = spectre device M9
M9 (VOP2 VOM1 VDD VDD) pch l=400n w=800.0000u m=1 nf=80

// pch Instance M7 = spectre device M7
M7 (net07 VCMFB1 VDD VDD) pch l=800n w=40u m=1 nf=4

// pch Instance M6 = spectre device M6
M6 (VOM1 VBP1 net07 VDD) pch l=200n w=50u m=1 nf=10

// pch Instance M107 = spectre device M107
M107 (VOP1 VBP1 net08 VDD) pch l=200n w=50u m=1 nf=10

// pch Instance M108 = spectre device M108
M108 (net08 VCMFB1 VDD VDD) pch l=800n w=40u m=1 nf=4

// cfmom Instance C7 = spectre device C7
C7 (VOP1 OUTM VSS) cfmom nr=50 lr=6u w=70n s=70n stm=1 spm=6 multi=1 \
        ftip=140.0n dmflag=0

// cfmom Instance C6 = spectre device C6
C6 (VOM1 OUTP VSS) cfmom nr=50 lr=6u w=70n s=70n stm=1 spm=6 multi=1 \
        ftip=140.0n dmflag=0

// cfmom Instance C5 = spectre device C5
C5 (VOM2 OUTM VSS) cfmom nr=88 lr=10u w=70n s=70n stm=1 spm=6 multi=1 \
        ftip=140.0n dmflag=0

// cfmom Instance C1 = spectre device C1
C1 (VOM2 OUTP VSS) cfmom nr=88 lr=10u w=70n s=70n stm=1 spm=6 multi=1 \
        ftip=140.0n dmflag=0
R8 (VCMFB2 VOM2 VSS ) rppolywo_m lr=10u wr=400n multi=(1) m=1

//Series configuration of R4
R4 (VOP1 VCMFB1 VSS ) rppolywo_m lr=10u wr=400n multi=(1) m=1 series=10 segspace=250n
//End of R4

R9 (OUTP net056 VSS ) rppolywo_m lr=8u wr=400n multi=(1) m=1

//Series configuration of R6q
R6q (VCMFB1 VOM1 VSS ) rppolywo_m lr=10u wr=400n multi=(1) m=1 series=10 segspace=250n
//End of R6q

R10 (net056 OUTM VSS ) rppolywo_m lr=8u wr=400n multi=(1) m=1

R7 (VOP2 VCMFB2 VSS ) rppolywo_m lr=10u wr=400n multi=(1) m=1


// nch_25ud18_mac Instance M12 = spectre device M12
M12 (net4 INP net1 VSS) nch_25ud18_mac l=800n w=90u multi=1 nf=10 

// nch_25ud18_mac Instance M2 = spectre device M2
M2 (net2 INM net1 VSS) nch_25ud18_mac l=800n w=90u multi=1 nf=10
ends Telescopic_Three_stage_flow
//simulatorOptions options reltol=1e-3 vabstol=1e-6 iabstol=1e-12 temp=27 \
//    tnom=27 scalem=1.0 scale=1.0 gmin=1e-12 rforce=1 maxnotes=5 maxwarns=5 \
//    digits=5 cols=80 pivrel=1e-3 sensfile="../psf/sens.output" \
//    checklimitdest=psf 
//modelParameter info what=models where=rawfile
//element info what=inst where=rawfile
//outputParameter info what=output where=rawfile
//designParamVals info what=parameters where=rawfile
//primitives info what=primitives where=rawfile
//subckts info what=subckts where=rawfile
//saveOptions options save=allpub
