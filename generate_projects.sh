#!/bin/bash
if [ ! -f "premake/premake5" ]; then 
    echo "Premake cannot be found, please run 'init' to acquire it."
    read -p "Press enter to continue"
    exit 
fi

if [ $# -gt 0 ]; then 
    args=$@
else
    os=$(uname -s)
    if [ "$os" == "Linux" ]; then
        args="gmake"
    elif [ "$os" == "Darwin" ]; then
        args="xcode4"
    else
        args="vs2017"
    fi
fi

printf "\nSetting up Software Renderer project\n"
premake/premake5 $args --file=premake/premake5.lua

# printf "\nSetting up Demos\n"
# premake5 $args
