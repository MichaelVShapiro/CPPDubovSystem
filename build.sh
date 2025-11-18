#!/bin/bash

#
# //  build.sh
# //  DubovSystem
# //
# //  Created by Michael Shapiro on 1/18/25.
# //

CPPDUBOVSYSTEM_BUILDSH_VERSION="1.0"
TARGET_CPPDUBOVSYSTEM_VERSION_INSTALL="2.1.1"

echo "CPPDubovSystem build.sh -- version $CPPDUBOVSYSTEM_BUILDSH_VERSION"
echo "CPPDubovSystem install version: $TARGET_CPPDUBOVSYSTEM_VERSION_INSTALL"

# change directory into DubovSystem
echo "Changing directory into DubovSystem"
cd DubovSystem

# lets validate that all the files are in the correct locations within the folder
# this is to really just make sure that all the required files were downloaded with the repository
echo "Checking files..."
declare -a files_to_check=("main.cpp" "graph util/Graph.cpp" "graph util/BinaryHeap.cpp" "graph util/Matching.cpp" "csv util/csv.cpp" "fpc.cpp" "trf util/trf.cpp" "trf util/rtg.cpp" "Player.cpp" "Tournament.cpp" "baku.cpp" "LinkedList.cpp")

for i in "${files_to_check[@]}"
do
    if [ -f "$i" ]; then
        echo "File $i validated"
    else
        echo "File $i is missing! This file is a part of the CPPDubovSystem installation and doesn't appear to exist in the DubovSystem directory. Try downloading this repository again!"
        echo "Exiting installation process..."
        exit 1
    fi
done

# now confirm the installation
read -p "All files validated, waiting for installation to be confirmed. Would you like to install CPPDubovSystem now? (y/n) " install_confirm

case "$install_confirm" in
    [Yy]* ) echo "Installing CPPDubovSystem...";;
    [Nn]* ) echo "Exiting..."; exit 0;;
    * ) echo "Invalid response. Please enter y or n."; exit 1;;
esac

# compile with g++ command for windows/linux and clang++ for mac

echo "Changing directory to above parent folder..."
cd ..

# we need to check the operating system first
if [ "$(uname)" == "Darwin" ]; then
    # then we use clang++ (the reccomended MacOS compiler) for compiling
    echo "Using clang++ command to install..."
    clang++ -std=c++20 -o CPPDubovSystem DubovSystem/main.cpp "DubovSystem/graph util/Graph.cpp" "DubovSystem/graph util/BinaryHeap.cpp" "DubovSystem/graph util/Matching.cpp" "DubovSystem/csv util/csv.cpp" "DubovSystem/fpc.cpp" "DubovSystem/trf util/trf.cpp" "DubovSystem/trf util/rtg.cpp" DubovSystem/Player.cpp DubovSystem/Tournament.cpp DubovSystem/baku.cpp DubovSystem/LinkedList.cpp
else
    # in that case we use g++ to compile
    echo "Using g++ command to install..."
    g++ -std=c++20 -o CPPDubovSystem DubovSystem/main.cpp "DubovSystem/graph util/Graph.cpp" "DubovSystem/graph util/BinaryHeap.cpp" "DubovSystem/graph util/Matching.cpp" "DubovSystem/csv util/csv.cpp" "DubovSystem/fpc.cpp" "DubovSystem/trf util/trf.cpp" "DubovSystem/trf util/rtg.cpp" DubovSystem/Player.cpp DubovSystem/Tournament.cpp DubovSystem/baku.cpp DubovSystem/LinkedList.cpp
fi

# lets make sure installation was a success
echo "Verifying that installation was successful"
if [ $? -eq 0 ]; then
    echo "Installation was successful! The output file (CPPDubovSystem) should be in the same folder as this buid.sh file."
else
    echo "Installation failed. Compilation failed while running g++"
    exit 1
fi

exit 0
