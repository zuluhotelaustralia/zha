/* Config.h (C) Folko, 2003 */

/*
    <CVS SERVER GENERATED>
    This project was stored on Folko's CVS server as $Source: /home/cvs/POLHookSolution/POLHook/Config.h,v $.
    
    Id tag follows (<filename> <revision> <last changed> <author> <status>):
    $Id: Config.h,v 1.7 2003/06/21 13:38:43 folko23 Exp $
    
    If you want to report any errors, please include this tag.
    </GENERATED>
*/

#ifndef _CONFIG_H_INCLUDED
#define _CONFIG_H_INCLUDED

/*  ReadConfig parses POLHook.cfg. It also calls ParseTriggers. */
char *ReadConfig(void);
void ReloadConfig(void);

/* ParseTriggers parses all the TRIGGER-elements in POLHook.cfg */
int ParseTriggers(FILE *cfg);

/* ParseStaffSerials parses all the StaffSerial-elements in POLHook.cfg */
int ParseStaffSerials(FILE *cfg);

/* GetConfigString reads <cfg>, if it finds <name> it copies it's value into <dest>.
 * If <timestamp> is not null, it only copies the value if the timestamp of the elem matches.
*/
void GetConfigString(FILE *cfg, char *name, char *dest, DWORD timestamp);

/* GetConfigInt reads <cfg>, if it finds <name> it returns it's value.
 * If <timestamp> is not null, it only returns the value if the timestamp of the elem matches.
*/
DWORD GetConfigInt(FILE *cfg, char *name, DWORD timestamp);

typedef struct _Trigger
{
	char phrase[100];
	char resp[100];
    int display;
}Trigger;

typedef struct _StaffSerial
{
	//barns: i was gonna put more in here but no point so this is fine
	//may extend later
	unsigned long staffserial;
}StaffSerial;

typedef struct _MoveRequestStore
{
	int numMReqsThisSec;
	__time64_t time;
	int mountHackCheck;
	int xy;
	int pinglag;
}MoveRequestStore;

typedef struct tagConfig
{
   int AllowSpeechMul;
   int EnableTriggers;
   int HookCrypt;
   int MaxCmdLevel;
   int TriggerCount;
   int MaxSimultaneousClientForSingleIP;
   int StaffSerialCount;
   int MaxStaffSerialCount;
   int MaxSimultaneousMoveRequests;
   int MaxOnLinePlayers;
   char LogPath[MAX_PATH];
   int UniCode;
   int MoveHookEnabled;
   int AOSCharProfileHookEnabled;
   int GodModeHookEnabled;
   int SingleClickHookEnabled;
   int DoubleClickHookEnabled;
   int GetStatusHookEnabled;
   int AttackHookEnabled;
   int ClientVersionHookEnabled;
   int TargetHookEnabled;
   int AccountLoginHookEnabled;
   int EnablePinger;
   DWORD Uptime;
}Config;

typedef struct tagPOLCore
{
   char  CORENAME[100];
   char  CONCEALEDFIXPASSWORD[100];
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
   void (*SendSysMessage) (DWORD who, char *text, WORD font, WORD color);
   void (*SendPacket) (DWORD who, BYTE *Packet, WORD len);
//   void (*InitialisePinger)();
 //  void (*ClosePinger)();
//   int (*MyPing)(char* strHost, ICMPECHO icmpEcho);
}POLCore;

extern Config conf;
extern POLCore core;
extern Trigger *Triggers;
extern StaffSerial *StaffSerials;
extern MoveRequestStore *MyMoveRequestStore;


#endif /* _CONFIG_H_INCLUDED */
