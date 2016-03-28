#!/bin/bash

# set -ex

INSTALLDIR=$(pwd)/install

if [[ $(file $INSTALLDIR/radiant.exe) == *"x86-64"* ]]; then
    MINGWDIR=/mingw64
else
    MINGWDIR=/mingw32
fi

function dependencies_single_target_no_depth {
    local TARGET=$1
    
    local DEPENDENCIESFILTER="| grep 'DLL Name' | sed -r 's/\s+DLL\s+Name\:\s+//' | xargs -i{} which {} | grep $MINGWDIR/bin"
    local COMMAND="objdump -x $TARGET $DEPENDENCIESFILTER | xargs -i{} echo {}"
    
    local DEPENDENCIES=$(eval "$COMMAND")
    
    if [ "$DEPENDENCIES" != "" ]; then
        echo "$DEPENDENCIES"
    fi
}

function dependencies {
    local TARGETS=$@
    
    local CURRENTDEPENDENCIES=""

    for TARGET in $TARGETS; do
        local CURRENTDEPENDENCIES=$(dependencies_single_target_no_depth "$TARGET" && echo "$CURRENTDEPENDENCIES")
    done
    
    local CURRENTDEPENDENCIES=$(echo "$CURRENTDEPENDENCIES" | sort -u)
    
    local NEWDEPENDENCIES="$CURRENTDEPENDENCIES"
    
    while [ "$NEWDEPENDENCIES" != "" ]; do
        local DEPENDENCIES=""
    
        for DEPENDENCY in $NEWDEPENDENCIES; do
            DEPENDENCIES=$(dependencies_single_target_no_depth "$DEPENDENCY" && echo "$DEPENDENCIES")
        done

        local NEWDEPENDENCIES=$(comm -13 <(echo "$CURRENTDEPENDENCIES") <(echo "$DEPENDENCIES" | sort -u))

        if [ "$NEWDEPENDENCIES" != "" ]; then
            local CURRENTDEPENDENCIES=$(printf '%s\n' "$CURRENTDEPENDENCIES" "$NEWDEPENDENCIES" | sort)
        fi
    done
    
    if [ "$CURRENTDEPENDENCIES" != "" ]; then
        echo "$CURRENTDEPENDENCIES"
    fi
}

for DEPENDENCY in $(dependencies ./install/*.exe); do
    cp -v "$DEPENDENCY" "$INSTALLDIR"
done

cd $MINGWDIR

for EXTRAPATH in \
    './lib/gtk-2.0/2.10.0/engines/*.dll' \
    './lib/gtk-2.0/modules/*.dll' \
    './share/themes' \
; do
    cp --parent -v `find $EXTRAPATH -type f` "$INSTALLDIR"
done
