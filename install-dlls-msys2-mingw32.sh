#!/bin/sh

MINGWDIR='/mingw64'
INSTALLDIR=$(pwd)/install

function dependencies {
    local EXECUATABLEDIR=$1
    local EXECUATABLE=$2
    local MINGWDIRBIN=$3
    
    local DEPENDENCY="| grep 'DLL Name' | sed -r 's/\s+DLL\s+Name\:\s+//' | xargs -i{} which {} | grep $MINGWDIRBIN"

    local DEPENDENCIES="$DEPENDENCY"

    for I in `seq 1 3`;
    do
        local COMMAND="objdump -x $EXECUATABLE $DEPENDENCIES | xargs -i{} cp -v {} $EXECUATABLEDIR"
        eval "$COMMAND"

        local DEPENDENCIES="$DEPENDENCIES | xargs -i{} objdump -x {} $DEPENDENCY"
    done 
}

dependencies './install' './install/q2map.exe' "$MINGWDIR/bin"
dependencies './install' './install/q3data.exe' "$MINGWDIR/bin"
dependencies './install' './install/q3map2.exe' "$MINGWDIR/bin"
dependencies './install' './install/qdata3.exe' "$MINGWDIR/bin"
dependencies './install' './install/radiant.exe' "$MINGWDIR/bin"

cd $MINGWDIR

for EXTRAPATH in \
    './lib/gtk-2.0/2.10.0/engines/*.dll' \
    './lib/gtk-2.0/modules/*.dll' \
    './share/themes' \
; do
    cp --parent -v `find $EXTRAPATH -type f` "$INSTALLDIR"
done
