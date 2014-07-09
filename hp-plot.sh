#!/bin/sh
# Quick little script that will convert an eps file into hpgl
# Copyright 1998-2014 Chris Osborn <fozztexx@fozztexx.com> 
PATH=/usr/local/bin:/usr/bin:/bin
export PATH
TMP=/tmp/prout.$$
HPGL=hpgl.ps

tr '\015' '\012' | sed -e 's/^%%PaperSize: *Tabloid/90 rotate 0 -1224 translate/' > ${TMP}
echo ';;IN;' > ${TMP}2
cat ${HPGL} ${TMP} | gs -q -sNOPAUSE -sDEVICE=nullpage -r100 -g1600x1140 - >> ${TMP}2
echo ';;SP0;PU;PA16000,11400;' >> ${TMP}2
cat ${TMP}2

rm -f ${TMP} ${TMP}2
