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
# Build the iOS SDK for the gateway agent
#
#   CORE_VERSION - version of the Core Android SDK to use
#   SERVICES_VERSION - version of the Services Android SDK to use
#   BUILD_VARIANT - release or debug
#   DEPENDENCIES_DIR - directory containing dependencies needed to build
#   GWAGENT_SDK_VERSION - version name to use in building the SDK
#   GWAGENT_SRC_DIR - root directory of gwagent git repo
#   ARTIFACTS_DIR - directory to copy build products
#   WORKING_DIR - directory for working with files


set -o nounset
set -o errexit
set -o verbose
set -o xtrace

# check for required env variables
for var in BUILD_VARIANT CORE_VERSION SERVICES_VERSION GWAGENT_SDK_VERSION DEPENDENCIES_DIR GWAGENT_SRC_DIR ARTIFACTS_DIR WORKING_DIR
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
sdkStaging=${WORKING_DIR}/sdk_stage
buildDir=${WORKING_DIR}/build
sdksDir=${ARTIFACTS_DIR}/sdks

# create the directories needed
mkdir -p $extractedSdks
mkdir -p $sdkStaging
mkdir -p $buildDir
mkdir -p $sdksDir


#========================================
# retrieve dependent jars/libs for the CORE_VERSION and SERVICES_VERSION

# determine the variant string
case ${BUILD_VARIANT} in
    debug)
        variantString=dbg
        buildVariantString=Debug
        ;;
    release)
        variantString=rel
        buildVariantString=Release
        ;;
esac


# unzip the core SDK
coreSdkName=alljoyn-${CORE_VERSION}-osx_ios-sdk
unzip ${DEPENDENCIES_DIR}/$coreSdkName.zip -d $extractedSdks
coreSdkContent=$extractedSdks/$coreSdkName

coreBuildDir=$buildDir/core/alljoyn
gwagentBuildDir=$buildDir/gateway/gwagent

mkdir -p $coreBuildDir/build/darwin
cp -r $coreSdkContent/alljoyn_objc $coreBuildDir/
cp -r $coreSdkContent/build/darwin/* $coreBuildDir/build/darwin/
cp -r $coreSdkContent/services $coreBuildDir/

mkdir -p $gwagentBuildDir

#========================================
# build the SDK and docs

export ALLJOYN_SDK_ROOT=$coreBuildDir
export ALLSEEN_BASE_SERVICES_ROOT=${GWAGENT_SRC_DIR}
export OPENSSL_ROOT=/usr/local/openssl-1.0.1e

libArtifacts=${ARTIFACTS_DIR}/lib
mkdir $libArtifacts

for language in objc cpp
do
    # build product directory
    proj_dir=${GWAGENT_SRC_DIR}/ios/GatewayController/samples/alljoyn_services_$language
    chmod -R 777 $proj_dir

    cd $proj_dir

    for sdk in "iphonesimulator7.1 -arch i386" iphoneos
    do
        xcodebuild -project $proj_dir/alljoyn_gateway_$language.xcodeproj -sdk $sdk -configuration $buildVariantString CONFIGURATION_BUILD_DIR=$gwagentBuildDir

        archiveFile=liballjoyn_gateway_${language}.a
        renamedArchive=${archiveFile}_`echo $sdk | awk '{print $1}'`

        cp $gwagentBuildDir/$archiveFile $gwagentBuildDir/$renamedArchive
        cp $gwagentBuildDir/$renamedArchive $libArtifacts/$renamedArchive

    done

    # create multi architecture library
    lipo -create $libArtifacts/liballjoyn_gateway_${language}.a_* -output $libArtifacts/liballjoyn_gateway_${language}.a
    lipo -create $gwagentBuildDir/liballjoyn_gateway_${language}.a_* -output $coreBuildDir/build/liballjoyn_gateway_${language}.a

    # generate documentation
    if [ $language = objc ]
    then

        doc_builddir=$gwagentBuildDir/docs/gateway/$language
        mkdir -p $doc_builddir
        chmod -R 777 $doc_builddir

        #doc_status=0
        (appledoc --keep-intermediate-files $proj_dir \
            --output $doc_builddir \
            --project-name "GatewayControllerService" \
            --project-company "AllSeen Alliance" --company-id "org.allseen" \
            --no-install-docset --no-publish-docset \
            --include ${GWAGENT_SRC_DIR}/ios/GatewayController/inc/alljoyn/gateway)

        gwagentDocsDir=${ARTIFACTS_DIR}/docs/gateway
        mkdir -p $gwagentDocsDir
        tar cf - -C $doc_builddir docset html | tar xf - -C $gwagentDocsDir

    fi

    # save header files for packaging
    case $language in
        cpp)  lang=cpp ;;
        objc) lang=ios ;;
    esac

    includeDir=${ARTIFACTS_DIR}/include/$language
    mkdir -p $includeDir
    tar cf - --exclude .gitignore --exclude '*.mm' -C ${GWAGENT_SRC_DIR}/$lang/GatewayController inc | \
        tar xf - -C $includeDir

done


# docs separately
tar czf docs-gwagent.tgz -C $gwagentBuildDir/ docs
mv docs-gwagent.tgz ${ARTIFACTS_DIR}/


#========================================
# create the SDK package

sdkName=alljoyn-gwagent-${GWAGENT_SDK_VERSION}-ios-sdk-$variantString
zipFile=$sdkName.zip

# create directory path for gwagent SDK
gwagentSdkDir=$sdkStaging/alljoyn-ios/gwagent/alljoyn-gwagent-${GWAGENT_SDK_VERSION}-$variantString
mkdir -p $gwagentSdkDir

# create directory structure
mkdir -p $gwagentSdkDir/cpp/inc
mkdir -p $gwagentSdkDir/cpp/lib
mkdir -p $gwagentSdkDir/docs/gateway
mkdir -p $gwagentSdkDir/objc/inc
mkdir -p $gwagentSdkDir/objc/lib
mkdir -p $gwagentSdkDir/objc/samples

# copy ReleaseNotes.txt
cp ${GWAGENT_SRC_DIR}/ReleaseNotes.txt $gwagentSdkDir/

# copy README.md
cp ${GWAGENT_SRC_DIR}/README.md $gwagentSdkDir/

# create Manifest.txt file
echo "gateway/gwagent: $(git rev-parse --abbrev-ref HEAD) $(git rev-parse HEAD)" > $gwagentSdkDir/Manifest.txt

# copy docs
cp -r ${gwagentDocsDir}/* $gwagentSdkDir/docs/gateway/

# copy libs
cp ${ARTIFACTS_DIR}/lib/liballjoyn_gateway_cpp.a $gwagentSdkDir/cpp/lib/
cp ${ARTIFACTS_DIR}/lib/liballjoyn_gateway_objc.a $gwagentSdkDir/objc/lib/

# copy inc directories
cp -r ${GWAGENT_SRC_DIR}/ios/GatewayController/inc/* $gwagentSdkDir/objc/inc
cp -r ${GWAGENT_SRC_DIR}/cpp/GatewayController/inc/* $gwagentSdkDir/cpp/inc

# copy samples
cp -r ${GWAGENT_SRC_DIR}/ios/GatewayController/samples/sampleApp $gwagentSdkDir/objc/samples/sampleApp


# build the SDK zip
mkdir -p $sdksDir

pushd $sdkStaging
zip -q -r $sdksDir/$zipFile alljoyn-ios
popd

pushd $sdksDir
md5File=$sdksDir/md5-$sdkName.txt
rm -f $md5File
md5 $zipFile > $md5File
popd

