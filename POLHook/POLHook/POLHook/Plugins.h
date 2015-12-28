/* Folko's Plugin Engine (C) Folko, 2003 */

/*
    <CVS SERVER GENERATED>
    This project was stored on Folko's CVS server as $Source: /home/cvs/POLHookSolution/POLHook/Plugins.h,v $.
    
    Id tag follows (<filename> <revision> <last changed> <author> <status>):
    $Id: Plugins.h,v 1.6 2003/06/21 13:38:43 folko23 Exp $
    
    If you want to report any errors, please include this tag.
    </GENERATED>
*/

#ifndef _PLUGINS_H_INCLUDED
#define _PLUGINS_H_INCLUDED

#define MAX_PLUGINS 50
#define PLUGIN_VERSION 0x0002

void LoadPlugins(void);
char *LoadPlugin(char *name);
char *KillPlugin(char *name);
char *ReloadPlugin(char *name);

typedef struct
{
   POLCore core;
   
   DWORD Uptime;
   void (*Pipe) (DWORD who, BYTE *Packet);
   void (*SpeechPacket) (BYTE *Packet, const char *Command, ...);
   void (*AddPacket) (BYTE PacketID, DWORD Leng);
   long (*GetNumConnections) (void);
   DWORD (*GetPlayer) (long index);
   char  *(*GetClientIP) (DWORD who);
   SOCKET (*GetSocket) (DWORD who);
   void (*Disconnect) (DWORD who);
   DWORD (*GetStatus) (DWORD who, const WORD Status);
   char *(*GetName) (DWORD who);
   char *(*GetAccount) (DWORD who);
   void (*StartScript) (DWORD who, char *script, ...);
}PluginData;

typedef struct
{
   HMODULE DLL;
   char  Name[50];
}PluginInfo;

extern PluginInfo Plugins[MAX_PLUGINS];
extern int PluginCount;

#endif	/* _PLUGINS_H_INCLUDED */
