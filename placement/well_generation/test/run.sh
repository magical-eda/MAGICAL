##
# @file   run.sh
# @author anoymous anoymous
# @date   Dec 2018
#
#!/bin/bash

# top script to run well generation for one circuit 
CUR_SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
if [ -e "${CUR_SCRIPT_DIR}/../../../test/color.sh" ]; then 
    source ${CUR_SCRIPT_DIR}/../../../test/color.sh
fi 

if [ ! -n "$1" ]
then
  echo -e "${STATUS_PREFIX_COLOR}Usage:${NC} `basename $0` circuit techfile spacing_rule enclosure_rule well_contact_GDSII placement_result library_name"
  return 
fi  

# example 
# source test/run.sh benchmarks/COMPARATOR_PRE_AMP_test_flow  techfile.simple spacing.rules enclosure.rules nwell.gds result_legal_detail.txt
CIRCUIT=$1
TECHFILE=$2
SPACING_RULE_FILE=$3
ENCLOSURE_RULE_FILE=$4
WELL_CON_GDS_FILE=$5
PLACE_RESULT_FILE=$6
CIRCUIT_NAME=$(basename "$CIRCUIT")
RESULT_DIR=results
LIB_NAME="TEST_FLOW"

echo "CIRCUIT_NAME = ${CIRCUIT_NAME}"
echo "LIB_NAME = ${LIB_NAME}"

# Data preprocessing: extract specific layers 
echo -e "${STATUS_PREFIX_COLOR}${CIRCUIT_NAME}:${NC} data preprocessing and layer extraction"
mkdir -p ${RESULT_DIR}/${CIRCUIT_NAME}/DataTest/gdsii
echo ${CIRCUIT}/${CIRCUIT_NAME}.place.gds
${CUR_SCRIPT_DIR}/../bin/well_gen ${CIRCUIT_NAME} ${CIRCUIT}/${CIRCUIT_NAME}.place.gds ${RESULT_DIR}/${CIRCUIT_NAME}/DataTest/gdsii/${CIRCUIT_NAME}_DEBUG.gds ${RESULT_DIR}/${CIRCUIT_NAME}/DataTest/gdsii/${CIRCUIT_NAME}_TRAIN.gds
mv ./${CIRCUIT_NAME}.gds2image.offset ${CIRCUIT}/${CIRCUIT_NAME}.gds2image.offset

# Convert GDSII to image 
echo -e "${STATUS_PREFIX_COLOR}${CIRCUIT_NAME}:${NC} GDSII to images convertion"
mkdir -p ${RESULT_DIR}/${CIRCUIT_NAME}/DataTest/merged_images
mkdir -p ${RESULT_DIR}/${CIRCUIT_NAME}/DataTest/images
python ${CUR_SCRIPT_DIR}/../python/parser_v2.py -f ${CIRCUIT_NAME}_TRAIN.gds -a ${RESULT_DIR}/${CIRCUIT_NAME}/DataTest/

# Run GAN to generate well guidance 
echo -e "${STATUS_PREFIX_COLOR}${CIRCUIT_NAME}:${NC} generate WELL guidance using GAN"
mkdir -p ${RESULT_DIR}/${CIRCUIT_NAME}/DataTest/GAN/${CIRCUIT_NAME}/test_input
mkdir -p ${RESULT_DIR}/${CIRCUIT_NAME}/DataTest/GAN/${CIRCUIT_NAME}/sample
mkdir -p ${RESULT_DIR}/${CIRCUIT_NAME}/DataTest/GAN/${CIRCUIT_NAME}/test
cp ${RESULT_DIR}/${CIRCUIT_NAME}/DataTest/merged_images/${CIRCUIT_NAME}*.png ${RESULT_DIR}/${CIRCUIT_NAME}/DataTest/GAN/${CIRCUIT_NAME}/test_input
python ${CUR_SCRIPT_DIR}/../pix2pix-tensorflow/main.py --checkpoint_dir=${CUR_SCRIPT_DIR}/../pix2pix-tensorflow/checkpoint \
    --phase=test \
    --test_input_dir=${RESULT_DIR}/${CIRCUIT_NAME}/DataTest/GAN/${CIRCUIT_NAME}/test_input \
    --sample_dir=${RESULT_DIR}/${CIRCUIT_NAME}/DataTest/GAN/${CIRCUIT_NAME}/sample \
    --test_dir=${RESULT_DIR}/${CIRCUIT_NAME}/DataTest/GAN/${CIRCUIT_NAME}/test

# combine image clips for one design 
echo -e "${STATUS_PREFIX_COLOR}${CIRCUIT_NAME}:${NC} combine image clips into one image"
mkdir -p ${RESULT_DIR}/${CIRCUIT_NAME}/DataTest/test
mkdir -p ${RESULT_DIR}/${CIRCUIT_NAME}/DataTest/combined_test_results_d1
mkdir -p ${RESULT_DIR}/${CIRCUIT_NAME}/DataTest/combined_test_results
cp ${RESULT_DIR}/${CIRCUIT_NAME}/DataTest/GAN/${CIRCUIT_NAME}/test/${CIRCUIT_NAME}*.png ${RESULT_DIR}/${CIRCUIT_NAME}/DataTest/test
# generate combined input 
python ${CUR_SCRIPT_DIR}/../python/combine_results.py -f ${CIRCUIT_NAME}_TRAIN.gds -a ${RESULT_DIR}/${CIRCUIT_NAME}/DataTest/ -g 1 -d 1 
# generate combined output 
python ${CUR_SCRIPT_DIR}/../python/combine_results.py -f ${CIRCUIT_NAME}_TRAIN.gds -a ${RESULT_DIR}/${CIRCUIT_NAME}/DataTest/ -g 0 -d 2 

