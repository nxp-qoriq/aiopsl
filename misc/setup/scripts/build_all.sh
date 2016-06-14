#!/usr/bin/env bash

# Root directory where you have aiopsl and aiop apps folder
export MY_ROOT_DIR=/cygdrive/y/git-repos
# file which will contain build logs for further inspection
export BUILD_LOG=/cygdrive/y/git-repos/aiopsl/build-all.log
# path to where CodeWarrior for APP is installed
export CW_PATH=/cygdrive/c/Freescale/CW4NET_v2015.12Beta1/CW_APP

# Path to AIOPSL and AIOP Apps
export APPS_PATH=aiopsl/build/generic/rev1
export AIOPSL_PATH=$APPS_PATH/aiopsl

# build of AIOP apps is optional.
BUILD_REFAPP=true
# choose what applications to build
aiop_apps=("classifier" "control_flow" "reflector" "header_manip" "statistics" "cmdif_test")
# Developers might want only generation of makefiles to manually issue make
GENERATE_MAKEFILE_ONLY=false

# build or generate makefiles for aiopsl project
cd $MY_ROOT_DIR || exit
echo "" >> ${BUILD_LOG}
if  [ "$GENERATE_MAKEFILE_ONLY" = true ] ; then
    echo ""
    echo "Generating makefiles for AIOPSL"
    echo ""
    $CW_PATH/eclipse/ecd.exe -generateMakefiles \
        -project $AIOPSL_PATH \
        -allConfigs
else
    echo ""
    echo "Building AIOPSL"
    echo ""
    $CW_PATH/eclipse/ecd.exe -build \
        -project $AIOPSL_PATH \
        -allConfigs \
        -cleanBuild | tee -a ${BUILD_LOG}
fi

# build aiop-refapp
if [ "$BUILD_REFAPP" = true ]; then
    for dir in "${aiop_apps[@]}"; do

        if [ "$dir" != "reflector" ]; then
            AIOP_CONFIG=("LS1088A" "LS2085A" "LS2088A")
        else
            AIOP_CONFIG=("LS1088A" "LS2085A" "LS2088A" "LS1088A_perf" "LS2085A_perf" "LS2088A_perf")
        fi

        cd $MY_ROOT_DIR || exit
        echo ""
        echo "Generating makefile for ${dir}"
        echo ""

        $CW_PATH/eclipse/ecd.exe -generateMakefiles \
            -project $APPS_PATH/"${dir}" -allConfigs
        if [ "$GENERATE_MAKEFILE_ONLY" = false ] ; then
            for config in "${AIOP_CONFIG[@]}"; do
                echo ""
                echo "Building ${dir} ${config}"
                echo ""

                cd ${MY_ROOT_DIR}/${APPS_PATH}/"${dir}"/"$config" && make | tee \
                    -a ${BUILD_LOG}
                cp ${MY_ROOT_DIR}/${APPS_PATH}/"${dir}"/"$config"/*.elf \
                    ${MY_ROOT_DIR}/${APPS_PATH}/../../../demos/images/"${config%%_*}"
            done
        fi
    done
fi
