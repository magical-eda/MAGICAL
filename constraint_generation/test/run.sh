##
# @file   run.sh
# @author anoymous anoymous, anoymous anoymous
# @date   Dec 2018
#
#!/bin/bash

# get script directory instead of execution directory
CUR_SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
if [ -e "${CUR_SCRIPT_DIR}/../../test/color.sh" ]; then 
    source ${CUR_SCRIPT_DIR}/../../test/color.sh
fi 

if [ ! -n "$1" ]
then
  echo -e "${STATUS_PREFIX_COLOR}Usage:${NC} `basename $0` circuit"
  return 
fi  

CIRCUIT=$1
CIRCUIT_NAME=$(basename ${CIRCUIT})
CIRCUIT_EXTENSION="${CIRCUIT_NAME##*.}"
CIRCUIT_NAME="${CIRCUIT_NAME%.*}"
RESULT_DIR=results

echo "CIRCUIT = ${CIRCUIT}"
echo "CIRCUIT_NAME = ${CIRCUIT_NAME}"
echo "CIRCUIT_EXTENSION = ${CIRCUIT_EXTENSION}"
echo "CUR_SCRIPT_DIR = ${CUR_SCRIPT_DIR}"

PYTHON_DIR=${CUR_SCRIPT_DIR}/../python
SFA_DIR=${CUR_SCRIPT_DIR}/../signal_flow_analysis

echo -e "${STATUS_PREFIX_COLOR}${CIRCUIT_NAME}:${NC} Parsing spectre netlist and device generation"
rm -rf ${RESULT_DIR}/${CIRCUIT_NAME}/gds
mkdir ${RESULT_DIR}/${CIRCUIT_NAME}/gds
python $PYTHON_DIR/devgen.py ${CIRCUIT} ${RESULT_DIR}/${CIRCUIT_NAME}/

echo -e "${STATUS_PREFIX_COLOR}${CIRCUIT_NAME}:${NC} Generating constraints for placement and routing"
$SFA_DIR/bin/SFA ${RESULT_DIR}/${CIRCUIT_NAME}/${CIRCUIT_NAME}.initObj ${RESULT_DIR}/${CIRCUIT_NAME}/${CIRCUIT_NAME} > /dev/null