# rectianoymousearize edges 
echo -e "${STATUS_PREFIX_COLOR}${CIRCUIT_NAME}:${NC} rectianoymousearize edges"
mkdir -p ${RESULT_DIR}/${CIRCUIT_NAME}/DataTest/${CIRCUIT_NAME}
python ${CUR_SCRIPT_DIR}/../python/legalize.py -f ${RESULT_DIR}/${CIRCUIT_NAME}/DataTest/combined_test_results/${CIRCUIT_NAME}.png -a ${RESULT_DIR}/${CIRCUIT_NAME}/DataTest/${CIRCUIT_NAME}/ -d ${RESULT_DIR}/${CIRCUIT_NAME}/DataTest/combined_test_results_d1/${CIRCUIT_NAME}.png 

# legalize well rules and insert contact 
echo "TECHFILE = ${TECHFILE}"
echo "SPACING_RULE_FILE = ${SPACING_RULE_FILE}"
echo "ENCLOSURE_RULE_FILE = ${ENCLOSURE_RULE_FILE}"
echo -e "${STATUS_PREFIX_COLOR}${CIRCUIT_NAME}:${NC} legalize WELLs and insert WELL contacts"
cp ${RESULT_DIR}/${CIRCUIT_NAME}/DataTest/${CIRCUIT_NAME}/${CIRCUIT_NAME}.contours.approx  ${RESULT_DIR}/${CIRCUIT_NAME}/${CIRCUIT_NAME}.contours.approx
${CUR_SCRIPT_DIR}/../bin/legalize \
    --circuit ${CIRCUIT_NAME} \
    --benchmark_dir ${RESULT_DIR} \
    --placement ${PLACE_RESULT_FILE} \
    --well_contact_gds ${WELL_CON_GDS_FILE} \
    --techfile ${TECHFILE} \
    --spacing ${SPACING_RULE_FILE} \
    --enclosure ${ENCLOSURE_RULE_FILE} \
	--optimizer LPSOLVE \
    --result ${RESULT_DIR}/${CIRCUIT_NAME}/DataTest
mv ${RESULT_DIR}/${CIRCUIT_NAME}/DataTest/${CIRCUIT_NAME}/${CIRCUIT_NAME}.wcon ${RESULT_DIR}/${CIRCUIT_NAME}/${CIRCUIT_NAME}.wcon
mv ${RESULT_DIR}/${CIRCUIT_NAME}/DataTest/${CIRCUIT_NAME}/${CIRCUIT_NAME}.wellgen.gds ${RESULT_DIR}/${CIRCUIT_NAME}/${CIRCUIT_NAME}.wellgen.gds
mv ${RESULT_DIR}/${CIRCUIT_NAME}/DataTest/${CIRCUIT_NAME}/${CIRCUIT_NAME}.sub ${RESULT_DIR}/${CIRCUIT_NAME}/${CIRCUIT_NAME}.sub

## only to add substrate guardring 
#echo -e "${STATUS_PREFIX_COLOR}${CIRCUIT_NAME}:${NC} add substrate guardring "
#RESULT_DIR_NAME="${RESULT_DIR}/${CIRCUIT_NAME}"
#RESULT_DIR_NAME="${RESULT_DIR_NAME//\//\\\/}"
#echo "RESULT_DIR_NAME = ${RESULT_DIR_NAME}"
#sed 's/CELL_NAME_PLACEHOLDER/'${CIRCUIT_NAME}'/g' ${CUR_SCRIPT_DIR}/../skill/addGuardRing.il.template > ${RESULT_DIR}/${CIRCUIT_NAME}/addGuardRing.il
#sed -i 's/LIB_NAME_PLACEHOLDER/'${LIB_NAME}'/g' ${RESULT_DIR}/${CIRCUIT_NAME}/addGuardRing.il 
#sed -i 's/RESULT_DIR_PLACEHOLDER/'${RESULT_DIR_NAME}'/g' ${RESULT_DIR}/${CIRCUIT_NAME}/addGuardRing.il 
#echo -e "${STATUS_PREFIX_COLOR}${CIRCUIT_NAME}:${NC} run ${FILE_PATH_COLOR}${RESULT_DIR}/${CIRCUIT_NAME}/addGuardRing.il${NC} and dump log to ${FILE_PATH_COLOR}${RESULT_DIR}/${CIRCUIT_NAME}/addGuardRing.log${NC}"
##virtuoso -nograph -restore ${RESULT_DIR}/${CIRCUIT_NAME}/addGuardRing.il > ${RESULT_DIR}/${CIRCUIT_NAME}/addGuardRing.log
