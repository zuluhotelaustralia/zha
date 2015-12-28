/*
    <CVS SERVER GENERATED>
    This project was stored on Folko's CVS server as $Source: /home/cvs/POLHookSolution/POLHook/POLHook.h,v $.
    
    Id tag follows (<filename> <revision> <last changed> <author> <status>):
    $Id: POLHook.h,v 1.2 2003/05/10 14:48:49 folko23 Exp $
    
    If you want to report any errors, please include this tag.
    </GENERATED>
*/

#ifndef _POLHOOK_H_INCLUDED
#define _POLHOOK_H_INCLUDED

/* Windows includes */
#include <windows.h>
#include <process.h>
#include <winsock.h>
#include <commctrl.h>

/* StdLib includes */
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <time.h>

/* Visual Studio stuff */
#pragma comment(lib, "ws2_32" )


#include "Config.h"
#include "pol.h"
#include "plugins.h"
//#include "cping.h"



#ifdef _DEBUG
#error "Don't compile in debug mode"
#endif /* _DEBUG */

void Command_ReloadPh(DWORD who, char *rest);
void Command_ListPlugins(DWORD who, char *rest);
void Command_KillPlugin(DWORD who, char *rest);
void Command_LoadPlugin(DWORD who, char *rest);
void Command_ReloadPlugin(DWORD who, char *rest);
void Command_Test(DWORD who, char *rest);

extern void (*HookChain[255]) (DWORD, BYTE *);

#endif /* _POLHOOK_H_INCLUDED */
