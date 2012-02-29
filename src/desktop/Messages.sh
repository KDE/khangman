#! /bin/sh
$EXTRACTATTR --attr=theme,uiname ../../themes/standardthemes.xml >> rc.cpp
$EXTRACTRC *.rc *.ui *.kcfg >> rc.cpp
LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`
if test -n "$LIST"; then
$XGETTEXT data.i18n $LIST rc.cpp -o $podir/khangman.pot
fi
