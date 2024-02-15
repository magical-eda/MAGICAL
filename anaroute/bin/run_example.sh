#!/bin/bash

./anaroute --tech_lef ../mockPDK/mock.lef \
           --tech_file ../mockPDK/mock.techfile \
           --design_file ../mockPDK/mock.netlist \
           --placement_layout ../mockPDK/mock.place.gds \
           --iopin ../mockPDK/mock.iopin \
           --spec ../mockPDK/mock.spec \
           --out mock.route.gds
