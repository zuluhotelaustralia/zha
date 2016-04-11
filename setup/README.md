# Setup
This folder contains the inital files required to seed a new server.

## Instructions
1. Rename pol.exe.RENAMEME to pol.exe
2. Copy the following files from the setup folder into the root of the pol folder and modify them.
    * config\servers.cfg
    * pol.cfg
    * POLHook.cfg
3. Run the data file generation. Modify setup\pol.cfg to point to UO folder then run 'Generate Data Files.bat'.
4. Compile scripts using 'Compile.bat'. This needs to be run every time the scripts have changed.
5. Run the server batch file 'Run.bat'.

## File listing
* config\servers.cfg - Server listings.
* pol.cfg - Main pol configuration file. Change the UoDataFileRoot variable.
* POLHook.cfg - The configuration for the pol hook settings. Change the log directory.
* data\ - A copy of a fully populated old ZHA world file.
* data-empty\ - A blank world file for quick boot up script testing.
