/* POLHook Snooping Plugin (C) Folko <Folko@elitecoder.net> */

/*
    <CVS SERVER GENERATED>
    This project was stored on Folko's CVS server as $Source: /home/cvs/POLHookSolution/Plugins/Snooping/Snooping.c,v $.
    
    Id tag follows (<filename> <revision> <last changed> <author> <status>):
    $Id: Snooping.c,v 1.4 2003/06/21 13:38:44 folko23 Exp $
    
    If you want to report any errors, please include this tag.
    </GENERATED>
*/

/*  History
   1.0: - Initial release.
*/

#include <windows.h>
#include "Plugin.h"

PluginData POLHook;
void (**HookChain) (DWORD, BYTE *);

int WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
   return 1;
}

void KillPlugin(void)
{
   HookChain[0x06]=0L;
   return;
}

void DoubleClickHook(DWORD who, BYTE *Packet)
{
	POLHook.Pipe(who, Packet);
	POLHook.SpeechPacket(Packet, "DoubleClick 0x%.8X", MAKESERIAL(Packet+1));
   //POLHook.StartScript(who, ".DoubleClick 0x%.8X", MAKESERIAL(Packet + 1));
}

int POLHookPlugin(int version, PluginData pdata, void (**pHookChain) (DWORD, BYTE *) )
{
   if(version!=PLUGIN_VERSION)
      return 0;
   HookChain=pHookChain;
   POLHook=pdata;

   HookChain[0x06]=DoubleClickHook;
   return 1;
}

