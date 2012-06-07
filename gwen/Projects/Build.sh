#!/bin/bash

UNAME=`uname`

if [ "$UNAME" == "Darwin" ]; then
 
    chmod 777 ./premake4-osx
    ./premake4-osx clean
    ./premake4-osx gmake
    ./premake4-osx xcode3
    ./premake4-osx xcode4
    
    cd macosx/gmake/
    make config=release
    make config=debug
    
fi
	
if [ "$UNAME" == "Linux" ]; then

    ./premake4 clean
    ./premake4 gmake
    
    cd linux/gmake/
    make config=release
    make config=debug
    
fi

