#!/bin/bash

pid=$(pidof "hl2_linux")
libpath=$(realpath "libenoch.so")

if [ "$pid" == "" ]; then
   echo "inject.sh: process not running."
   exit 1
fi

# Check for needed fonts. Delete these lines if you are sure all the needed
# fonts are installed.
cozettevector=$(fc-list | grep "CozetteVector" | wc -l)
if [ $cozettevector -lt 1 ]; then
    echo "ERROR: inject.sh: font \"CozetteVector\" not found."
    exit 1
fi

# Used to echo each command. For debugging.
#set -x

if [ "$1" == "unload" ]; then
    sudo gdb -n -q -batch                                   \
         -ex "attach $pid"                                  \
         -ex "set \$dlopen = (void* (*)(char*, int))dlopen" \
         -ex "set \$dlclose = (int (*)(void*))dlclose"      \
         -ex "set \$dlerror =  (char* (*)(void))dlerror"    \
                                                            \
         -ex "set \$self = \$dlopen(\"$libpath\", 6)"       \
         -ex "call \$dlclose(\$self)"                       \
         -ex "call \$dlclose(\$self)"                       \
                                                            \
         -ex "call \$dlerror()"                             \
         -ex "detach"                                       \
         -ex "quit"

    exit 0
elif [ "$1" == "debug" ]; then
   echo "Launching in debug mode."
   sudo gdb -n -q                                          \
        -ex "attach $pid"                                  \
        -ex "set \$dlopen = (void* (*)(char*, int))dlopen" \
        -ex "set \$dlclose = (int (*)(void*))dlclose"      \
        -ex "set \$dlerror =  (char* (*)(void))dlerror"    \
        -ex "call \$dlopen(\"$libpath\", 2)"               \
        -ex "call \$dlerror()"                             \
        -ex "continue" # Comment this line for manual debug

    exit 0
fi

if grep -q "$libpath" "/proc/$pid/maps"; then
    echo -e "Enoch already loaded. Reloading...\n";

    # 0x2 -> RTLD_NOW
    # 0x6 -> RTLD_LAZY | RTLD_NOLOAD
    # For more info on the 3 mid lines, see self_unload() in main.c
    sudo gdb -n -q -batch                                   \
         -ex "attach $pid"                                  \
         -ex "set \$dlopen = (void* (*)(char*, int))dlopen" \
         -ex "set \$dlclose = (int (*)(void*))dlclose"      \
         -ex "set \$dlerror =  (char* (*)(void))dlerror"    \
                                                            \
         -ex "set \$self = \$dlopen(\"$libpath\", 6)"       \
         -ex "call \$dlclose(\$self)"                       \
         -ex "call \$dlclose(\$self)"                       \
                                                            \
         -ex "call \$dlopen(\"$libpath\", 2)"               \
         -ex "call \$dlerror()"                             \
         -ex "detach"                                       \
         -ex "quit"
else
    sudo gdb -n -q -batch                                   \
         -ex "attach $pid"                                  \
         -ex "set \$dlopen = (void* (*)(char*, int))dlopen" \
         -ex "set \$dlclose = (int (*)(void*))dlclose"      \
         -ex "set \$dlerror =  (char* (*)(void))dlerror"    \
         -ex "call \$dlopen(\"$libpath\", 2)"               \
         -ex "call \$dlerror()"                             \
         -ex "detach"                                       \
         -ex "quit"
fi

set +x
echo -e "\nDone."
