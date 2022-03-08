source ./../skywaterPDK/extract.sh Core_test_flow_flat.gds
netgen -batch lvs "gds-extracted-Core_test_flow_flat.spice Core_test_flow_flat" "ota1.lvs.spice Core_test_flow" ./../skywaterPDK/sky130A_setup.tcl
