#!/bin/bash

# Copyright AllSeen Alliance. All rights reserved.
#
#    Permission to use, copy, modify, and/or distribute this software for any
#    purpose with or without fee is hereby granted, provided that the above
#    copyright notice and this permission notice appear in all copies.
#
#    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
#    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
#    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
#    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
#    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
#    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
#    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
#
# Builds OpenWRT ipks needed to install the gwagent and sample connector
#
#   BUILD_VARIANT - release or debug (default to release if not given)
#   OPENWRT_REPO_URL - OpenWRT git repo URL to use
#                    - defaults to git://git.openwrt.org/14.07/openwrt.git
#                    - ex) git://git.openwrt.org/14.07/openwrt.git
#                    - ex) git://git.openwrt.org/12.09/openwrt.git
#   OPENWRT_ROOT_DIR - directory to clone OpenWRT git repo to
#                    - defaults to build/jobs/openwrt_root
#                    - ex) ${WORKSPACE}/openwrt
#   OPENWRT_FEED_SRC - path to the AllJoyn OpenWRT feed src dir
#                    - defaults to https://git.allseenalliance.org/gerrit/core/openwrt_feed.git
#                    - ex) file://${WORKSPACE}/allseen/core/openwrt_feed
#                    - ex) https://git.allseenalliance.org/gerrit/core/openwrt_feed.git
#   OPENWRT_DL_DIR - directory for storing packages downloaded (optional)
#   ARTIFACTS_DIR - directory to copy build products (defaults to build/jobs/artifacts)
#   WORKING_DIR - temporary directory for use by this script (defaults to build/jobs/tmp)
#   CORE_SRCTAR - file containing the core source for building (optional)
#                 if set the file will be extracted and CORE_SOURCE_DIR set to the extracted source
#   SERVICES_SRCTAR - file containing the services source for building (optional)
#                 if set the file will be extracted and SERVICES_SOURCE_DIR set to the extracted source
#   GATEWAY_SRCTAR - file containing the gateway source for building (optional)
#                 if set the file will be extracted and GATEWAY_SOURCE_DIR set to the extracted source
#   USE_SOURCE_DIR_OPTION - if set to y then CORE_SOURCE_DIR, SERVICES_SOURCE_DIR, and GATEWAY_SOURCE_DIR
#                           will default to relative directories
#   CORE_SOURCE_DIR - directory contains the core source for building (optional)
#                     if USE_SOURCE_DIR_OPTION=y then it defaults to ../../../core/alljoyn
#   SERVICES_SOURCE_DIR - directory contains the services source for building (optional)
#                     if USE_SOURCE_DIR_OPTION=y then it defaults to ../../..
#   GATEWAY_SOURCE_DIR - direcory contains the gateway source for building (optional)
#                     if USE_SOURCE_DIR_OPTION=y then it defaults to ../../..
#
# Some ways to use this build script to build the OpenWrt ipks
#   1. Build the ipks using the source as specified from the feed
#      - this is the default build when none of the environment variables mentioned below are set
#   2. Provide the source tar files for one of more of the packages (core, services, or gateway)
#      - CORE_SRCTAR - source tar file for building core packages
#      - SERVICES_SRCTAR - source tar file for building services packages
#      - GATEWAY_SRCTAR - source tar file for building the gateway packages
#   3. Use local directories for the sources (assumes all git projects are peers on local file system
#      and the directory names match the project names)
#      - USE_SOURCE_DIR_OPTION="y"

set -o nounset
set -o errexit
set -o verbose
set -o xtrace

#========================================
# Set default values for any unset environment variables

export BUILD_VARIANT=${BUILD_VARIANT:-release}

