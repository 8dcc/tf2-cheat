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

old_tmp_libpath_txt="/tmp/hl2_linux_${pid}_enoch.txt"
if [[ -f "old_tmp_libpath_txt" ]]; then
    old_tmp_libpath=$(head -n 1 "$old_tmp_libpath_txt")
    if [[ -z "$old_tmp_libpath" && "$old_tmp_libpath" != "" ]]; then
        echo "inject.sh: old_tmp_libpath: $old_tmp_libpath"
    fi
fi

tmp_libpath=""
if [[ "$1" == "unload" ]] && [[ ! -z "$2" ]]; then
    # While unloading use the right library path
    tmp_libpath="$2"
else
    # Generate temp library path
    tmp_libpath="/tmp/libgl"
    tmp_libpath+=$(head /dev/urandom | tr -dc 'a-z0-9' | head -c 4)
    tmp_libpath+=".so"
    cp -p "$libpath" "$tmp_libpath"
fi
echo "inject.sh: Our cheat library is located at $tmp_libpath"

# Used to echo each command. For debugging.
#set -x

if [[ "$1" == "unload" ]] && [[ ! -z "$tmp_libpath" ]]; then
    # Unloading
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
    rm "$old_tmp_libpath_txt"
elif [[ "$1" == "debug" ]]; then
   # Injecting in debug mode
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

    echo -e "\nEnoch loaded. Unload cheat using:"
    echo -e "$0 unload $tmp_libpath"

    rm "$tmp_libpath"
    echo "$tmp_libpath" > "$old_tmp_libpath_txt"
else
    if [[ ! -z "$old_tmp_libpath" ]] && [[ "$old_tmp_libpath" != "" ]] && grep -q "$old_tmp_libpath" "/proc/$pid/maps"; then
        # Reloading
        echo -e "Enoch already loaded. Reloading...\n";

        # 0x2 -> RTLD_NOW
        # 0x6 -> RTLD_LAZY | RTLD_NOLOAD
        # For more info on the 3 mid lines, see self_unload() in main.c
        gdb -n -q -batch                                         \
            -ex "attach $pid"                                    \
            -ex "set \$dlopen = (void* (*)(char*, int))dlopen"   \
            -ex "set \$dlclose = (int (*)(void*))dlclose"        \
            -ex "set \$dlerror =  (char* (*)(void))dlerror"      \
                                                                 \
            -ex "set \$self = \$dlopen(\"$old_tmp_libpath\", 6)" \
            -ex "call \$dlclose(\$self)"                         \
            -ex "call \$dlclose(\$self)"                         \
                                                                 \
            -ex "call \$dlopen(\"$tmp_libpath\", 2)"             \
            -ex "call \$dlerror()"                               \
            -ex "detach"                                         \
            -ex "quit"
    else
        # Injecting
        gdb -n -q -batch                                       \
            -ex "attach $pid"                                  \
            -ex "set \$dlopen = (void* (*)(char*, int))dlopen" \
            -ex "set \$dlclose = (int (*)(void*))dlclose"      \
            -ex "set \$dlerror =  (char* (*)(void))dlerror"    \
            -ex "call \$dlopen(\"$tmp_libpath\", 2)"           \
            -ex "call \$dlerror()"                             \
            -ex "detach"                                       \
            -ex "quit"
    fi

    set +x

    echo -e "\nEnoch loaded. Unload cheat using:"
    echo -e "$0 unload $tmp_libpath"

    rm "$tmp_libpath"
    echo "$tmp_libpath" > "$old_tmp_libpath_txt"
fi
