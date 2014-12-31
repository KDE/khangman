#! /bin/sh
$EXTRACTATTR --attr=theme,uiname ../themes/standardthemes.xml >> rc.cpp
$EXTRACTRC *.rc *.ui *.kcfg >> rc.cpp
LIST=`find . -name \*.h -o -name \*.cpp -o -name \*.qml`
if test -n "$LIST"; then
$XGETTEXT data.i18n $LIST rc.cpp -o $podir/khangman.pot
fi
