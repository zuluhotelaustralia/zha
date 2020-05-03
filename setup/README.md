# Setup
This folder contains the inital files required to seed a new server.

## Instructions
1. Rename /pol.exe.RENAMEME to /pol.exe
2. Copy the following files from the setup folder into the root of the pol folder and modify them.
    * /setup/config/servers.cfg -> /config/servers.cfg - Change IP if production server/
    * /setup/pol.cfg -> /pol.cfg - No changes should be needed/
    * /setup/POLHook.cfg -> /POLHook.cfg - No changes should be needed.
    * /setup/data or /setup/data-empty -> /data - To initally seed a world.
3. Run the data file generation using Generate Data Files.bat. Modify setup\pol.cfg to point to UO folder then run 'Generate Data Files.bat'.
4. Compile scripts using 'Compile.bat'. This needs to be run every time the scripts have changed.
5. Run the server batch file '/Run.bat'. Connect to localhost with port 5333.

## File listing
* /setup/config/servers.cfg - Server listings.
* /setup/pol.cfg - Main pol configuration file. Change the UoDataFileRoot variable.
* /setup/POLHook.cfg - The configuration for the pol hook settings. Change the log directory.
* /setup/data/ - A copy of a fully populated old ZHA world file.
* /setup/data-empty/ - A blank world file for quick boot up script testing.
