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
#   GWAGENT_SDK_VERSION - version name ot use in buildig the archive file
#   GWAGENT_SRC_DIR - root directory of the gwagent git repo
#   ARTIFACTS_DIR - directory to copy build products
#   WORKING_DIR - directory for working with files


set -o nounset
set -o errexit
set -o verbose
set -o xtrace


# check for required env variables
for var in GWAGENT_SDK_VERSION GWAGENT_SRC_DIR ARTIFACTS_DIR WORKING_DIR
do
    if [ -z "${!var:-}" ]
    then
        printf "$var must be defined!\n"
        exit 1
    fi
done


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

pushd ${GWAGENT_SRC_DIR}
scons V=1 BINDINGS=cpp DOCS=html -u gwma_docs
popd


#========================================
# create the documentation package

docArtifacts=$sdkStaging
cp -r ${GWAGENT_SRC_DIR}/cpp/GatewayMgmtApp/docs/* $docArtifacts

# create Manifest.txt file
echo "gateway/gwagent: $(git rev-parse --abbrev-ref HEAD) $(git rev-parse HEAD)" > $sdkStaging/Manifest.txt

sdkName=alljoyn-gwagent-${GWAGENT_SDK_VERSION}-docs

tarFile=$sdksDir/$sdkName.tar.gz

pushd $sdkStaging
tar zcvf $tarFile * --exclude=SConscript
popd

pushd $sdksDir
md5File=$sdksDir/md5-$sdkName.txt
rm -f $md5File
md5sum $sdkName.tar.gz > $md5File
popd

