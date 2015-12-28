Welcome to Folko's POLHook 8.2 (Source)!

Installing:
1) Copy all files from the binaries directory into a subdir called POLHook of your POL directory.
2) Edit polhook.cfg.
3) Then drag POL.exe on Injector.exe. That will create a new file, pol_hooked.exe.
4) Ise that file to start your server.

Info:
You can load plugins using ".loadplugin <name>".
You can unload plugins using ".killplugin <name>".
To see the currently loaded plugins, use ".listplugins".
To reload a plugin, use ".reloadplugin <name>".
To reload POLHook.cfg, use ".reloadph".
These commands allow you to add / remove / fix features while POL is running!

Bugs:
If you ".killplugin polmon" and then ".loadplugin polmon", it will crash.
Solution: Use ".reloadplugin polmon".

Compiling:
POLHook is written in ANSI compatible C. You should be able to compile it with any
Windows compiler system. I suggest you to compile POLHook with Visual C++ .NET 2003
because then you can use the Master Solution File "POLHook.sln".

Visual C++ .NET 2003:
Just open the Master Solution File "POLHook.sln" and choose "Build Solution".

LCC:
Make a new, empty DLL project, change the name of the entrypoint to "DllMain"
and disable underscores in DLL Exports. Then add the .def files to the project,
add ws2_32.lib and compile.
Note that LCC seems to have a bug in win.h, find this:
typedef struct _IMAGE_THUNK_DATA {
	union {
		PBYTE ForwarderString;
		PDWORD Function;
		DWORD Ordinal;
		PIMAGE_IMPORT_BY_NAME AddressOfData;
	} ;
} IMAGE_THUNK_DATA,*PIMAGE_THUNK_DATA;
and replace it with this:
typedef struct _IMAGE_THUNK_DATA {
	union {
		PBYTE ForwarderString;
		PDWORD Function;
		DWORD Ordinal;
		PIMAGE_IMPORT_BY_NAME AddressOfData;
	}u1;
} IMAGE_THUNK_DATA,*PIMAGE_THUNK_DATA;
