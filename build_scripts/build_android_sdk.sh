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
# Builds the Android SDK for the gateway agent
#
#   ANDROID_SDK - Android SDK (required to be defined)
#   GWAGENT_SRC_DIR - root directory of the gwagent git repo (defaults to relative location if not given)
#   BUILD_VARIANT - release or debug (defaults to release if not given)
#   GWAGENT_SDK_VERSION - version name to use in building the SDK (version number left out if not given)
#   LIBRARIES_DIR - directory containing dependent jars/shared objects
#                   (either LIBRARIES_DIR or DEPENDENCIES_DIR must be given)
#   DEPENDENCIES_DIR - directory containing the SDK dependencies needed to build
#                      (either LIBRARIES_DIR or DEPENDENCIES_DIR must be given)
#   ARTIFACTS_DIR - directory to copy build products (defaults to build/jobs/artifacts)
#   WORKING_DIR - directory for working with files (default to build/jobs/tmp)

set -o nounset
set -o errexit
set -o verbose
set -o xtrace

#========================================
# Set default values for any unset environment variables

export BUILD_VARIANT=${BUILD_VARIANT:-release}

if [ -z "${GWAGENT_SRC_DIR:-}" ]
then
    # set it to the top level directory for the git repo
    # (based on relative position of the build_scripts)
    export GWAGENT_SRC_DIR=$(dirname $(dirname $(readlink -f $0)))
fi

export ARTIFACTS_DIR=${ARTIFACTS_DIR:-$GWAGENT_SRC_DIR/build/jobs/artifacts}
export WORKING_DIR=${WORKING_DIR:-$GWAGENT_SRC_DIR/build/jobs/tmp}

#========================================

if [ -z "${LIBRARIES_DIR:-}" ] && [ -z "${DEPENDENCIES_DIR:-}" ]
then
    printf "DEPENDENCIES_DIR or LIBRARIES_DIR must be defined!\n"
    exit 1
fi

# check for required env variables
for var in ANDROID_SDK
do
    if [ -z "${!var:-}" ]
    then
        printf "$var must be defined!\n"
        exit 1
    fi
done


#========================================
# set variables for different directories needed
jarsDepends=${WORKING_DIR}/jars
libsDepends=${WORKING_DIR}/libs
sdkStaging=${WORKING_DIR}/sdk_stage
docStaging=${WORKING_DIR}/doc_stage
sdksDir=${ARTIFACTS_DIR}/sdks

# create the directories needed
mkdir -p $jarsDepends
mkdir -p $libsDepends
mkdir -p $sdkStaging
mkdir -p $sdksDir


#========================================
# retrieve dependent jars/libs for the core and services

# determine the variant string
case ${BUILD_VARIANT} in
    debug)
        variantString=dbg
        ;;
    release)
        variantString=rel
        ;;
esac

# get jars and shared objects from SDKs
if [ -n "${DEPENDENCIES_DIR:-}" ]
then

    extractedSdks=${WORKING_DIR}/unzipped_sdks
    mkdir -p $extractedSdks


    # get alljoyn_apps_android_utils.jar from config SDK
    configSdkNameMatch=alljoyn-config-service-framework-*-android-sdk-rel
    configSdkName=alljoyn-config-service-framework-android-sdk-rel
    configSdkContent=$extractedSdks/$configSdkName
    mkdir -p $configSdkContent
    unzip ${DEPENDENCIES_DIR}/$configSdkNameMatch.zip -d $configSdkContent
    cp $configSdkContent/alljoyn-android/services/alljoyn-config-*-rel/samples/ConfigClientSample/libs/alljoyn_apps_android_utils.jar $jarsDepends/

    # get alljoyn jars/so from the AllJoyn SDK
    coreSdkNameMatch=alljoyn-[^a-z]*-android-sdk-${variantString}
    coreSdkName=alljoyn-android-sdk-${variantString}
    coreSdkContent=$extractedSdks/$coreSdkName
    mkdir -p $coreSdkContent
    unzip ${DEPENDENCIES_DIR}/$coreSdkNameMatch.zip -d $coreSdkContent
    javaDir=alljoyn-android/core/alljoyn-*-${variantString}/java
    javaContentDir=$coreSdkContent/$javaDir
    cp $javaContentDir/jar/alljoyn.jar $jarsDepends/
    cp $javaContentDir/jar/alljoyn_about.jar $jarsDepends/
    cp $javaContentDir/lib/liballjoyn_java.so $libsDepends/
