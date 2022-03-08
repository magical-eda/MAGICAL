source ./../skywaterPDK/extract.sh OTA_2_flat.gds
netgen -batch lvs "gds-extracted-OTA_2_flat.spice OTA_2_flat" "ota2.lvs.spice OTA_2" ./../skywaterPDK/sky130A_setup.tcl
