/* Folko's Plugin Engine for POLHook (C) Folko, 2003 */

/*
    <CVS SERVER GENERATED>
    This project was stored on Folko's CVS server as $Source: /home/cvs/POLHookSolution/Plugins/POLMon/Plugin.h,v $.
    
    Id tag follows (<filename> <revision> <last changed> <author> <status>):
    $Id: Plugin.h,v 1.6 2003/06/21 13:38:44 folko23 Exp $
    
    If you want to report any errors, please include this tag.
    </GENERATED>
*/
#ifndef _PLUGIN_H_INCLUDED
#define _PLUGIN_H_INCLUDED

#define PLUGIN_VERSION 0x0002
#define MAKESERIAL(Buffer) (*(Buffer)<<24|*(Buffer+1)<<16|*(Buffer+2)<<8|*(Buffer+3))
#define MAKEWORDPARAM(Buffer) (*(Buffer)<<8|*(Buffer+1))

typedef struct tagPOLCore
{
   char  CORENAME[100];
   DWORD TimeStamp;
   DWORD CLIENT_START;
   DWORD PACKET_TABLE_ADDRESS;
   DWORD PACKET_HANDLER_ADDRESS;
   DWORD IGNITION_HANDLER;
   DWORD ORIGINAL_PACKETHANDLER;

   WORD IP;
   WORD ACCOUNT;
   WORD X;
   WORD Y;
   WORD NAME;
   WORD CMDLEVEL;
   WORD DEAD;
   WORD HIDDEN;
   WORD CONCEALED;
   WORD FROZEN;
   WORD PARALYZED;
   WORD SQUELCHED;
   DWORD StartScript;
   void (*SendSysMessage) (DWORD who, char *Message, WORD font /*=3*/ , WORD color /*=0x480*/ );
   void (*SendPacket) (DWORD who, BYTE *Packet, WORD len);
}POLCore;

typedef struct
{
   POLCore pol;
   
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

int POLHookPlugin(int version, PluginData pdata, void (**pHookChain) (DWORD, BYTE *) );
void KillPlugin(void);
void ReloadPlugin(void);

#endif	/* _PLUGIN_H_INCLUDED */
