source ./../skywaterPDK/extract.sh COMPARATOR_flat.gds
netgen -batch lvs "gds-extracted-COMPARATOR_flat.spice COMPARATOR_flat" "comp.lvs.spice COMPARATOR" ./../skywaterPDK/sky130A_setup.tcl
