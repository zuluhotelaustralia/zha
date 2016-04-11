cd %~dp0\..
del /S "config\multis.cfg"
del /S "config\tiles.cfg"
del /S "config\landtiles.cfg"
RMDIR "realm" /S /Q
pause