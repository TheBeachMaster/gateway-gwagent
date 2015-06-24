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
# Builds an archive file of the Linux build for gwagent
#
#   GWAGENT_SDK_VERSION - version name ot use in building the archive file (version number left out if not given)
#   GWAGENT_SRC_DIR - root directory of the gwagent git repo (defaults to relative location if not given)
#   ARTIFACTS_DIR - directory to copy build products (defaults to build/jobs/artifacts)
#   WORKING_DIR - directory for working with files (defaults to build/jobs/tmp)


set -o nounset
set -o errexit
set -o verbose
set -o xtrace


#========================================
# Set default values for any unset environment variables

if [ -z "${GWAGENT_SRC_DIR:-}" ]
then
    # set it to the top level directory for the git repo
    # (based on relative position of the build_scripts)
    export GWAGENT_SRC_DIR=$(dirname $(dirname $(readlink -f $0)))
fi

export ARTIFACTS_DIR=${ARTIFACTS_DIR:-$GWAGENT_SRC_DIR/build/jobs/artifacts}
export WORKING_DIR=${WORKING_DIR:-$GWAGENT_SRC_DIR/build/jobs/tmp}

versionString=""
if [ -n "${GWAGENT_SDK_VERSION:-}" ]
then
    versionString="${GWAGENT_SDK_VERSION}-"
fi

#========================================
# set variables for different directories needed
sdkStaging=${WORKING_DIR}/sdk_stage
sdksDir=${ARTIFACTS_DIR}/sdks

# create the directories needed
mkdir -p $sdkStaging
mkdir -p ${ARTIFACTS_DIR}
mkdir -p $sdksDir



#========================================
# generate the docs

md5File=$sdksDir/md5-alljoyn-gwagent-${versionString}docs.txt
rm -f $md5File

generateDocs() {
    docName=$1
    docSrc=$2

    pushd ${GWAGENT_SRC_DIR}
    scons V=1 BINDINGS=cpp DOCS=html VARIANT=release -u ${docName}_docs
    popd

    docArtifacts=$sdkStaging/$docName
    cp -r ${GWAGENT_SRC_DIR}/build/linux/x86_64/release/dist/$2/docs/* $docArtifacts

    # create Manifest.txt file
    pushd ${GWAGENT_SRC_DIR}
    python ${GWAGENT_SRC_DIR}/build_scripts/genversion.py > $docArtifacts/Manifest.txt
    popd

    # create the documentation package
    sdkName=alljoyn-gwagent-${versionString}$docName-docs
    tarFile=$sdksDir/$sdkName.tar.gz

    pushd $docArtifacts
    tar zcvf $tarFile * --exclude=SConscript
    popd

    pushd $sdksDir
    md5sum $sdkName.tar.gz >> $md5File
    popd
}



generateDocs gwma gatewayMgmtApp
generateDocs gwcnc gatewayConnector
generateDocs gwc gatewayController

iosDocsZip=alljoyn-gwagent-${versionString}ios-sdk-docs.zip
androidDocsZip=alljoyn-gwagent-${versionString}android-sdk-docs.zip

pushd $sdksDir
[[ ! -f $iosDocsZip ]] || md5sum $iosDocsZip >> $md5File
[[ ! -f $androidDocsZip ]] || md5sum $androidDocsZip >> $md5File
popd

