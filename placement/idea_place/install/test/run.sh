##
# @file   run.sh
# @author Yibo Lin
# @date   Dec 2018
#
#!/bin/bash

CUR_SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
if [ -e "${CUR_SCRIPT_DIR}/../../../test/color.sh" ]; then 
    source ${CUR_SCRIPT_DIR}/../../../test/color.sh
fi 

if [ ! -n "$1" ]
then
  echo -e "${STATUS_PREFIX_COLOR}Usage:${NC} `basename $0` circuit techfile spacing_rule width_area_rule"
  return 
fi  

# example 
# source test/run.sh /home/local/eda06/biyingxu/Research/utda-ams-place/files/benchmarks/COMPARATOR_PRE_AMP_test_flow /home/local/eda06/biyingxu/Research/idea_place/techfile.simple /home/local/eda06/biyingxu/Research/idea_place/spacing.rules /home/local/eda06/biyingxu/Research/idea_place/width_area.rules

CIRCUIT=$1
CIRCUIT_NAME=$(basename ${CIRCUIT})
TECHFILE=$2
SPACING_RULE_FILE=$3
WIDTH_AREA_RULE_FILE=$4

echo "CIRCUIT = ${CIRCUIT}"
echo "CIRCUIT_NAME = ${CIRCUIT_NAME}"
echo "TECHFILE = ${TECHFILE}"
echo "SPACING_RULE_FILE = ${SPACING_RULE_FILE}"
echo "WIDTH_AREA_RULE_FILE = ${WIDTH_AREA_RULE_FILE}"

echo -e "${STATUS_PREFIX_COLOR}${CIRCUIT_NAME}:${NC} analog placement"
mkdir -p results/${CIRCUIT_NAME}
${CUR_SCRIPT_DIR}/../bin/idea_place \
    --techfile ${TECHFILE} \
    --spacing ${SPACING_RULE_FILE} \
    --width_area ${WIDTH_AREA_RULE_FILE} \
    --input ${CIRCUIT} \
    --result results/${CIRCUIT_NAME}/  \
	--optimizer LPSOLVE
 
