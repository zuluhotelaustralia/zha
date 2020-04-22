#!/bin/bash

BINARY=../uoconvert

mkdir -p ../realm/

$BINARY multis
mv multis.cfg ../config/

$BINARY tiles
mv tiles.cfg ../config/

$BINARY landtiles
mv landtiles.cfg ../config/

# Non Mondain's Legacy use "width=6144" here
$BINARY  map     realm=britannia mapid=0 usedif=1 width=6144 height=4096
$BINARY  statics realm=britannia
$BINARY  maptile realm=britannia

mv -f realm/* ../realm/
echo "Only generating brit realm 1 for zha legacy"
exit 0

# Non Mondain's Legacy use "width=6144" here
$BINARY  map     realm=britannia_alt mapid=1 usedif=1 width=6144 height=4096
$BINARY  statics realm=britannia_alt
$BINARY  maptile realm=britannia_alt

$BINARY  map     realm=ilshenar mapid=2 usedif=1 width=2304 height=1600
$BINARY  statics realm=ilshenar
$BINARY  maptile realm=ilshenar

$BINARY  map     realm=malas mapid=3 usedif=1 width=2560 height=2048
$BINARY  statics realm=malas
$BINARY  maptile realm=malas

$BINARY  map     realm=tokuno mapid=4 usedif=1 width=1448 height=1448
$BINARY  statics realm=tokuno
$BINARY  maptile realm=tokuno