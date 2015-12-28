/* POLHook SingleClick Plugin  */


/*  History
   1.0: - Initial release.
*/

#include <windows.h>
#include "Plugin.h"

PluginData POLHook;
POLCore core;
int i=0;

void (**HookChain) (DWORD, BYTE *);

int WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
   return 1;
}

void KillPlugin(void)
{
   HookChain[0x09]=0;
   return;
}


void SingleClickHook(DWORD who, BYTE *Packet)
{
	//core.SendSysMessage(who, "You clicked on something", 3, 0x480);
	POLHook.Pipe(who, Packet); //Pass the Packet to POL so the name will still be displayed
	POLHook.SpeechPacket(Packet, "SingleClick 0x%.8X %d", MAKESERIAL(Packet+1), POLHook.FindStaffSerial(MAKESERIAL(Packet+1)));
	/*if (POLHook.FindStaffSerial(MAKESERIAL(Packet+1))){
		POLHook.SpeechPacket(Packet, "Found a staff serial 0x%.8X", MAKESERIAL(Packet+1));
	}
	else{
		POLHook.SpeechPacket(Packet, "Didnt find a staff serial 0x%.8X", MAKESERIAL(Packet+1));
	}*/
	//POLHook.ZHAStaffConcealedHook(who, hookedSerial);
}

int POLHookPlugin(int version, PluginData pdata, void (**pHookChain) (DWORD, BYTE *) )
{
   if(version!=PLUGIN_VERSION)
      return 0;
   HookChain=pHookChain;
   POLHook=pdata;

   HookChain[0x09]=SingleClickHook;
   return 1;
}