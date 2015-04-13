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
# Builds a source tar of the gwagent project
#
#   GWAGENT_SDK_VERSION - version name to use in building the archive file
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


# create the directories needed
mkdir -p ${ARTIFACTS_DIR}
mkdir -p ${WORKING_DIR}

outputTarFileName=alljoyn-gwagent-$GWAGENT_SDK_VERSION-src.tar

pushd ${GWAGENT_SRC_DIR}
git archive --prefix=alljoyn-gwagent-$GWAGENT_SDK_VERSION-src/gateway/gwagent/ HEAD^{tree} -o ${WORKING_DIR}/$outputTarFileName

cd ../../core/alljoyn
git archive --prefix=alljoyn-gwagent-$GWAGENT_SDK_VERSION-src/core/alljoyn/ HEAD^{tree} build_core -o ${WORKING_DIR}/core.tar

cd ../../services/base
git archive --prefix=alljoyn-gwagent-$GWAGENT_SDK_VERSION-src/services/base/ HEAD^{tree} sample_apps -o ${WORKING_DIR}/services.tar

cd ../..

tar --concatenate --file=${WORKING_DIR}/$outputTarFileName ${WORKING_DIR}/core.tar
tar --concatenate --file=${WORKING_DIR}/$outputTarFileName ${WORKING_DIR}/services.tar

gzip ${WORKING_DIR}/$outputTarFileName

pushd ${WORKING_DIR}
md5sum $outputTarFileName.gz > md5sum.txt
popd


cp ${WORKING_DIR}/$outputTarFileName.gz ${ARTIFACTS_DIR}
cp ${WORKING_DIR}/md5sum.txt ${ARTIFACTS_DIR}
