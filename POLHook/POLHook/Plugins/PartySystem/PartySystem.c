/* POLHook PartySystem Plugin (C) Folko <Folko@elitecoder.net> */

/*
    <CVS SERVER GENERATED>
    This project was stored on Folko's CVS server as $Source: /home/cvs/POLHookSolution/Plugins/PartySystem/PartySystem.c,v $.
    
    Id tag follows (<filename> <revision> <last changed> <author> <status>):
    $Id: PartySystem.c,v 1.5 2003/06/21 13:38:44 folko23 Exp $
    
    If you want to report any errors, please include this tag.
    </GENERATED>
*/

/*  History
   1.0: - Initial release.
*/

#include <windows.h>
#include <stdio.h>
#include "Plugin.h"

PluginData POLHook;
void (**HookChain) (DWORD, BYTE *);

int WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
   return 1;
}

void KillPlugin(void)
{
   HookChain[0xBF]=0L;
   return;
}

void GenericPacketHook(DWORD who, BYTE *Packet)
{
   if (Packet[4] == 0x06) /* Party system */
   {
      switch (Packet[5])
      {
         case 0x01: /* Add member */
            {
               POLHook.StartScript(who, ".PartySystem add 0x%.8X", MAKESERIAL(Packet + 6));
            }break;
         case 0x02: /* Del member */
            {
               POLHook.StartScript(who, ".PartySystem remove 0x%.8X", MAKESERIAL(Packet + 6));
            }break;
         case 0x03: /* Msg member */
            {
               int i;
               char buf2[16];
               char Message[2048];
               int packlen = MAKEWORD(Packet[2], Packet[1]);
               
               Message[0]='\0';
               for (i = 10; i < packlen; i++)
               {
                  sprintf(buf2, "%.2X", Packet[i]);
                  strcat(Message, buf2);
               }
               POLHook.StartScript(who, ".PartySystem msg 0x%.8X %s", MAKESERIAL(Packet + 6), Message);
            }break;
         case 0x04: /* Chat */
            {
               int i;
               char buf2[16];
               char Message[2048];
               int packlen = MAKEWORD(Packet[2], Packet[1]);

               Message[0]='\0';
               for (i = 6; i < packlen; i++)
               {
                  sprintf(buf2, "%.2X", Packet[i]);
                  strcat(Message, buf2);
               }
               POLHook.StartScript(who, ".PartySystem chat %s", Message);
            }break;
         case 0x06: /* Set loot */
            {
               POLHook.StartScript(who, ".PartySystem loot %.2X", Packet[6]);
            }break;
         case 0x08: /* Accept invitation */
            {
               POLHook.StartScript(who, ".PartySystem accept 0x%.8X", MAKESERIAL(Packet + 6));
            }break;
         case 0x09: /* Decline invitation */
            {
               POLHook.StartScript(who, ".PartySystem decline  0x%.8X", MAKESERIAL(Packet + 6));
            }break;
      }
      POLHook.SpeechPacket(Packet, "");
   }
}


int POLHookPlugin(int version, PluginData pdata, void (**pHookChain) (DWORD, BYTE *) )
{
   if(version!=PLUGIN_VERSION)
      return 0;
   HookChain=pHookChain;
   POLHook=pdata;

   HookChain[0xBF]=GenericPacketHook;
   return 1;
}

