#!/bin/sh
#maprShortVersion=6.2.0
#release=.GA
cd $1 > /dev/null 2>&1
GIT_BRANCH=`git describe --all | cut -d '/' -f2`
if [ -z "$GIT_BRANCH" ] ; then
  GIT_BRANCH=`git describe --long`
fi
DATE_TAG=`date +%Y%m%d%H%M%S`
HEAD_CHECKSUM=`git log -n 1 --format="%H"`
#echo ":$GIT_BRANCH-$maprShortVersion.$DATE_TAG$release-$HEAD_CHECKSUM"
echo ":$GIT_BRANCH-$DATE_TAG-$HEAD_CHECKSUM"
cd - > /dev/null 2>&1
