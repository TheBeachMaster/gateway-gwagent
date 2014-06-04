#!/bin/bash
# Copyright (c) 2014, AllSeen Alliance. All rights reserved.
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

echo "Trying to install: $1"

if [ ! -d "/opt/alljoyn" ]; then
  mkdir "/opt/alljoyn" 2>/dev/null || { echo "Could not create alljoyn directory"; exit 1; }
fi

if [ ! -d "/opt/alljoyn/apps" ]; then
  mkdir "/opt/alljoyn/apps" 2>/dev/null || { echo "Could not create alljoyn/apps directory"; exit 2; }
fi

if [ ! -d "/opt/alljoyn/alljoyn-daemon.d" ]; then
  mkdir "/opt/alljoyn/alljoyn-daemon.d" 2>/dev/null || { echo "Could not create alljoyn/alljoyn-daemon.d directory"; exit 3; }
fi

if [ ! -d "/opt/alljoyn/alljoyn-daemon.d/apps" ]; then
  mkdir "/opt/alljoyn/alljoyn-daemon.d/apps" 2>/dev/null || { echo "Could not create alljoyn/alljoyn-daemon.d/apps directory"; exit 4; }
fi

if [ ! -d "/opt/alljoyn/app-manager" ]; then
  mkdir "/opt/alljoyn/app-manager" 2>/dev/null || { echo "Could not create alljoyn/app-manager directory"; exit 5; }
fi

uuid=$(cat /proc/sys/kernel/random/uuid)

if [ -d "/opt/alljoyn/app-manager/$uuid" ]; then
  echo "Uid $uuid directory already exists. Should not have happened"; exit 6; 
fi

mkdir "/opt/alljoyn/app-manager/$uuid" 2>/dev/null || { echo "Could not create alljoyn/app-manager/$uuid directory"; exit 7; }

tar -xzf $1 -C /opt/alljoyn/app-manager/$uuid

if [ ! -f "/opt/alljoyn/app-manager/$uuid/Manifest.xml" ]; then
  echo "Can not find Manifest file. Aborting"; exit 8; 
fi

if [ ! -d "/opt/alljoyn/app-manager/$uuid/bin" ]; then
  echo "Can not find bin directory. Aborting"; exit 9; 
fi

appId=$(grep "<appId>" /opt/alljoyn/app-manager/$uuid/Manifest.xml | sed -e "s/ *<appId>//" | sed -e "s/<\/appId *>//")

if [ -d "/opt/alljoyn/apps/$appId" ]; then
  echo "Can not create directory for this app. It already exists"; exit 10; 
fi

mkdir "/opt/alljoyn/apps/$appId" 2>/dev/null || { echo "Could not create /opt/alljoyn/apps/$appId directory"; rm -rf "/opt/alljoyn/apps/$appId"; exit 11; }
mkdir "/opt/alljoyn/apps/$appId/acls" 2>/dev/null || { echo "Could not create /opt/alljoyn/apps/$appId/acls directory"; rm -rf "/opt/alljoyn/apps/$appId"; exit 12; }
mkdir "/opt/alljoyn/apps/$appId/bin" 2>/dev/null || { echo "Could not create /opt/alljoyn/apps/$appId/bin directory"; rm -rf "/opt/alljoyn/apps/$appId"; exit 13; }
## TODO mkdir "/opt/alljoyn/apps/$appId/etc" 2>/dev/null || { echo "Could not create /opt/alljoyn/apps/$appId/etc directory"; rm -rf "/opt/alljoyn/apps/$appId"; exit 14; }
mkdir "/opt/alljoyn/apps/$appId/lib" 2>/dev/null || { echo "Could not create /opt/alljoyn/apps/$appId/lib directory"; rm -rf "/opt/alljoyn/apps/$appId"; exit 15; }
## TODO mkdir "/opt/alljoyn/apps/$appId/res" 2>/dev/null || { echo "Could not create /opt/alljoyn/apps/$appId/res directory"; rm -rf "/opt/alljoyn/apps/$appId"; exit 16; }
mkdir "/opt/alljoyn/apps/$appId/store" 2>/dev/null || { echo "Could not create /opt/alljoyn/apps/$appId/store directory"; rm -rf "/opt/alljoyn/apps/$appId"; exit 17; }
## TODO mkdir "/opt/alljoyn/apps/$appId/tmp" 2>/dev/null || { echo "Could not create /opt/alljoyn/apps/$appId/tmp directory"; rm -rf "/opt/alljoyn/apps/$appId"; exit 18; }

cp -rf /opt/alljoyn/app-manager/$uuid/bin/* /opt/alljoyn/apps/$appId/bin 2>/dev/null || { echo "Could not copy files from bin directory"; rm -rf "/opt/alljoyn/apps/$appId"; exit 19; }

if [ -d "/opt/alljoyn/app-manager/$uuid/lib" ]; then
    cp -rf /opt/alljoyn/app-manager/$uuid/lib/* /opt/alljoyn/apps/$appId/lib 2>/dev/null || { echo "Could not copy files from lib directory"; rm -rf "/opt/alljoyn/apps/$appId"; exit 20; }
fi

cp /opt/alljoyn/app-manager/$uuid/Manifest.xml /opt/alljoyn/apps/$appId/ 2>/dev/null || { echo "Could not copy Manifest file"; rm -rf "/opt/alljoyn/apps/$appId"; exit 21; }

useradd $appId 2>/dev/null || { echo "Could not create a user for $appId"; rm -rf "/opt/alljoyn/apps/$appId"; exit 22; }

chown -R "$appId" "/opt/alljoyn/apps/$appId/store" 2>/dev/null || { echo "Could not chown /opt/alljoyn/apps/$appId/store directory"; rm -rf "/opt/alljoyn/apps/$appId"; userdel $appId; exit 23; }
chmod -R a+rx "/opt/alljoyn/apps/$appId/bin" 2>/dev/null || { echo "Could not chmod /opt/alljoyn/apps/$appId/bin directory"; rm -rf "/opt/alljoyn/apps/$appId"; userdel $appId; exit 24; }
chmod -R a+rx "/opt/alljoyn/apps/$appId/lib" 2>/dev/null || { echo "Could not chmod /opt/alljoyn/apps/$appId/lib directory"; rm -rf "/opt/alljoyn/apps/$appId"; userdel $appId; exit 25; }

echo "Successfully installed $1"; 
exit 0; 

