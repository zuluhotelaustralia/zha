SET EXE=uoconvert.exe

%EXE% multis
move multis.cfg ..\config

%EXE% tiles
move tiles.cfg ..\config

%EXE% landtiles
move landtiles.cfg ..\config

rem Non Mondain's Legacy use "width=6144" here
%EXE%  map     realm=britannia mapid=0 usedif=1 width=7168 height=4096
%EXE%  statics realm=britannia
%EXE%  maptile realm=britannia

move realm ..\realm
echo "Only generating brit realm 1 for zha legacy"
pause
exit



rem Non Mondain's Legacy use "width=6144" here
%EXE%  map     realm=britannia_alt mapid=1 usedif=1 width=7168 height=4096
%EXE%  statics realm=britannia_alt
%EXE%  maptile realm=britannia_alt

%EXE%  map     realm=ilshenar mapid=2 usedif=1 width=2304 height=1600
%EXE%  statics realm=ilshenar
%EXE%  maptile realm=ilshenar

%EXE%  map     realm=malas mapid=3 usedif=1 width=2560 height=2048
%EXE%  statics realm=malas
%EXE%  maptile realm=malas

%EXE%  map     realm=tokuno mapid=4 usedif=1 width=1448 height=1448
%EXE%  statics realm=tokuno
%EXE%  maptile realm=tokuno

pause