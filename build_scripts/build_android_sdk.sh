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
#   CORE_VERSION - version of the Core Android SDK to use
#   SERVICES_VERSION - version of the Services Android SDKs to use
#   BUILD_VARIANT - release or debug
#   DEPENDENCIES_DIR - directory containing dependencies needed to build
#   GWAGENT_SDK_VERSION - version name to use in building the SDK
#   GWAGENT_SRC_DIR - root directory of the gwagent git repo
#   ARTIFACTS_DIR - directory to copy build products
#   WORKING_DIR - directory for working with files
#   ANDROID_SDK - Android SDK


set -o nounset
set -o errexit
set -o verbose
set -o xtrace

# check for required env variables
for var in CORE_VERSION SERVICES_VERSION BUILD_VARIANT DEPENDENCIES_DIR GWAGENT_SDK_VERSION GWAGENT_SRC_DIR ARTIFACTS_DIR WORKING_DIR ANDROID_SDK
do
    if [ -z "${!var:-}" ]
    then
        printf "$var must be defined!\n"
        exit 1
    fi
done


#========================================
# set variables for different directories needed
extractedSdks=${WORKING_DIR}/unzipped_sdks
jarsDepends=${WORKING_DIR}/jars
libsDepends=${WORKING_DIR}/libs
sdkStaging=${WORKING_DIR}/sdk_stage
docStaging=${WORKING_DIR}/doc_stage
sdksDir=${ARTIFACTS_DIR}/sdks

# create the directories needed
mkdir -p $jarsDepends
mkdir -p $libsDepends
mkdir -p $extractedSdks
mkdir -p $sdkStaging
mkdir -p $sdksDir


#========================================
# retrieve dependent jars/libs for the CORE_VERSION and SERVICES_VERSION

# determine the variant string
case ${BUILD_VARIANT} in
    debug)
        variantString=dbg
        ;;
    release)
        variantString=rel
        ;;
esac

# get alljoyn_apps_android_utils.jar
configSdkName=alljoyn-config-service-framework-${SERVICES_VERSION}-android-sdk-rel
configSdkContent=$extractedSdks/$configSdkName
mkdir -p $configSdkContent
unzip ${DEPENDENCIES_DIR}/$configSdkName.zip -d $configSdkContent
cp $configSdkContent/alljoyn-android/services/alljoyn-config-${SERVICES_VERSION}-rel/samples/ConfigClientSample/libs/alljoyn_apps_android_utils.jar $jarsDepends/

# get android-support-v4.jar
cp ${ANDROID_SDK}/extras/android/support/v4/android-support-v4.jar $jarsDepends/

# get android-support-v13.jar
cp ${ANDROID_SDK}/extras/android/support/v13/android-support-v13.jar $jarsDepends/

# get alljoyn jars/so
coreSdkName=alljoyn-${CORE_VERSION}-android-sdk-${variantString}
coreSdkContent=$extractedSdks/$coreSdkName
mkdir -p $coreSdkContent
unzip ${DEPENDENCIES_DIR}/$coreSdkName.zip -d $coreSdkContent
javaDir=alljoyn-android/core/alljoyn-${CORE_VERSION}-${variantString}/java
javaContentDir=$coreSdkContent/$javaDir
cp $javaContentDir/jar/alljoyn.jar $jarsDepends/
cp $javaContentDir/jar/alljoyn_about.jar $jarsDepends/
cp $javaContentDir/lib/liballjoyn_java.so $libsDepends/


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

sdkName=alljoyn-gwagent-${GWAGENT_SDK_VERSION}-android-sdk-$variantString
zipFile=$sdkName.zip


# create directory path for gwagent SDK
gwagentSdkDir=$sdkStaging/alljoyn-android/gwagent/alljoyn-gwagent-${GWAGENT_SDK_VERSION}-$variantString
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
    docName=alljoyn-gwagent-${GWAGENT_SDK_VERSION}-android-sdk-docs
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