export OPENWRT_REPO_URL=${OPENWRT_REPO_URL:-git://git.openwrt.org/14.07/openwrt.git}

if [ -z "${GWAGENT_SRC_DIR:-}" ]
then
    # set it to the top level directory for the git repo
    # (based on relative position of the build_scripts)
    export GWAGENT_SRC_DIR=$(dirname $(dirname $(readlink -f $0)))
fi

export OPENWRT_ROOT_DIR=${OPENWRT_ROOT_DIR:-$GWAGENT_SRC_DIR/build/jobs/openwrt_root}
export WORKING_DIR=${WORKING_DIR:-$GWAGENT_SRC_DIR/build/jobs/tmp}
export ARTIFACTS_DIR=${ARTIFACTS_DIR:-$GWAGENT_SRC_DIR/build/jobs/artifacts}
export OPENWRT_FEED_SRC=${OPENWRT_FEED_SRC:-https://git.allseenalliance.org/gerrit/core/openwrt_feed.git}

#========================================
# extract source tar (if given)
#   CORE_SRCTAR - file containing the core source for building (optional)
#   SERVICES_SRCTAR - file containing the services source for building (optional)
#   GATEWAY_SRCTAR - file containing the gateway source for building (optional)

if [ -n "${CORE_SRCTAR:-}" ]
then
    CORE_SOURCE_DIR=${WORKING_DIR}/core_source
    rm -fr ${CORE_SOURCE_DIR}
    mkdir -p ${CORE_SOURCE_DIR}
    tar -zxf ${CORE_SRCTAR} -C ${CORE_SOURCE_DIR}
    CORE_SOURCE_DIR=$(readlink -f ${CORE_SOURCE_DIR}/*)
fi
if [ -n "${SERVICES_SRCTAR:-}" ]
then
    SERVICES_SOURCE_DIR=${WORKING_DIR}/services_source
    rm -fr ${SERVICES_SOURCE_DIR}
    mkdir -p ${SERVICES_SOURCE_DIR}
    tar -zxf ${SERVICES_SRCTAR} -C ${SERVICES_SOURCE_DIR}
    SERVICES_SOURCE_DIR=$(readlink -f ${SERVICES_SOURCE_DIR}/*)
fi
if [ -n "${GATEWAY_SRCTAR:-}" ]
then
    GATEWAY_SOURCE_DIR=${WORKING_DIR}/gateway_source
    rm -fr ${GATEWAY_SOURCE_DIR}
    mkdir -p ${GATEWAY_SOURCE_DIR}
    tar -zxf ${GATEWAY_SRCTAR} -C ${GATEWAY_SOURCE_DIR}
    GATEWAY_SOURCE_DIR=$(readlink -f ${GATEWAY_SOURCE_DIR}/*)
fi

#========================================
# if USE_SOURCE_DIR_OPTION is given then set the source directories

if [ "${USE_SOURCE_DIR_OPTION:-}" == "y" ]
then
    # use relative paths for the other projects
    rootGitDir=$(dirname $(dirname $GWAGENT_SRC_DIR))
    export CORE_SOURCE_DIR=${CORE_SOURCE_DIR:-$rootGitDir/core/alljoyn}
    export SERVICES_SOURCE_DIR=${SERVICES_SOURCE_DIR:-$rootGitDir}
    export GATEWAY_SOURCE_DIR=${GATEWAY_SOURCE_DIR:-$rootGitDir}
fi

#========================================

# OpenWRT vars
OPENWRT_FEEDS_CONF=$OPENWRT_ROOT_DIR/feeds.conf
OPENWRT_CONFIG=$OPENWRT_ROOT_DIR/.config

# Start build routine
if ! [ -d "$OPENWRT_ROOT_DIR" ]; then
    git clone $OPENWRT_REPO_URL $OPENWRT_ROOT_DIR
fi

# create link to DL directory (if provided)
if [ -n "${OPENWRT_DL_DIR:-}" ]
then
    [ -d "${OPENWRT_DL_DIR}" ] || mkdir -p ${OPENWRT_DL_DIR}
    if [ -d "${OPENWRT_ROOT_DIR}/dl" ]
    then
        rm -fr "${OPENWRT_ROOT_DIR}/dl"
    fi
    ln -s $OPENWRT_DL_DIR ${OPENWRT_ROOT_DIR}/dl
else
    mkdir -p ${OPENWRT_ROOT_DIR}/dl/
fi

# Clean up from previous build
rm -f $OPENWRT_CONFIG
rm -rf $OPENWRT_ROOT_DIR/dl/*alljoyn*

pushd $OPENWRT_ROOT_DIR

# Add basic feeds to feeds.conf
grep "src-git packages" $OPENWRT_FEEDS_CONF.default > $OPENWRT_FEEDS_CONF
grep "src-git luci" $OPENWRT_FEEDS_CONF.default >> $OPENWRT_FEEDS_CONF

# Add AllJoyn feed to feeds.conf
echo "src-git alljoyn $OPENWRT_FEED_SRC" >> $OPENWRT_FEEDS_CONF

./scripts/feeds list -r alljoyn | awk '{ print $1 }' | xargs -r -n 1 ./scripts/feeds uninstall
rm -rf $OPENWRT_ROOT_DIR/feeds/alljoyn*
./scripts/feeds update -a
./scripts/feeds install -a -p alljoyn -d m

make defconfig

AJ_CORE_PKG="CONFIG_PACKAGE_alljoyn"
AJ_ABOUT_PKG="CONFIG_PACKAGE_alljoyn-about"
AJ_C_PKG="CONFIG_PACKAGE_alljoyn-c"
AJ_CONFIG_PKG="CONFIG_PACKAGE_alljoyn-config"
AJ_CONTROLPANEL_PKG="CONFIG_PACKAGE_alljoyn-controlpanel"
AJ_NON_GW_CONFIG_PKG="CONFIG_PACKAGE_alljoyn-non-gw-config"
AJ_NOTIFICATION_PKG="CONFIG_PACKAGE_alljoyn-notification"
AJ_ONBOARDING_PKG="CONFIG_PACKAGE_alljoyn-onboarding"
AJ_SAMPLES_PKG="CONFIG_PACKAGE_alljoyn-samples"
AJ_SERVICES_COMMON_PKG="CONFIG_PACKAGE_alljoyn-services_common"
AJ_SERVICES_SAMPLE_APPS_PKG="CONFIG_PACKAGE_alljoyn-sample_apps"
AJ_CONFIG_SAMPLES_PKG="CONFIG_PACKAGE_alljoyn-config-samples"
AJ_CONTROLPANEL_SAMPLES_PKG="CONFIG_PACKAGE_alljoyn-controlpanel-samples"
AJ_NOTIFICATION_SAMPLES_PKG="CONFIG_PACKAGE_alljoyn-notification-samples"
AJ_ONBOARDING_SAMPLES_PKG="CONFIG_PACKAGE_alljoyn-onboarding-samples"
AJ_GWAGENT_PKG="CONFIG_PACKAGE_alljoyn-gwagent"
AJ_GWAGENT_SAMPLES_PKG="CONFIG_PACKAGE_alljoyn-gwagent-samples"

# Enable these AllJoyn packages
sed -i "s/.*$AJ_CORE_PKG is not set/$AJ_CORE_PKG\=m/g" $OPENWRT_CONFIG
sed -i "s/.*$AJ_CORE_PKG\=y/$AJ_CORE_PKG\=m/g" $OPENWRT_CONFIG
sed -i "s/.*$AJ_ABOUT_PKG is not set/$AJ_ABOUT_PKG\=m/g" $OPENWRT_CONFIG
sed -i "s/.*$AJ_ABOUT_PKG\=y/$AJ_ABOUT_PKG\=m/g" $OPENWRT_CONFIG
sed -i "s/.*$AJ_CONFIG_PKG is not set/$AJ_CONFIG_PKG\=m/g" $OPENWRT_CONFIG
sed -i "s/.*$AJ_CONFIG_PKG\=y/$AJ_CONFIG_PKG\=m/g" $OPENWRT_CONFIG
sed -i "s/.*$AJ_NOTIFICATION_PKG is not set/$AJ_NOTIFICATION_PKG\=m/g" $OPENWRT_CONFIG
sed -i "s/.*$AJ_NOTIFICATION_PKG\=y/$AJ_NOTIFICATION_PKG\=m/g" $OPENWRT_CONFIG
sed -i "s/.*$AJ_SERVICES_COMMON_PKG is not set/$AJ_SERVICES_COMMON_PKG\=m/g" $OPENWRT_CONFIG
sed -i "s/.*$AJ_SERVICES_COMMON_PKG\=y/$AJ_SERVICES_COMMON_PKG\=m/g" $OPENWRT_CONFIG
sed -i "s/.*$AJ_GWAGENT_PKG is not set/$AJ_GWAGENT_PKG\=m/g" $OPENWRT_CONFIG
sed -i "s/.*$AJ_GWAGENT_PKG\=y/$AJ_GWAGENT_PKG\=m/g" $OPENWRT_CONFIG
sed -i "s/.*$AJ_GWAGENT_SAMPLES_PKG is not set/$AJ_GWAGENT_SAMPLES_PKG\=m/g" $OPENWRT_CONFIG
sed -i "s/.*$AJ_GWAGENT_SAMPLES_PKG\=y/$AJ_GWAGENT_SAMPLES_PKG\=m/g" $OPENWRT_CONFIG

# Disable these AllJoyn packages
sed -i "s/.*$AJ_C_PKG\=.*$/# $AJ_C_PKG is not set/g" $OPENWRT_CONFIG
sed -i "s/.*$AJ_CONFIG_SAMPLES_PKG\=.*$/# $AJ_CONFIG_SAMPLES_PKG is not set/g" $OPENWRT_CONFIG
sed -i "s/.*$AJ_NOTIFICATION_SAMPLES_PKG\=.*$/# $AJ_NOTIFICATION_SAMPLES_PKG is not set/g" $OPENWRT_CONFIG
sed -i "s/.*$AJ_CONTROLPANEL_PKG\=.*$/# $AJ_CONTROLPANEL_PKG is not set/g" $OPENWRT_CONFIG
sed -i "s/.*$AJ_CONTROLPANEL_SAMPLES_PKG\=.*$/# $AJ_CONTROLPANEL_SAMPLES_PKG is not set/g" $OPENWRT_CONFIG
sed -i "s/.*$AJ_NON_GW_CONFIG_PKG\=.*$/# $AJ_NON_GW_CONFIG_PKG is not set/g" $OPENWRT_CONFIG
sed -i "s/.*$AJ_ONBOARDING_PKG\=.*$/# $AJ_ONBOARDING_PKG is not set/g" $OPENWRT_CONFIG
sed -i "s/.*$AJ_ONBOARDING_SAMPLES_PKG\=.*$/# $AJ_ONBOARDING_SAMPLES_PKG is not set/g" $OPENWRT_CONFIG
sed -i "s/.*$AJ_SAMPLES_PKG\=.*$/# $AJ_SAMPLES_PKG is not set/g" $OPENWRT_CONFIG
sed -i "s/.*$AJ_SERVICES_SAMPLE_APPS_PKG\=.*$/# $AJ_SERVICES_SAMPLE_APPS_PKG is not set/g" $OPENWRT_CONFIG

# enable CONFIG_DEBUG based on BUILD_VARIANT
if [ "${BUILD_VARIANT}" == "debug" ]
then
    sed -i "s/^.*CONFIG_DEBUG[ =].*$/CONFIG_DEBUG=y/g" $OPENWRT_CONFIG
else
    sed -i "s/^.*CONFIG_DEBUG[ =].*$/# CONFIG_DEBUG is not set/g" $OPENWRT_CONFIG
fi

make V=s tools/install
make V=s toolchain/install

make V=s package/zlib/{clean,compile,install}
make V=s package/openssl/{clean,compile,install}
make V=s package/libcap/{clean,compile,install}
make V=s package/libxml2/{clean,compile,install}

function makeSourceBuild()
{
    # we need to run prepare once without the USE_SOURCE_DIR so that the source
    # package gets downloaded or it will get downloaded later during the compile
    # phase and override our USE_SOURCE_DIR setting
    make V=s $1/clean
    make V=s $1/prepare
    make V=s $1/clean
    make V=s $1/prepare USE_SOURCE_DIR=$2
    make V=s $1/compile
}

if [ -z "${CORE_SOURCE_DIR:-}" ]
then
    make V=s package/feeds/alljoyn/alljoyn/{clean,compile,install}
else
    makeSourceBuild package/alljoyn $CORE_SOURCE_DIR
fi

if [ -z "${SERVICES_SOURCE_DIR:-}" ]
then
    make V=s package/feeds/alljoyn/alljoyn-services_common/{clean,compile,install}
    make V=s package/feeds/alljoyn/alljoyn-config/{clean,compile,install}
    make V=s package/feeds/alljoyn/alljoyn-notification/{clean,compile,install}
else
    makeSourceBuild package/alljoyn-services_common ${SERVICES_SOURCE_DIR}
    makeSourceBuild package/alljoyn-config ${SERVICES_SOURCE_DIR}
    makeSourceBuild package/alljoyn-notification ${SERVICES_SOURCE_DIR}
fi

if [ -z "${GATEWAY_SOURCE_DIR:-}" ]
then
    make V=s package/feeds/alljoyn/alljoyn-gwagent
else
    makeSourceBuild package/alljoyn-gwagent ${GATEWAY_SOURCE_DIR}
fi

# set variables for the different directories needed
ipksDir=${ARTIFACTS_DIR}/ipks

# create the directory if needed
mkdir -p $ipksDir

# copy ipks
find . -name alljoyn*.ipk | xargs -I{} cp {} $ipksDir

popd

# package ipks
pushd $ipksDir
tar zcvf $ARTIFACTS_DIR/gwagent_ipks.tar.gz --owner=0 --group=0 *.ipk
popd

