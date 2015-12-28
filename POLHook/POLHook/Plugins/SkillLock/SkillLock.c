/* POLHook SkillLock Plugin (C) Folko <Folko@elitecoder.net> */

/*
    <CVS SERVER GENERATED>
    This project was stored on Folko's CVS server as $Source: /home/cvs/POLHookSolution/Plugins/SkillLock/SkillLock.c,v $.
    
    Id tag follows (<filename> <revision> <last changed> <author> <status>):
    $Id: SkillLock.c,v 1.4 2003/06/21 13:38:44 folko23 Exp $
    
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
   POLHook.AddPacket(0x3A, 0L);
   HookChain[0x3A]=0L;
   return;
}

void SkillLockHook(DWORD who, BYTE *Packet)
{
   POLHook.StartScript(who, ".SkillLock %.4X %.2X", MAKEWORDPARAM(Packet + 3), Packet[5]);
   POLHook.SpeechPacket(Packet, "");
}

int POLHookPlugin(int version, PluginData pdata, void (**pHookChain) (DWORD, BYTE *) )
{
   if(version!=PLUGIN_VERSION)
      return 0;
   HookChain=pHookChain;
   POLHook=pdata;

   POLHook.AddPacket(0x3A, -2L);
   HookChain[0x3A]=SkillLockHook;
   return 1;
}

