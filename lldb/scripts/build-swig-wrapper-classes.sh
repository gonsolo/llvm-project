#!/bin/sh

# build-swig-wrapper-classes.sh
#
# For each scripting language liblldb supports, we need to create the
# appropriate Script Bridge wrapper classes for that language so that 
# users can call Script Bridge functions from within the script interpreter.
# 
# We use SWIG to help create the appropriate wrapper classes/functions for
# the scripting language.  In some cases the file generated by SWIG may
# need some tweaking before it is completely ready to use.

# Below are the arguments/parameters that this script takes (and passes along
# to all the language-specific build scripts that it calls):
#
# SRC_ROOT is the root of the lldb source tree.
# TARGET_DIR is where the lldb framework/shared library gets put.
# CONFIG_BUILD_DIR is where the build-swig-Python-LLDB.sh  shell script 
#           put the lldb.py file it was generated from running SWIG.
# PREFIX is where non-Darwin systems want to put the .py and .so
#           files so that Python can find them automatically.
# debug_flag (optional) determines whether or not this script outputs 
#           additional information when running.

SRC_ROOT=$1
TARGET_DIR=$2
CONFIG_BUILD_DIR=$3
PREFIX=$4

shift 4

#
# Check to see if we are in debug-mode or not.
#

if [ -n "$1" -a "$1" = "-debug" ]
then
    debug_flag="$1"
    Debug=1
    shift
else
    debug_flag=""
    Debug=0
fi

#
# Check to see if we were called from the Makefile system. If we were, check
# if the caller wants swig to generate a dependency file.
#

if [ -n "$1" -a "$1" = "-m" ]
then
    makefile_flag="$1"
    shift
    if [ -n "$1" -a "$1" = "-M" ]
    then
        dependency_flag="$1"
        shift
    else
        dependency_flag=""
    fi
else
    makefile_flag=""
    dependency_flag=""
fi

#
# Verify that 'lldb.swig' exists.
#

if [ ! -f ${SRC_ROOT}/scripts/lldb.swig ]
then
    echo Error: unable to find file 'lldb.swig' >&2
    exit 1
fi

if [ $Debug -eq 1 ]
then
    echo "Found lldb.swig file"
fi

#
# Next look for swig
#

SWIG=
if [ -f /usr/bin/swig ]
then
    SWIG=/usr/bin/swig
else
    if [ -f /usr/local/bin/swig ]
    then
        SWIG=/usr/local/bin/swig
    fi
fi

if [ ${SWIG}a = a ]
then
    echo Error: could not find the swig binary
    exit 1
fi

#
# For each scripting language, make sure the build script for that language
# exists, and if so, call it.
#
# For now the only language we support is Python, but we expect this to
# change.

languages="Python"
cwd=${SRC_ROOT}/scripts

for curlang in $languages
do
    if [ $Debug -eq 1 ]
    then
        echo "Current language is $curlang"
    fi

    if [ ! -d "$cwd/$curlang" ]
    then
        echo "Error:  unable to find $curlang script sub-dirctory" >&2
        continue
    else

        if [ $Debug -eq 1 ]
        then
            echo "Found $curlang sub-directory"
        fi

        cd $cwd/$curlang

        filename="./build-swig-${curlang}.sh"

        if [ ! -f $filename ]
        then
            echo "Error: unable to find swig build script for $curlang: $filename" >&2
            continue
        else

            if [ $Debug -eq 1 ]
            then
                echo "Found $curlang build script."
                echo "Executing $curlang build script..."
            fi

            ./build-swig-${curlang}.sh  "$SRC_ROOT" "$TARGET_DIR" "$CONFIG_BUILD_DIR" "${PREFIX}" "${debug_flag}" "${SWIG}" "${makefile_flag}" "${dependency_flag}" || exit $?
        fi
    fi
done

