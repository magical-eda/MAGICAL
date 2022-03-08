source ./../skywaterPDK/extract.sh Telescopic_Three_stage_flow_flat.gds
netgen -batch lvs "gds-extracted-Telescopic_Three_stage_flow_flat.spice Telescopic_Three_stage_flow_flat" "Telescopic_Three_stage_flow.lvs.spice Telescopic_Three_stage_flow" ./../skywaterPDK/sky130A_setup.tcl
