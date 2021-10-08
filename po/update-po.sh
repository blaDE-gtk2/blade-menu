#!/bin/sh


BASEDIR=../bar-plugin
WDIR=`pwd`


echo -n 'Preparing desktop file...'
cd ${BASEDIR}
rm -f blademenu.desktop.in.h
rm -f blademenu.desktop.in
cp blademenu.desktop blademenu.desktop.in
sed -e '/Name\[/ d' \
	-e '/Comment\[/ d' \
	-e 's/Name/_Name/' \
	-e 's/Comment/_Comment/' \
	-i blademenu.desktop.in
intltool-extract --quiet --type=gettext/ini blademenu.desktop.in
cd ${WDIR}
echo ' DONE'


echo -n 'Extracting messages...'
# Sort alphabetically to match cmake
xgettext --from-code=UTF-8 --c++ --keyword=_ --keyword=N_:1 --sort-output \
	--package-name='blademenu' --copyright-holder='Graeme Gott' \
	--output=blade-menu-plugin.pot ${BASEDIR}/*.cpp ${BASEDIR}/*.h
echo ' DONE'


echo -n 'Merging translations...'
for POFILE in *.po;
do
	echo -n " $POFILE"
	msgmerge --quiet --update --backup=none $POFILE blade-menu-plugin.pot
done
echo ' DONE'


echo -n 'Merging desktop file translations...'
cd ${BASEDIR}
intltool-merge --quiet --desktop-style ${WDIR} blademenu.desktop.in blademenu.desktop
rm -f blademenu.desktop.in.h
rm -f blademenu.desktop.in
echo ' DONE'