fi

# get android-support-v4.jar
cp ${ANDROID_SDK}/extras/android/support/v4/android-support-v4.jar $jarsDepends/

# get android-support-v13.jar
cp ${ANDROID_SDK}/extras/android/support/v13/android-support-v13.jar $jarsDepends/

# get the necessary jars/shared objects from the LIBRARIES_DIR (if set)
if [ -n "${LIBRARIES_DIR:-}" ]
then
    cp ${LIBRARIES_DIR}/*.jar $jarsDepends/
    cp ${LIBRARIES_DIR}/*.so $libsDepends/
fi

#========================================
# build the SDK and generate the javadocs

controllerDir=${GWAGENT_SRC_DIR}/java/GatewayController
jarDir=$controllerDir/libs
soDir=$controllerDir/libs/armeabi

# copy dependent jars
mkdir -p $jarDir
cp $jarsDepends/* $jarDir/

# copy alljoyn shared object
mkdir -p $soDir
cp $libsDepends/* $soDir/

# save artifact of sample project code
mkdir -p ${ARTIFACTS_DIR}/samples
cp -r $controllerDir ${ARTIFACTS_DIR}/samples/

# remove the android support jars from the sample project code
find ${ARTIFACTS_DIR}/samples -name "android-support*.jar" -exec rm -f {} \;

# build the project
pushd $controllerDir
ant -Dsdk.dir=${ANDROID_SDK}

# build the javadoc
ant -Dsdk.dir=${ANDROID_SDK} jdoc
popd


#========================================
# copy build products to artifacts directory

# save artifacts from build
cp $controllerDir/build/deploy/GatewayController.jar ${ARTIFACTS_DIR}
cp $controllerDir/bin/GatewayController.apk ${ARTIFACTS_DIR}

docArtifacts=${ARTIFACTS_DIR}/docs
mkdir -p $docArtifacts
cp -r $controllerDir/docs/* $docArtifacts


#========================================
# create the SDK package

versionString=""
if [ -n "${GWAGENT_SDK_VERSION:-}" ]
then
    versionString="${GWAGENT_SDK_VERSION}-"
fi

sdkName=alljoyn-gwagent-${versionString}android-sdk-$variantString
zipFile=$sdkName.zip


# create directory path for gwagent SDK
gwagentSdkDir=$sdkStaging/alljoyn-android/gwagent/alljoyn-gwagent-${versionString}$variantString
mkdir -p $gwagentSdkDir

# create directory structure
mkdir -p $gwagentSdkDir/docs
mkdir -p $gwagentSdkDir/samples
mkdir -p $gwagentSdkDir/tools
mkdir -p $gwagentSdkDir/java/libs

# copy ReleaseNotes.txt
cp ${GWAGENT_SRC_DIR}/ReleaseNotes.txt $gwagentSdkDir/

# copy README.md
cp ${GWAGENT_SRC_DIR}/README.md $gwagentSdkDir/

# create Manifest.txt file
pushd ${GWAGENT_SRC_DIR}
python ${GWAGENT_SRC_DIR}/build_scripts/genversion.py > $gwagentSdkDir/Manifest.txt
popd

# copy docs
cp -r ${docArtifacts}/* $gwagentSdkDir/docs/

# copy libs
cp ${ARTIFACTS_DIR}/GatewayController.jar $gwagentSdkDir/java/libs/

# copy apk
cp  ${ARTIFACTS_DIR}/GatewayController.apk $gwagentSdkDir/tools/

# copy samples
cp -r ${ARTIFACTS_DIR}/samples/* $gwagentSdkDir/samples/

# build the SDK zip
mkdir -p $sdksDir

pushd $sdkStaging
zip -q -r $sdksDir/$zipFile *
popd


# build a zip of the Javadoc on release builds
docZipFile=
if [ "$BUILD_VARIANT" == "release" ]
then
    docName=alljoyn-gwagent-${versionString}android-sdk-docs
    docZipDir=$docStaging/$docName
    mkdir -p $docZipDir
    cp -r $docArtifacts/* $docZipDir/

    docZipFile=$docName.zip
    pushd $docStaging
    zip -q -r $sdksDir/$docZipFile *
    popd
fi

# generate md5s
pushd $sdksDir
md5File=$sdksDir/md5-$sdkName.txt
rm -f $md5File
md5sum $zipFile > $md5File
[[ -z "$docZipFile" ]] || md5sum $docZipFile >> $md5File
popd

