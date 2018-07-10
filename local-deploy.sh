#!/bin/bash -x
DEST_DIR="$HOME/.arduino15/packages/tinybrd/hardware/avr/"

INSTALLED=`ls -1 $DEST_DIR |head -1`
#rm -fR ${INSTALLED}/*
cp -fR NettigoTinyBrd/avr/* ${DEST_DIR}$INSTALLED
