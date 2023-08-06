#!/bin/bash

pid=$(pidof "PROJECT-NAME")
libpath=$(realpath "libPROJECT-NAME.so")

if [ "$pid" == "" ]; then
   echo "inject.sh: process not running."
   exit 1
fi

# Used to echo the command. For debugging.
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
fi

if grep -q "$libpath" "/proc/$pid/maps"; then
    echo -e "PROJECT-NAME already loaded. Reloading...\n";

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
