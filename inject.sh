#!/bin/bash

# Verify that we are root
if [[ ! $(id -u) -eq 0 ]]; then
    echo "This script must be ran as root!"
    exit 1
fi

# -s is used to only get one pid
# If you need to get an array of processes remove it
pid=$(pidof -s "hl2_linux")
libpath=$(realpath "libenoch.so")

if [[ ! -f "$libpath" ]]; then
    echo "ERROR: inject.sh: Compile cheat first!"
    exit 1
fi

if [[ -z "$pid" ]] || [[ "$pid" == "" ]]; then
   echo "ERROR: inject.sh: process not running."
   exit 1
fi

# Check for needed fonts. Delete these lines if you are sure all the needed
# fonts are installed.
cozettevector=$(fc-list | grep "CozetteVector" | wc -l)
if [[ $cozettevector -lt 1 ]]; then
    echo "ERROR: inject.sh: font \"CozetteVector\" not found."
    exit 1
fi

tmp_libpath=""
if [[ "$1" == "unload" ]] && [[ ! -z "$2" ]]; then
    # While unloading use the right library path
    tmp_libpath="$2"
else
    # Generate new temp directory file
    tmp_libpath="/tmp/libgl"
    tmp_libpath+=$(head /dev/urandom | tr -dc 'a-z0-9' | head -c 4)
    tmp_libpath+=".so"
    cp -p "$libpath" "$tmp_libpath"
fi
echo "Our cheat library is located at $tmp_libpath"

# Used to echo each command. For debugging.
#set -x
if [[ "$1" == "unload" ]] && [[ ! -z "$tmp_libpath" ]]; then
    gdb -n -q -batch                                        \
         -ex "attach $pid"                                  \
         -ex "set \$dlopen = (void* (*)(char*, int))dlopen" \
         -ex "set \$dlclose = (int (*)(void*))dlclose"      \
         -ex "set \$dlerror =  (char* (*)(void))dlerror"    \
                                                            \
         -ex "set \$self = \$dlopen(\"$tmp_libpath\", 6)"   \
         -ex "call \$dlclose(\$self)"                       \
         -ex "call \$dlclose(\$self)"                       \
                                                            \
         -ex "call \$dlerror()"                             \
         -ex "detach"                                       \
         -ex "quit"
elif [[ "$1" == "debug" ]]; then
   echo "Launching in debug mode."
   gdb -n -q                                               \
        -ex "attach $pid"                                  \
        -ex "set \$dlopen = (void* (*)(char*, int))dlopen" \
        -ex "set \$dlclose = (int (*)(void*))dlclose"      \
        -ex "set \$dlerror =  (char* (*)(void))dlerror"    \
        -ex "call \$dlopen(\"$tmp_libpath\", 2)"           \
        -ex "call \$dlerror()"                             \
        -ex "continue" # Comment this line for manual debug

    set +x
    echo -e "\nDone. Unload cheat using:\n$0 unload $tmp_libpath"
    rm "$tmp_libpath"
else
    # Im still thinking of a way to reload cheat while using temp library
    # so i'll remove it for now.
    gdb -n -q -batch                                       \
        -ex "attach $pid"                                  \
        -ex "set \$dlopen = (void* (*)(char*, int))dlopen" \
        -ex "set \$dlclose = (int (*)(void*))dlclose"      \
        -ex "set \$dlerror =  (char* (*)(void))dlerror"    \
        -ex "call \$dlopen(\"$tmp_libpath\", 2)"           \
        -ex "call \$dlerror()"                             \
        -ex "detach"                                       \
        -ex "quit"
    set +x
    echo -e "\nDone. Unload cheat using:\n$0 unload $tmp_libpath"
    rm "$tmp_libpath"
fi
