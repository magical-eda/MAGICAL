#!/bin/bash
# contact: the new pin shapes need to be add
# COMPARATOR_PRE_AMP_test_flow.gr: name of the original .gr file
# out.gr: The file name for the output merged .gr file
# VDD: which net should the additional pins be added to
python addPinsToGR.py Core_test_flow.wcon Core_test_flow.gr temp.gr VDD
python addPinsToGR.py Core_test_flow.sub temp.gr out.gr GND
rm temp.gr
