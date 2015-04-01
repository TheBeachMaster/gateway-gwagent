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
# Builds an archive of the Linux build for gwagent
#
#   BUILD_VARIANT - release or debug
#   GWAGENT_SDK_VERSION - version name to use in building the archive file
#   GWAGENT_SRC_DIR - root directory of the gwagent git repo
#   ARTIFACTS_DIR - directory to copy build products
#   WORKING_DIR - directory for working with files


set -o nounset
set -o errexit
set -o verbose
set -o xtrace


# check for required env variables
for var in BUILD_VARIANT GWAGENT_SDK_VERSION GWAGENT_SRC_DIR ARTIFACTS_DIR WORKING_DIR
do
    if [ -z "${!var:-}" ]
    then
        printf "$var must be defined!\n"
        exit 1
    fi
done


#========================================
# set variables for the different directories needed
sdkStaging=${WORKING_DIR}/sdk_stage
sdksDir=${ARTIFACTS_DIR}/sdks


# create the directories needed
mkdir -p $sdkStaging
mkdir -p $sdksDir


# determine the variant string
case ${BUILD_VARIANT} in
    debug)
        variantString=dbg
        ;;
    release)
        variantString=rel
        ;;
esac


#========================================
# build the code

pushd ${GWAGENT_SRC_DIR}
scons V=1 OS=linux CPU=x86_64 BINDINGS=cpp BR=off VARIANT=${BUILD_VARIANT} WS=check POLICYDB=on
popd


#========================================
# copy build products to staging directories

# create directory structure
mkdir -p $sdkStaging/alljoyn-daemon.d/apps
mkdir -p $sdkStaging/apps
mkdir -p $sdkStaging/app-manager
mkdir -p $sdkStaging/gw-mgmt/lib
mkdir -p $sdkStaging/daemon


distDir=${GWAGENT_SRC_DIR}/build/linux/x86_64/${BUILD_VARIANT}/dist

cp $distDir/gatewayMgmtApp/bin/* $sdkStaging/gw-mgmt
cp $distDir/gatewayConnector/tar/lib/* $sdkStaging/gw-mgmt/lib
cp $distDir/gatewayMgmtApp/bin/installPackage.sh $sdkStaging/app-manager
chmod u+x $sdkStaging/app-manager/installPackage.sh
cp $distDir/gatewayConnector/tar/lib/* $sdkStaging/gw-mgmt/lib
cp $distDir/cpp/bin/alljoyn-daemon $sdkStaging/daemon/
cp $distDir/cpp/lib/* $sdkStaging/daemon/

cp ${GWAGENT_SRC_DIR}/cpp/GatewayMgmtApp/defaultConfig.xml $sdkStaging/alljoyn-daemon.d/
cp ${GWAGENT_SRC_DIR}/cpp/GatewayMgmtApp/samples/config.xml $sdkStaging/daemon/

# package sample connector
pushd $distDir/gatewayConnector/tar
tar zcvf $sdkStaging/app-manager/dummyApp.tar.gz *
popd

#========================================
# create the SDK package

# copy ReleaseNotes.txt
cp ${GWAGENT_SRC_DIR}/ReleaseNotes.txt $sdkStaging/

# copy README.md
cp ${GWAGENT_SRC_DIR}/README.md $sdkStaging/

# create Manifest.txt file
echo "gateway/gwagent: $(git rev-parse --abbrev-ref HEAD) $(git rev-parse HEAD)" > $sdkStaging/Manifest.txt

sdkName=alljoyn-gwagent-${GWAGENT_SDK_VERSION}-linux-sdk-$variantString
tarFile=$sdkName.tar.gz

pushd $sdkStaging
tar zcvf $sdksDir/$tarFile *
popd

pushd $sdksDir
md5File=$sdksDir/md5-$sdkName.txt
rm -f $md5File
md5sum $tarFile > $md5File
popd


