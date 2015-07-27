#!/usr/bin/env bash

echo "Tagi z GITa:"
echo
git tag
echo "---------"
echo "Podaj numer wersji. Będzie częscią pliku"
read ans
for i in *.zip; do
	name=`basename $i .zip`
	echo "$name.zip => $name-$ans.zip"
done
echo "OK? (y/n)"
read confirm

if [ "$confirm" = "y" ]; then
	for i in *.zip; do
		name=`basename $i .zip`
		scp $name.zip static.nettigo.pl:NTG-STATIC/$name-$ans.zip
	done

fi

exit 0

./do-release.sh
scp *.zip static.nettigo.pl:NTG-STATIC/
