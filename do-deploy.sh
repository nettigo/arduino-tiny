#!/usr/bin/env bash



BASENAME="tinyBrdCore"
BOARD_MANAGER_SUFFIX="bm"

while getopts "h?t:" opt; do
    case "$opt" in
    h|\?)
        exit 0
        ;;
    t) ans=$OPTARG
        ;;
    esac
done


if [ $ans ]; then
    echo "Nazwa wersji: $ans"
    echo "Nacisnij <ENTER> by kontynuować"
    read 
else
    echo "Tagi z GITa:"
    echo
    git tag
    echo "---------"
    echo "Podaj numer wersji. Będzie częscią pliku"
    read ans
fi


BM_DIRECTORY="$BASENAME-$BOARD_MANAGER_SUFFIX-$ans"
LIB="$BASENAME-$ans".zip

echo "Katalog dla BM: $BM_DIRECTORY"

    rm -f *.zip
    zip -qr $LIB NettigoTinyBrd
    cp -fR NettigoTinyBrd $BM_DIRECTORY
    cd $BM_DIRECTORY/avr && mv * .. && cd .. && rm -fR avr && cd ..

    zip -qr $BM_DIRECTORY.zip $BM_DIRECTORY
    rm -fR $BM_DIRECTORY

echo "Lista plików"
ls -1 *zip
echo "OK? (y/n)"
read confirm




host="static.nettigo.pl"

PACKAGE="package_nettigo.pl_index.json"

if [ "$confirm" = "y" ]; then
    ruby tools/package.rb -f $BM_DIRECTORY.zip -v $ans > $PACKAGE

#    scp $LIB ${host}:NTG-STATIC/tinybrd/cores/$LIB
#    scp $BM_DIRECTORY.zip ${host}:NTG-STATIC/tinybrd/cores/$BM_DIRECTORY.zip
#    scp $PACKAGE ${host}:NTG-STATIC/tinybrd/
#    ssh -C $host "cd NTG-STATIC/
#    rm tinyBrd-current.zip
#    ln -s tinybrd/cores/$LIB tinyBrd-current.zip
#    "

fi

exit 0

./do-release.sh
scp *.zip static.nettigo.pl:NTG-STATIC/
