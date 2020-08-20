// Generated for: spectre
// Generated on: Dec  9 17:25:45 2018
// Design library name: Two_stage_miller
// Design cell name: Core
// Design view name: schematic
// simulator lang=spectre
// global 0
// include "/usr/local/packages/tsmc_40/pdk/tsmcN40/../models/spectre/toplevel.scs" section=top_tt

// Library name: Two_stage_miller
// Cell name: Core
// View name: schematic
// terminal mapping: INM	= INM
//                   INP	= INP
//                   OUTM	= OUTM
//                   OUTP	= OUTP
//                   VBIAS_P	= VBIAS_P
//                   VDD	= VDD
//                   VREF	= VREF
//                   VSS	= VSS
topckt Core_test_flow INM INP OUTM OUTP VBIAS_P VDD VREF VSS
// pch Instance MP1c = spectre device MP1c
MP1c (net020 VBIAS_P VDD VDD) pch l=120.0n w=16.0u m=1 nf=8 

// pch Instance M20 = spectre device M20
M20 (net028 VBIAS_P VDD VDD) pch l=120.0n w=16.0u m=1 nf=8 

// pch Instance M7 = spectre device M7
M7 (vbias_n VBIAS_P VDD VDD) pch l=120.0n w=16.0u m=1 nf=8 

// pch Instance M5 = spectre device M5
M5 (VBIAS_P VBIAS_P VDD VDD) pch l=120.0n w=16.0u m=1 nf=8 

// pch Instance MP1a = spectre device MP1a
MP1a (intm VBIAS_P VDD VDD) pch l=120.0n w=40u m=1 nf=20 

// pch Instance MP1b = spectre device MP1b
MP1b (intp VBIAS_P VDD VDD) pch l=120.0n w=40u m=1 nf=20 

// cfmom_2t Instance C1 = spectre device C1
C1 (OUTM net037) cfmom_2t nr=210 lr=13.0u w=70n s=70n stm=3 spm=6 multi=1 \
        ftip=140.0n

// cfmom_2t Instance C0 = spectre device C0
C0 (OUTP net047) cfmom_2t nr=210 lr=13.0u w=70n s=70n stm=3 spm=6 multi=1 \
        ftip=140.0n

// nch_lvt Instance M10 = spectre device M10
M10 (net7 vbias_n VSS VSS) nch_lvt l=120.0n w=32.0u m=1 nf=16 

// nch_lvt Instance M9 = spectre device M9
M9 (OUTM vbias_n VSS VSS) nch_lvt l=120.0n w=12.0u m=1 nf=6 

// nch_lvt Instance M6 = spectre device M6
M6 (OUTP vbias_n VSS VSS) nch_lvt l=120.0n w=12.0u m=1 nf=6 

// nch_lvt Instance M13 = spectre device M13
M13 (vcmfb vcmfb VSS VSS) nch_lvt l=120.0n w=4u m=1 nf=2 

// nch_lvt Instance M12 = spectre device M12
M12 (net025 net025 VSS VSS) nch_lvt l=120.0n w=4u m=1 nf=2 

// nch_lvt Instance M11 = spectre device M11
M11 (net7 vcmfb VSS VSS) nch_lvt l=120.0n w=8u m=1 nf=4 

// nch_lvt Instance M4 = spectre device M4
M4 (vbias_n vbias_n VSS VSS) nch_lvt l=120.0n w=8u m=1 nf=4 

// pch_lvt Instance M19 = spectre device M19
M19 (net025 VREF net028 VDD) pch_lvt l=120.0n w=16.0u m=1 nf=8 

// pch_lvt Instance M17 = spectre device M17
M17 (net025 VREF net020 VDD) pch_lvt l=120.0n w=16.0u m=1 nf=8 

// pch_lvt Instance M15 = spectre device M15
M15 (vcmfb OUTP net020 VDD) pch_lvt l=120.0n w=16.0u m=1 nf=8 

// pch_lvt Instance M25 = spectre device M25
M25 (net037 VSS intp VDD) pch_lvt l=120.0n w=24.0u m=1 nf=12 

// pch_lvt Instance M23 = spectre device M23
M23 (net047 VSS intm VDD) pch_lvt l=120.0n w=24.0u m=1 nf=12 

// pch_lvt Instance M3 = spectre device M3
M3 (OUTP intm VDD VDD) pch_lvt l=120.0n w=24.0u m=1 nf=12 

// pch_lvt Instance M1 = spectre device M1
M1 (OUTM intp VDD VDD) pch_lvt l=120.0n w=24.0u m=1 nf=12 

// pch_lvt Instance M18 = spectre device M18
M18 (vcmfb OUTM net028 VDD) pch_lvt l=120.0n w=16.0u m=1 nf=8 

// nch_25ud18_mac Instance M0 = spectre device M0
M0 (intp INM net7 VSS) nch_25ud18_mac l=500n w=60u multi=1 nf=12 

// nch_25ud18_mac Instance M2 = spectre device M2
M2 (intm INP net7 VSS) nch_25ud18_mac l=500n w=60u multi=1 nf=12 
ends Core_test_flow

// simulatorOptions options reltol=1e-3 vabstol=1e-6 iabstol=1e-12 temp=27 \
//     tnom=27 scalem=1.0 scale=1.0 gmin=1e-12 rforce=1 maxnotes=5 maxwarns=5 \
//     digits=5 cols=80 pivrel=1e-3 sensfile="../psf/sens.output" \
//     checklimitdest=psf 
// modelParameter info what=models where=rawfile
// element info what=inst where=rawfile
// outputParameter info what=output where=rawfile
// designParamVals info what=parameters where=rawfile
// primitives info what=primitives where=rawfile
// subckts info what=subckts where=rawfile
// saveOptions options save=allpub
