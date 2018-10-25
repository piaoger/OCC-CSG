#!/bin/bash

INIT_PROJECT=$1
RUN_TEST=$2

OS=$(uname)
THIS_FOLDER=$(cd ${0%/*} && echo $PWD)

APP_ROOT=$THIS_FOLDER
APP_BUILD_ROOT=$APP_ROOT/build

generate() {
        export OpenCASCADE_DIR=${APP_ROOT}/3rd/opencascade-v7.2.0.1-macos
    mkdir -p ${APP_BUILD_ROOT} && cd ${APP_BUILD_ROOT}
    cmake .. -DOpenCASCADE_DIR=${OpenCASCADE_DIR}
    make -j4
}

build() {

    cd ${APP_BUILD_ROOT}
    make
    export PATH=${APP_BUILD_ROOT}/bin:$PATH
}

run_test() {
    echo "run test ..."
    occ-csg --create box -5,-5,-5,5,5,5 ${APP_BUILD_ROOT}/box.stp
    occ-csg --create sphere 0,0,0,6.5 ${APP_BUILD_ROOT}/sphere.stp
    occ-csg --csg difference box.stp sphere.stp ${APP_BUILD_ROOT}/cut.obj
}

main() {
    if [ "VAR${INIT_PROJECT}" == "VARinit" ]; then
        generate
    fi

    build

    if [ "VAR${RUN_TEST}" == "VARtest" ]; then
        run_test
    fi
}

main