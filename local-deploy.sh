#!/bin/bash
# kopuje bieżące developerskie tinyBrd core do katalogu Arduino, nadpisuje pierwszy znaleziony core

DEST_DIR="$HOME/.arduino15/packages/tinybrd/hardware/avr/"

INSTALLED=`ls -1 $DEST_DIR |head -1`
rm -fR ${DEST_DIR}/${INSTALLED}/*
cp -fR NettigoTinyBrd/avr/* ${DEST_DIR}$INSTALLED

echo "Wgrano developerską wersję w miejsce wersji ${INSTALLED}"
