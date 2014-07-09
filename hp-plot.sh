#!/bin/sh
# Quick little script that will convert an eps file into hpgl
# Copyright 1998-2014 Chris Osborn <fozztexx@fozztexx.com> 
# This file is part of hp-plot.
#
# hp-plot is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# hp-plot is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with hp-plot; see the file COPYING. If not see
# <http://www.gnu.org/licenses/>.

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
