##
# @file   ../test/run.sh
# @author anoymous anoymous
# @date   Dec 2018
#
#!/bin/bash

CUR_SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
if [ -e "${CUR_SCRIPT_DIR}/../../test/color.sh" ]; then 
    source ${CUR_SCRIPT_DIR}/../../test/color.sh
fi 

if [ ! -n "$1" ]
then
  echo -e "${STATUS_PREFIX_COLOR}Usage:${NC} `basename $0` circuit_GDSII place_sol pin wcon sub iopin symnet lef techfile"
  return 
fi  

CIRCUIT_GDSII=$1
CIRCUIT_NAME=$(basename ${CIRCUIT_GDSII})
CIRCUIT_NAME="${CIRCUIT_NAME%.*}"
CIRCUIT_NAME="${CIRCUIT_NAME%.*}"
PLACE_SOL_FILE=$2
PIN_FILE=$3
WCON_FILE=$4
SUB_FILE=$5
IOPIN_FILE=$6
SYMNET_FILE=$7
LEF_FILE=$8
TECHFILE=$9
RESULT_DIR=results

echo "CIRCUIT_GDSII = ${CIRCUIT_GDSII}"
echo "CIRCUIT_NAME = ${CIRCUIT_NAME}"

# generate gr file 
echo -e "${STATUS_PREFIX_COLOR}${CIRCUIT_NAME}:${NC} generate gr file"
python ${CUR_SCRIPT_DIR}/../python/getPinLoc.py -f ${PLACE_SOL_FILE} -p ${PIN_FILE} -n ${RESULT_DIR}/${CIRCUIT_NAME}/${CIRCUIT_NAME}.connection -o ${RESULT_DIR}/${CIRCUIT_NAME}/${CIRCUIT_NAME}.gr

# merge GR 
echo -e "${STATUS_PREFIX_COLOR}${CIRCUIT_NAME}:${NC} merge VDD/VSS contacts to GR"
python ${CUR_SCRIPT_DIR}/../python/addPinsToGR.py ${WCON_FILE} ${RESULT_DIR}/${CIRCUIT_NAME}/${CIRCUIT_NAME}.gr temp.gr VDD
python ${CUR_SCRIPT_DIR}/../python/addPinsToGR.py ${SUB_FILE} temp.gr ${RESULT_DIR}/${CIRCUIT_NAME}/ROUTING_INPUT.gr GND
rm temp.gr

# routing 
echo -e "${STATUS_PREFIX_COLOR}${CIRCUIT_NAME}:${NC} analog routing"
${CUR_SCRIPT_DIR}/../arouterex/bin/ARouter\
    --designType ispd08pin \
    --designFile ${RESULT_DIR}/${CIRCUIT_NAME}/ROUTING_INPUT.gr \
    --techLef ${LEF_FILE} \
    --techfile ${TECHFILE} \
    --out ${RESULT_DIR}/${CIRCUIT_NAME}/${CIRCUIT_NAME}.route.gds \
    --placementLayout ${CIRCUIT_GDSII} \
    --iopin ${IOPIN_FILE} \
    --symnet ${SYMNET_FILE}
