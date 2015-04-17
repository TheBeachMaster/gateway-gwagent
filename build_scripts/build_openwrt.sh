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
# Builds OpenWRT ipks for gwagent
#
#   OPENWRT_REPO_URL - OpenWRT git repo URL to use
#                    - ex) git://git.openwrt.org/14.07/openwrt.git
#                    - ex) git://git.openwrt.org/12.09/openwrt.git
#   OPENWRT_ROOT_DIR - directory to clone OpenWRT git repo to
#                    - ex) ${WORKSPACE}/openwrt
#   OPENWRT_FEED_SRC_DIR - path to the AllJoyn OpenWRT feed src dir
#                    - ex) ${WORKSPACE}/allseen/core/openwrt_feed
#   ARTIFACTS_DIR - directory to copy build products
#                    - ex) ${WORKSPACE}/artifacts

set -o nounset
set -o errexit
set -o verbose
set -o xtrace

# check for required env variables
for var in OPENWRT_REPO_URL OPENWRT_ROOT_DIR OPENWRT_FEED_SRC_DIR ARTIFACTS_DIR
do
    if [ -z "${!var:-}" ]
    then
        printf "$var must be defined!\n"
        exit 1
    fi
done

# OpenWRT vars
OPENWRT_FEEDS_CONF=$OPENWRT_ROOT_DIR/feeds.conf
OPENWRT_CONFIG=$OPENWRT_ROOT_DIR/.config

# Start build routine
if ! [ -d "$OPENWRT_ROOT_DIR" ]; then
    git clone $OPENWRT_REPO_URL $OPENWRT_ROOT_DIR
fi

# Clean up from previous build
rm -f $OPENWRT_CONFIG
rm -rf $OPENWRT_ROOT_DIR/dl/*alljoyn*

mkdir -p ${OPENWRT_ROOT_DIR}/dl/
cp ${SRCTARS_DIR}/* ${OPENWRT_ROOT_DIR}/dl/

pushd $OPENWRT_ROOT_DIR

# Add basic feeds to feeds.conf
grep "src-git packages" $OPENWRT_FEEDS_CONF.default > $OPENWRT_FEEDS_CONF
grep "src-git luci" $OPENWRT_FEEDS_CONF.default >> $OPENWRT_FEEDS_CONF

# Add AllJoyn feed to feeds.conf
echo "src-git alljoyn file://$OPENWRT_FEED_SRC_DIR" >> $OPENWRT_FEEDS_CONF

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

make V=s tools/install
make V=s toolchain/install
make V=s package/feeds/alljoyn/alljoyn/{clean,compile,install}
make V=s package/feeds/alljoyn/alljoyn-services_common/{clean,compile,install}
make V=s package/feeds/alljoyn/alljoyn-config/{clean,compile,install}
make V=s package/feeds/alljoyn/alljoyn-notification/{clean,compile,install}
make V=s package/feeds/alljoyn/alljoyn-gwagent/{clean,compile,install}

# set variables for the different directories needed
ipksDir=${ARTIFACTS_DIR}/ipks

# create the directory if needed
mkdir -p $ipksDir

# copy ipks
find . -name alljoyn*.ipk | xargs -I{} cp {} $ipksDir

# package ipks
pushd $ipksDir
tar zcvf $ARTIFACTS_DIR/gwagent_ipks.tar.gz --owner=0 --group=0 *.ipk
popd

popd
