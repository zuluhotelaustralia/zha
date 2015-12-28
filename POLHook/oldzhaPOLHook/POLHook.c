/*  POLHook 8.2 (C) Folke 'Folko' Will, <Folko@elitecoder.net> */
/*  ZHA Functionality added by Craig Jones (Barnabus) */

/*
    <CVS SERVER GENERATED>
    This project was stored on Folko's CVS server as $Source: /home/cvs/POLHookSolution/POLHook/POLHook.c,v $.
    


    Id tag follows (<filename> <revision> <last changed> <author> <status>):
    $Id: POLHook.c,v 1.8 2003/07/08 02:56:17 folko23 Exp $
    
    If you want to report any errors, please include this tag.
    </GENERATED>
*/ 

/* History
   8.2 / 08 Jul 2003:
         - Ignition hook working for 095 again.

   8.1 / 21 Jun 2003:
         - Improved the performance by getting rid of the packet trampoline.
           As a side effect, POLHook is now compatible with the new 095 cores
           without further changes.

   8.0 / 23 Apr 2003:
         - Now using a Plugin Engine.
         - Found a better way to start scripts. You can now use a string with an
           infinite length.

   7.2 / 02 Apr 2003:
         - Fixed a bug where eg. ".createnpc banker" would call the trigger "bank".

   7.1 / 12 Mar 2003:
         - Just a few config parser optimizations
         - Fixed a small bug in the trigger system (It's a client bug actually)

   7.0 / 08 Mar 2003:
         - Now using 'DWORD', 'WORD' and 'BYTE' to make the source look nicer.
         - Moved a lot of stuff into a configuration file.
         - Added ".polmon" to start POLMon.
         - Added ".reloadph" to reload some options of POLHook.cfg.
         - Added pragmas for Visual Studio to automatically add the required .lib's.

*/
#include "POLHook.h"
#include "icmpdefs.h"
//#include <iostream.h>
#include <winsock.h>
#include <windowsx.h>



/* Hook prototypes */
void GodModeHook(DWORD who, BYTE *Packet);
void UnicodeSpeechHook(DWORD who, BYTE *Packet);
void AOSSpecialFeaturesHook(DWORD who, BYTE *Packet); // Craig Jones
void AOSNotSupportedHook(DWORD who, BYTE *Packet); // Craig Jones
void AOSCharProfileHook(DWORD who, BYTE *Packet); // Craig Jones
void SingleClickHook(DWORD who, BYTE *Packet); // Craig Jones
void GetStatusHook(DWORD who, BYTE *Packet); // Staff detect, Seravy
void DoubleClickHook(DWORD who, BYTE *Packet); // Craig Jones
void MoveHook(DWORD who, BYTE *Packet); // Craig Jones
void AttackHook(DWORD who, BYTE *Packet); // Craig Jones
void ClientVersionHook(DWORD who, BYTE *Packet); //Craig Jones
void PingHook(DWORD who, BYTE *Packet); //Craig Jones
void AccountLoginHook(DWORD who, BYTE *Packet); // Craig Jones
void TargetHook(DWORD who, BYTE *Packet); // Craig Jones
void AsciiSpeechHook(DWORD who, BYTE *Packet);// Craig Jones


/* Normal prototypes */
void SetupHooks(void);
void HookPacket(DWORD who, BYTE *Packet);
int FindStaffSerial (unsigned long queryserial); // Craig Jones
void AddStaffSerial (unsigned long queryserial); // Craig 
void SearchItemFile(DWORD who, char* itemname); // Craig Jones
void SearchObjTypeFile(DWORD who, char *objtypequery); // Craig Jones
void writeToStaffLogFile(char *name, char *charname, char *logmsg); // Craig Jones
void mysubstr (char *buffer, char *copiedfrom, int length); // Craig Jones
int CheckPastMaxClientsForIP(DWORD who);// Craig Jones
void ListStaffSerials();
void TellOnlineStaff(char *msg);
int InitialisePinger();
void ClosePinger();
int MyPing(char* strHost);
void PingClientIPAddress(DWORD who);
/* Globals */
static FILE *hooklog; /* Hook logfile */
void (*HookChain[255]) (DWORD, BYTE *); /* Holds the hooked functions addresses */

TextCommand Commands[20];
int CommandCount=0;
int pingerActive=0;

void CALLBACK IgnitionGameCryptHook(BYTE *Source, BYTE *Dest, WORD len)
{
   /* You can do your decryption here.
    * Do not attempt to do anything else but plain decryption here
    * as the packets arrive as a stream, not as whole.
    * Ie., a 100 byte packet could arrive as chunks of 10 bytes each.
    */
   memcpy(Dest, Source, len); /* Default Ignition; Dest=Source. */
}

/* DLL Entry point, shouldn't be changed. */
int APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
   if (ul_reason_for_call == DLL_PROCESS_DETACH)
   {
      if(hooklog)
         fclose(hooklog); /* Close log on process_detach */
      if(Triggers)
         free(Triggers);
	  if(pingerActive)
		 ClosePinger();
   }
   return 1;
}

void Initer(void *nil)
{
   char *error=0;

   /* TODO: Find a better way than Sleep. */
   Sleep(1000); /* Wait a bit so POL can initialize it's heap etc. */
   if(error=ReadConfig()) /* Checks if your core is compatible and initializes POLHook if it is. */
   {
      MessageBox(0, error, "Fatal Error!", MB_ICONERROR);
      return;
   }
   conf.Uptime=GetTickCount()-1000; /* "Undo" the Sleep. */
   if(strlen(conf.LogPath) && strlen(core.CORENAME))
   {
      hooklog = fopen(conf.LogPath, "w"); /* Open logfile and clean it */
      fprintf(hooklog, "This log was created using POLHook\n");
      fprintf(hooklog, "Detected core %s.\n", core.CORENAME);
      fflush(hooklog);
   } else {
      hooklog = 0;
   }
   SetupHooks(); /* Fill hookchain */
   HookCore(HookPacket); /* Apply packethook */
   if(conf.HookCrypt)
      HookGameCrypt(IgnitionGameCryptHook); /* Apply crypthook */

   LoadPlugins();
   if (conf.EnablePinger && InitialisePinger()){
		pingerActive=1;
		printf("Initialised Pinger!\n");
   }
   _endthread(); /* End this thread */
   return;
}

/* This function is called by hooked POL cores on startup */
void WINAPI Init(void)
{
   /* Had to put everything into an extra thread so POL continues starting */
   _beginthread(Initer, 0, 0); /* Init POLHook */
   return;
}

/* The actual PacketHook, better don't change it since every line could decrease performance.. */
void HookPacket(DWORD who, BYTE *Packet)
{
   //printf("%s 0x%x\n", GetAccount(who), Packet[0]);  //tells u what packet was recieved by server on console		
	if (Packet[0]==0xAD)
      UnicodeSpeechHook(who, Packet);
	if (HookChain[Packet[0]])
      HookChain[Packet[0]] (who, Packet);
	Pipe(who, Packet);
   return;
}

/* Add your hooks here or in external Plugins. */
void SetupHooks(void)
{
   printf("POLHook HOOK CHAIN SETUP\n");
   memset(HookChain, 0, sizeof(HookChain)); /* Clear HookChain */
   //barns
   HookChain[0x03] = AsciiSpeechHook;
   HookChain[0x3A] = AOSNotSupportedHook;
   HookChain[0x98] = AOSNotSupportedHook;
   HookChain[0xbf] = AOSSpecialFeaturesHook;
   HookChain[0x73] = PingHook;

   if (conf.AOSCharProfileHookEnabled){
	HookChain[0xb8] = AOSCharProfileHook;
	printf("AOSCharProfileHook Enabled\n");
   }
   if (conf.GodModeHookEnabled){
	HookChain[0x04] = GodModeHook;
	printf("GodModeHook  Enabled\n");
   }
   if (conf.SingleClickHookEnabled){
	HookChain[0x09]=SingleClickHook;
    printf("SingleClickHook  Enabled\n");
   }
   if (conf.GetStatusHookEnabled){
	HookChain[0x34] = GetStatusHook;
    printf("GetStatusHook Enabled\n");
   }
   if (conf.DoubleClickHookEnabled){
	HookChain[0x06]=DoubleClickHook;
	printf("DoubleClickHook Enabled\n");
   }
   if (conf.AttackHookEnabled){
	HookChain[0x05]=AttackHook;
	printf("AttackHook Enabled\n");
   }
   if (conf.MoveHookEnabled){
	HookChain[0x02]=MoveHook;
	printf("MoveHook  Enabled\n");
   }
   if (conf.ClientVersionHookEnabled){
	HookChain[0xBD]=ClientVersionHook;
	printf("ClientVersionHook Enabled\n");
   }
   if (conf.TargetHookEnabled){
	HookChain[0x6C]=TargetHook;
	printf("TargetHook  Enabled\n");
   }
   if (conf.AccountLoginHookEnabled){
	HookChain[0x80]=AccountLoginHook;
	printf("AccountLoginHook Enabled\n");
   }
   AddTextCommand("loadplugin", Command_LoadPlugin);
   AddTextCommand("killplugin", Command_KillPlugin);
   AddTextCommand("listplugins", Command_ListPlugins);
   AddTextCommand("reloadplugin", Command_ReloadPlugin);
   AddTextCommand("reloadph", Command_ReloadPh);
   return;
}

void AccountLoginHook(DWORD who, BYTE *Packet){
	unsigned char BlockedAccountPacket[]={0x82, 2};
	if (CheckPastMaxClientsForIP(who)){
		printf("Account login blocked due to more than %d clients at IP %s\n",conf.MaxSimultaneousClientForSingleIP,GetClientIP(who));
		core.SendPacket(who, BlockedAccountPacket, 2);
	}
}

void TargetHook(DWORD who, BYTE *Packet){
	char myserial[80];
	if (GetStatus(who, core.CMDLEVEL)>=1){
		return;
	}
	if (FindStaffSerial(MAKESERIAL(Packet+(3 + sizeof(DWORD))))){
		//SpeechPacket(Packet, ".__TargetStaffFix 0x%.8X %s", MAKESERIAL(Packet+(3 + sizeof(DWORD))), core.CONCEALEDFIXPASSWORD);
		sprintf(myserial, ".__TargetStaffFix 0x%.8X %s", MAKESERIAL(Packet+(3 + sizeof(DWORD))), core.CONCEALEDFIXPASSWORD);
		StartScript(who, myserial );
	}
}

void AOSSpecialFeaturesHook(DWORD who, BYTE *Packet){
	//char anim hook
	if (MAKEWORDPARAM(Packet+1)==9){
		//SpeechPacket(Packet, ".__ConcealedStaffFix 0x%.8X %d %s", MAKESERIAL(Packet+1), 0, core.CONCEALEDFIXPASSWORD);
		SpeechPacket(Packet, ".__DoAction %d %s", MAKEWORDPARAM(Packet+7), core.CONCEALEDFIXPASSWORD);
	}
	//SpeechPacket(Packet, "%d %d %d %d %d %d %d %d",MAKEWORDPARAM(Packet+1),MAKEWORDPARAM(Packet+2),MAKEWORDPARAM(Packet+3),MAKEWORDPARAM(Packet+4),MAKEWORDPARAM(Packet+5),MAKEWORDPARAM(Packet+6),MAKEWORDPARAM(Packet+7),MAKEWORDPARAM(Packet+8));
	if (MAKEWORDPARAM(Packet+2)==2560 && MAKEWORDPARAM(Packet+3)==6){
		core.SendSysMessage(who, "Use .party to access all party commands and .pc for party chat.",3,53);
		SpeechPacket(Packet, ".party");
	}
}

void ClientVersionHook(DWORD who, BYTE *Packet){ //best post login hook
	unsigned long whoserial;

	//Ping to get lag estimate
	
	//PingIPAddress(GetClientIP(who));

	//char buffer[20];
	if (GetStatus(who, core.CMDLEVEL)>=1){
		ListStaffSerials();
		whoserial = GetWhoSerial(who);
		//writeToStaffLogFile(GetAccount(who), GetName(who), "*LOGGED IN*");
		if(!FindStaffSerial(whoserial)){
			AddStaffSerial(whoserial);
			core.SendSysMessage(who, "Concealed-detection protection enabled.", 3, 0x480);
		}
		else{
			core.SendSysMessage(who, "Concealed-detection protection re-enabled.", 3, 30);
		}
	}
	else if (CheckPastMaxClientsForIP(who)){
		printf("Account login blocked due to more than %d clients at IP %s\n",conf.MaxSimultaneousClientForSingleIP,GetClientIP(who));
		Disconnect(who);
	}
}

void PingHook(DWORD who, BYTE *Packet){ //record player ping for speedhack calc
	//_beginthread(PingClientIPAddress(who), 0, 0); /* Ping */
	//_beginthread(PingClientIPAddress, 0, (void*)who);
	_beginthread(PingClientIPAddress, 0, who);
	//PingClientIPAddress(GetClientIP(who));
}

void AOSNotSupportedHook(DWORD who, BYTE *Packet){
	SpeechPacket(Packet, "");
}



void AOSCharProfileHook(DWORD who, BYTE *Packet){
	SpeechPacket(Packet, ".__CharProfile 0x%.8X %s", MAKESERIAL(Packet+4), core.CONCEALEDFIXPASSWORD);
}

void AttackHook(DWORD who, BYTE *Packet){
	char myserial[80];
	//SpeechPacket(Packet, "attacking serial 0x%.8X", MAKESERIAL(Packet+1));
	sprintf(myserial, ".__AttackInitiation 0x%.8X %s", MAKESERIAL(Packet+1), core.CONCEALEDFIXPASSWORD);
	//printf(myserial);
	StartScript(who, myserial );
	/*if (GetStatus(who, core.CMDLEVEL)>=1){
		writeToStaffLogFile(GetAccount(who), GetName(who), "*INITIATED ATTACK*");
	}*/
}

void AsciiSpeechHook(DWORD who, BYTE *Packet){
	//char cmd[2048];
    //int len, max;
	//max=(MAKEWORDPARAM(Packet+1)-12)/2-1; /* Grr, the client doesn't NULL-terminate the string properly */
    //len=WideCharToMultiByte(CP_ACP, 0, (WCHAR *)(Packet+7), max, cmd, sizeof(cmd), 0, 0);
    //cmd[len]='\0'; /* Maybe because WideCharToMultiByte doesn't NULL-terminate either.. */
	//SpeechPacket(Packet, ".__AsciiSpeechFix %s %s", cmd, core.CONCEALEDFIXPASSWORD);
	SpeechPacket(Packet, ".__AsciiSpeechFix %s", core.CONCEALEDFIXPASSWORD);
}

void UnicodeSpeechHook(DWORD who, BYTE *Packet)
{
   char cmd[2048];
   char tempstr[200];
   int i, j, len, max;
   char *pdest;
   

   if ((Packet[3] & 0xC0) && !conf.AllowSpeechMul) /* Client has speech.mul (Crashes others) */
   {
      core.SendSysMessage(who, "Please delete the speech.mul file in your UO directory or you won't be able to talk.", 3, 0x480);
      core.SendSysMessage(who, "If you still get the message after deleting the file, make sure you don't have your UO CD in drive.", 3, 0x480);
      if(hooklog)
         fprintf(hooklog, "%s is using speech.mul.\n", GetClientIP(who));
      SpeechPacket(Packet, "");
      return;
   }
   max=(MAKEWORDPARAM(Packet+1)-12)/2-1; /* Grr, the client doesn't NULL-terminate the string properly */
   len=WideCharToMultiByte(CP_ACP, 0, (WCHAR *)(Packet+13), max, cmd, sizeof(cmd), 0, 0);
   cmd[len]='\0'; /* Maybe because WideCharToMultiByte doesn't NULL-terminate either.. */
	
	//Barns -- quick check for new line characters.. cos of zeebs crash exploit
   pdest = strchr( cmd, '\n' );
   if ( pdest!= NULL ){
      //core.SendSysMessage(who, "Detected sending newline character", 3, 0x480);
	  SpeechPacket(Packet, "");
      return;
   }

   //Barns -- anything staff say is written to logs
   if (GetStatus(who, core.CMDLEVEL)>=1){
		writeToStaffLogFile(GetAccount(who), GetName(who), cmd);
   }

   //changed so that everything everyone says is logged
   //writeToStaffLogFile(GetAccount(who), GetName(who), cmd);

   for(i=0; i<(int)strlen(cmd); i++)
      cmd[i]=tolower(cmd[i]);

   /* Internal triggers */
   if(cmd[0]=='.' || cmd[0]== '=' && (int)GetStatus(who, core.CMDLEVEL)>=conf.MaxCmdLevel)
   {
      for(i=0; i<CommandCount; i++)
         if(strncmp(cmd+1, Commands[i].phrase, strlen(Commands[i].phrase))==0)
         {
            Commands[i].Handler(who, cmd+1+strlen(Commands[i].phrase)+1);
            SpeechPacket(Packet, "");
            break;
         }
   }

   /* Configurable Triggers */
   if(conf.EnableTriggers && cmd[0]!='.' && cmd[0]!='=')
   {
      for(i=0; i<conf.TriggerCount; i++)
      {
         if(strstr(cmd, Triggers[i].phrase))
         {
            if(!Triggers[i].display)
               SpeechPacket(Packet, "");
            StartScript(who, Triggers[i].resp);
            return;
         }
      }
   }
	
   
   /* Barns Special POLHOOK staff commands */


   if(cmd[0]=='.' && (int)GetStatus(who, core.CMDLEVEL)>=3) //gm
   {
		if(strstr(cmd, ".searchitem"))
        {
			//find words after command
			tempstr[0]='\0';
			i=11;
			j=0;
			while (cmd[i]!='\0' && cmd[i]==' '){
				i++;
			}
			while (cmd[i]!='\0'){
				tempstr[j++]=cmd[i++];
			}
			if (strlen(tempstr)<1){
				core.SendSysMessage(who, "Usage: .searchitem <name of item to search for>", 3, 0x480);
				SpeechPacket(Packet, "");
				return;
			}
			tempstr[j++]='\0';
			//core.SendSysMessage(who, tempstr, 3, 0x480);
			SearchItemFile(who, tempstr);
			SpeechPacket(Packet, ".searchreport %s", tempstr);
			return;
        }

		if(strstr(cmd, ".searchobjtype"))
        {
			//find words after command
			tempstr[0]='\0';
			i=14;
			j=0;
			while (cmd[i]!='\0' && cmd[i]==' '){
				i++;
			}
			while (cmd[i]!='\0'){
				tempstr[j++]=cmd[i++];
			}
			if (strlen(tempstr)<1){
				core.SendSysMessage(who, "Usage: .searchobjtype <hexcode of objtype to find>", 3, 0x480);
				SpeechPacket(Packet, "");
				return;
			}
			tempstr[j++]='\0';
			//core.SendSysMessage(who, tempstr, 3, 0x480);
			SearchObjTypeFile(who, tempstr);
			SpeechPacket(Packet, ".searchreport %s", tempstr);
			return;
        }
   }
}



void GodModeHook(DWORD who, BYTE *Packet) /* Support GodClient for staff */
{
   unsigned char enable_godmode[] = { 0x2B, 0x01 };

   printf("God logged in from %s / CmdLevel %d\n", GetClientIP(who), GetStatus(who, core.CMDLEVEL));
   GetStatus(who, core.CMDLEVEL) < 3 ? Disconnect(who) : core.SendPacket(who, enable_godmode, 2);
   SpeechPacket(Packet, "");
}



void MoveHook(DWORD who, BYTE *Packet)
{
	int MaxMoveRequestsPingModified=0;
	char scriptparams[80];
	int i=0;
	__time64_t ltime;
	
    _time64( &ltime );

	//if (GetStatus(who, core.CMDLEVEL)>=1){
	//	return;
	//}
	
	for (i=0; i<GetNumConnections(); i++){
		if (GetPlayer(i)==who){
			if (MyMoveRequestStore[i].time!=ltime){
				MyMoveRequestStore[i].time=ltime;
				MyMoveRequestStore[i].numMReqsThisSec=0;
				//MyMoveRequestStore[i].x=GetStatus(who,core.X);
				//MyMoveRequestStore[i].y=GetStatus(who,core.Y);
				MyMoveRequestStore[i].xy=GetStatus(who,core.X)+GetStatus(who,core.Y);
				if(MyMoveRequestStore[i].mountHackCheck>5){
					MyMoveRequestStore[i].mountHackCheck =0;
				}
				else if(MyMoveRequestStore[i].mountHackCheck>0){
					MyMoveRequestStore[i].mountHackCheck = MyMoveRequestStore[i].mountHackCheck-1;				
				}
			}
			else{
				//if (GetStatus(who,core.X)!=MyMoveRequestStore[i].x || GetStatus(who,core.Y)!=MyMoveRequestStore[i].y){
				if (MyMoveRequestStore[i].xy != GetStatus(who,core.X)+GetStatus(who,core.Y)){
					MyMoveRequestStore[i].numMReqsThisSec++;
					MaxMoveRequestsPingModified = conf.MaxSimultaneousMoveRequests + (MyMoveRequestStore[i].pinglag/100);
					//if (MyMoveRequestStore[i].numMReqsThisSec>conf.MaxSimultaneousMoveRequests){
					if (MyMoveRequestStore[i].numMReqsThisSec>MaxMoveRequestsPingModified){
						SpeechPacket(Packet, ".__SpeedHackFix %s", core.CONCEALEDFIXPASSWORD);									
						MyMoveRequestStore[i].numMReqsThisSec=0; //so players/staff aint spammed
					}
					//if (!MyMoveRequestStore[i].mountHackCheck && MyMoveRequestStore[i].numMReqsThisSec>(conf.MaxSimultaneousMoveRequests/2)){
					if (!MyMoveRequestStore[i].mountHackCheck && MyMoveRequestStore[i].numMReqsThisSec>(MaxMoveRequestsPingModified/2)){
						MyMoveRequestStore[i].mountHackCheck =4; //do this once every 4 seconds of running				
						sprintf(scriptparams, ".__MountHackFix %s", core.CONCEALEDFIXPASSWORD);
						StartScript(who, scriptparams );
					}
				}
			}
			break;
		}
		
	}
	/*
		for (i=0; i<GetNumConnections(); i++){
			if (GetPlayer(i)==who){
				if (MyMoveRequestStore[i].time!=ltime){
					MyMoveRequestStore[i].time=ltime;
					MyMoveRequestStore[i].numMReqsThisSec=0;
					MyMoveRequestStore[i].xy=GetStatus(who,core.X)+GetStatus(who,core.Y);
				}
				else{
				if (GetStatus(who,core.X)+GetStatus(who,core.Y)!=MyMoveRequestStore[i].xy){
					MyMoveRequestStore[i].numMReqsThisSec++;
					if (MyMoveRequestStore[i].numMReqsThisSec>conf.MaxSimultaneousMoveRequests){
						//SpeechPacket(Packet, ".__SpeedHackFix %s", core.CONCEALEDFIXPASSWORD);									
						MyMoveRequestStore[i].numMReqsThisSec=0; //so players/staff aint spammed
						//Packet=CantMovePacket;
						//SpeechPacket(Packet, "Oops!");									
						
						//Pipe(who, Packet);
						sprintf(charname, "%s dropped for speedhacking!", GetName(who));
						TellOnlineStaff(charname);
						core.SendSysMessage(who, "You have been dropped for speedhacking!", 3, 33);
						Disconnect(who);
						//SpeechPacket(Packet, "You have been detected speedhacking");
					}
					}
				}
				break;
			}
		}
		*/
}



void SingleClickHook(DWORD who, BYTE *Packet)
{
	//single click hook to stop players detecting concealed staff with injection
	//staff work normally
	//all player single clicks are checked vs an array of staff serials
	//if a click is on a staff serial then an escript is run to see if
	//that staff member is concealed
	//otherwise normal
	//char buffer[20];
	if (GetStatus(who, core.CMDLEVEL)>=1){
		return;
	}
	if (FindStaffSerial(MAKESERIAL(Packet+1))){
		SpeechPacket(Packet, ".__ConcealedStaffFix 0x%.8X %d %s", MAKESERIAL(Packet+1),1, core.CONCEALEDFIXPASSWORD);
	}
	//Pipe(who, Packet); //Pass the Packet to POL so the name will still be displayed
}

void GetStatusHook(DWORD who, BYTE *Packet)
{
	//Used to stop getting status of concealed staff.
	if (GetStatus(who, core.CMDLEVEL)>=1){
		return;
	}
	if (FindStaffSerial(MAKESERIAL(Packet+6))){
		SpeechPacket(Packet, ".__ConcealedStaffFix 0x%.8X %d %s", MAKESERIAL(Packet+6),2, core.CONCEALEDFIXPASSWORD);
	}
	//Pipe(who, Packet); //Pass the Packet to POL so the name will still be displayed
}
void DoubleClickHook(DWORD who, BYTE *Packet)
{
	//single click hook to stop players detecting concealed staff with injection
	//staff work normally
	//all player single clicks are checked vs an array of staff serials
	//if a click is on a staff serial then an escript is run to see if
	//that staff member is concealed
	//otherwise normal
	
	//char buffer[20];
	if (GetStatus(who, core.CMDLEVEL)>=1){
		return;
	}
	if (FindStaffSerial(MAKESERIAL(Packet+1))){
		SpeechPacket(Packet, ".__ConcealedStaffFix 0x%.8X %d %s", MAKESERIAL(Packet+1), 0, core.CONCEALEDFIXPASSWORD);
	}
	//Pipe(who, Packet); //Pass the Packet to POL so the name will still be displayed
}

void AddStaffSerial (unsigned long queryserial){
	//StaffSerials=(StaffSerial *) realloc(StaffSerials, sizeof(StaffSerial)*(conf.StaffSerialCount+1));
    //memset(&StaffSerials[conf.StaffSerialCount], 0, sizeof(StaffSerials));
	if (conf.StaffSerialCount<conf.MaxStaffSerialCount){
		StaffSerials[conf.StaffSerialCount].staffserial=queryserial;
		conf.StaffSerialCount++;
	}
}

int FindStaffSerial (unsigned long queryserial){
	int i;
    //printf("conf.StaffSerialCount %d\n", conf.StaffSerialCount);
	for(i=0; i<conf.StaffSerialCount; i++)
      {
	    //printf("Does StaffSerial %d ==  queryserial %d  \n", StaffSerials[i].staffserial, queryserial);
        if(StaffSerials[i].staffserial == queryserial)
        {
			//we caught a player clicking on a staff serial
			//pass to POL script to see if staff is concealed
			//StartScript(who, ".SingleClick 0x%.8X", hookedSerial );
           return 1;
        }
     }
	 return 0;
}

void ListStaffSerials(){
	int i;
    printf("StaffSerials:\n"); 
	for(i=0; i<conf.StaffSerialCount; i++)
    {
		  printf(" %d\t", StaffSerials[i].staffserial);
	}
	printf("\nStaff Serial Count %d\n", conf.StaffSerialCount);
}


void Command_LoadPlugin(DWORD who, char *rest)
{
   char *res=LoadPlugin(rest);
   if(res)
      core.SendSysMessage(who, res, 3, 0x480);
   else
      core.SendSysMessage(who, "Done", 3, 0x480);
   return;
}

void Command_KillPlugin(DWORD who, char *rest)
{
   char *res=KillPlugin(rest);
   if(res)
      core.SendSysMessage(who, res, 3, 0x480);
   else
      core.SendSysMessage(who, "Done", 3, 0x480);
   return;
}

void Command_ListPlugins(DWORD who, char *rest)
{
   int i;
   for(i=0; i<PluginCount; i++)
      core.SendSysMessage(who, Plugins[i].Name, 3, 0x480);
   return;
}

void Command_ReloadPh(DWORD who, char *rest)
{
   ReloadConfig();
   SetupHooks();
   core.SendSysMessage(who, "Done", 3, 0x480);
   return;
}

void Command_ReloadPlugin(DWORD who, char *rest)
{
   char *res=ReloadPlugin(rest);
   if(res)
      core.SendSysMessage(who, res, 3, 0x480);
   else
      core.SendSysMessage(who, "Done", 3, 0x480);
   return;
}

void SearchItemFile(DWORD who, char *itemname)
{
	int z=0, j=0;
	char line[200];
	char currentserial[200];
	char name[200];
	char tempstr[450];
	
	int inItem=0;
	FILE *itemsfile, *outputfile;
	
	itemsfile=fopen("data/items.txt", "r");
	
	if(!itemsfile){
		printf( "Couldn't find 'data/items.txt'!\n");
		return;
	}
	outputfile=fopen("pkg/opt/searchreport/searchdata.txt", "w");
	if(!outputfile){
		printf( "Couldn't find 'pkg/opt/searchreport/searchdata.txt'!\n");
		fclose(itemsfile);
		return;
	}
	while(fgets(line, sizeof(line), itemsfile)){
		
		if (inItem==1){
			if(strstr(line, "Serial\t")||strstr(line, "Serial ")){
				strcpy(currentserial, line);
			}	
			if(strstr(line, "Name\t")||strstr(line, "Name ")){
				strcpy(name, line);
			}
			if(strstr(line, "}")){
				inItem=0;
				for(z=0; z<(int)strlen(name); z++)
					name[z]=tolower(name[z]);
				if (strstr(name, itemname)){
					z=7;
					while (z<(int)strlen(currentserial) && (currentserial[z]==' '|| currentserial[z]=='\t')){
						z++;
					}
					while (z<(int)strlen(currentserial)){
						tempstr[j]=currentserial[z++];
						j++;
					}
					tempstr[j]='\0';
					fputs(tempstr, outputfile);
					j=0;
					z=0;
					strcpy(currentserial, "");
					strcpy(name, "");
					strcpy(tempstr, "");

				}
			}
		}
		if(strstr(line, "{")){
			inItem=1;
		}
	}
	fclose(itemsfile);

	itemsfile=fopen("data/pcequip.txt", "r");
	
	if(!itemsfile){
		printf( "Couldn't find 'data/pcequip.txt'!\n");
		return;
	}

		while(fgets(line, sizeof(line), itemsfile)){
		
		if (inItem==1){
			if(strstr(line, "Serial\t")||strstr(line, "Serial ")){
				strcpy(currentserial, line);
			}	
			if(strstr(line, "Name\t")||strstr(line, "Name ")){
				strcpy(name, line);
			}
			if(strstr(line, "}")){
				inItem=0;
				for(z=0; z<(int)strlen(name); z++)
					name[z]=tolower(name[z]);
				if (strstr(name, itemname)){
					z=7;
					while (z<(int)strlen(currentserial) && (currentserial[z]==' '|| currentserial[z]=='\t')){
						z++;
					}
					while (z<(int)strlen(currentserial)){
						tempstr[j]=currentserial[z++];
						j++;
					}
					tempstr[j]='\0';
					fputs(tempstr, outputfile);
					j=0;
					z=0;
					strcpy(currentserial, "");
					strcpy(name, "");
					strcpy(tempstr, "");

				}
			}
		}
		if(strstr(line, "{")){
			inItem=1;
		}
	}

	fclose(itemsfile);

	itemsfile=fopen("data/storage.txt", "r");
	
	if(!itemsfile){
		printf( "Couldn't find 'data/storage.txt'!\n");
		return;
	}
		while(fgets(line, sizeof(line), itemsfile)){
		
		if (inItem==1){
			if(strstr(line, "Serial\t")||strstr(line, "Serial ")){
				strcpy(currentserial, line);
			}	
			if(strstr(line, "Name\t")||strstr(line, "Name ")){
				strcpy(name, line);
			}
			if(strstr(line, "}")){
				inItem=0;
				for(z=0; z<(int)strlen(name); z++)
					name[z]=tolower(name[z]);
				if (strstr(name, itemname)){
					z=7;
					while (z<(int)strlen(currentserial) && (currentserial[z]==' '|| currentserial[z]=='\t')){
						z++;
					}
					while (z<(int)strlen(currentserial)){
						tempstr[j]=currentserial[z++];
						j++;
					}
					tempstr[j]='\0';
					fputs(tempstr, outputfile);
					j=0;
					z=0;
					strcpy(currentserial, "");
					strcpy(name, "");
					strcpy(tempstr, "");

				}
			}
		}
		if(strstr(line, "{")){
			inItem=1;
		}
	}

	fclose(itemsfile);
	fclose(outputfile);
}

void SearchObjTypeFile(DWORD who, char *objtypequery)
{
	int z=0, j=0;
	char line[200];
	char currentserial[200];
	char objtype[200];
	char tempstr[450];
	char newline[10];
	
	int inItem=0;
	FILE *itemsfile, *outputfile;
	
	itemsfile=fopen("data/items.txt", "r");
	
	if(!itemsfile){
		printf( "Couldn't find 'data/items.txt'!\n");
		return;
	}
	outputfile=fopen("pkg/opt/searchreport/searchdata.txt", "w");
	if(!outputfile){
		printf( "Couldn't find 'pkg/opt/searchreport/searchdata.txt'!\n");
		fclose(itemsfile);
		return;
	}
	while(fgets(line, sizeof(line), itemsfile)){
		
		if (inItem==1){
			mysubstr(newline, line, 7);
			//if(strstr(line, "Serial\t")||strstr(line, "Serial ")){
			if(strstr(newline, "Serial")){
				strcpy(currentserial, line);
			}	
			//if(strstr(line, "ObjType\t")||strstr(line, "ObjType ")){
			if(strstr(newline, "ObjType")){
				strcpy(objtype, line);
			}
			//if(strstr(line, "}")){
			if(strstr(newline, "}")){
				inItem=0;
				for(z=0; z<(int)strlen(objtype); z++)
					objtype[z]=tolower(objtype[z]);
				if (strstr(objtype, objtypequery)){
					z=7;
					while (z<(int)strlen(currentserial) && (currentserial[z]==' '|| currentserial[z]=='\t')){
						z++;
					}
					while (z<(int)strlen(currentserial)){
						tempstr[j]=currentserial[z++];
						j++;
					}
					tempstr[j]='\0';
					fputs(tempstr, outputfile);
					j=0;
					z=0;
					strcpy(currentserial, "");
					strcpy(objtype, "");
					strcpy(tempstr, "");

				}
			}
		}
		//if(strstr(line, "{")){
		if(strstr(newline, "{")){
			inItem=1;
		}
	}
	fclose(itemsfile);

	itemsfile=fopen("data/pcequip.txt", "r");
	
	if(!itemsfile){
		printf( "Couldn't find 'data/pcequip.txt'!\n");
		return;
	}

	while(fgets(line, sizeof(line), itemsfile)){
		mysubstr(newline, line, 7);
		if (inItem==1){
			if(strstr(newline, "Serial")){
				strcpy(currentserial, line);
			}	
			if(strstr(newline, "ObjType")){
				strcpy(objtype, line);
			}
			if(strstr(newline, "}")){
				inItem=0;
				for(z=0; z<(int)strlen(objtype); z++)
					objtype[z]=tolower(objtype[z]);
				if (strstr(objtype, objtypequery)){
					z=7;
					while (z<(int)strlen(currentserial) && (currentserial[z]==' '|| currentserial[z]=='\t')){
						z++;
					}
					while (z<(int)strlen(currentserial)){
						tempstr[j]=currentserial[z++];
						j++;
					}
					tempstr[j]='\0';
					fputs(tempstr, outputfile);
					j=0;
					z=0;
					strcpy(currentserial, "");
					strcpy(objtype, "");
					strcpy(tempstr, "");

				}
			}
		}
		if(strstr(newline, "{")){
			inItem=1;
		}
	}

	fclose(itemsfile);

	itemsfile=fopen("data/storage.txt", "r");
	
	if(!itemsfile){
		printf( "Couldn't find 'data/storage.txt'!\n");
		return;
	}
	while(fgets(line, sizeof(line), itemsfile)){
		mysubstr(newline, line, 7);
		if (inItem==1){
			if(strstr(newline, "Serial")){
				strcpy(currentserial, line);
			}	
			if(strstr(newline, "ObjType")){
				strcpy(objtype, line);
			}
			if(strstr(newline, "}")){
				inItem=0;
				for(z=0; z<(int)strlen(objtype); z++)
					objtype[z]=tolower(objtype[z]);
				if (strstr(objtype, objtypequery)){
					z=7;
					while (z<(int)strlen(currentserial) && (currentserial[z]==' '|| currentserial[z]=='\t')){
						z++;
					}
					while (z<(int)strlen(currentserial)){
						tempstr[j]=currentserial[z++];
						j++;
					}
					tempstr[j]='\0';
					fputs(tempstr, outputfile);
					j=0;
					z=0;
					strcpy(currentserial, "");
					strcpy(objtype, "");
					strcpy(tempstr, "");

				}
			}
		}
		if(strstr(newline, "{")){
			inItem=1;
		}
	}

	fclose(itemsfile);
	fclose(outputfile);
}

void writeToStaffLogFile(char *accname, char *charname, char *logmsg){

	struct tm *today;
	__time64_t ltime;
    char tmpbuf[128];
	//char tmpbuf2[128];
	FILE *stafflogfile;
	char fname[100];
	char finalogmsg[300];

	if (accname==0||charname==0){
		return;
	}
	
	//if (!strlen(charname)){
	//	return;
	//}

	_time64( &ltime );
	today = _localtime64( &ltime );
	

    strftime( tmpbuf, 128, "_%d_%m_%y", today );
	//printf("writing to log file\n");
	sprintf(fname, "log/%s%s.txt\0", accname, tmpbuf);
	strftime( tmpbuf, 128, "%H:%M", today );
	sprintf(finalogmsg, "[%s %s]: %s\n", charname, tmpbuf, logmsg);
	stafflogfile= fopen(fname, "a+");
	if (stafflogfile!=NULL){
		//printf("putting to log file\n");
		fputs(finalogmsg, stafflogfile);
		fclose(stafflogfile);
	}
	else{
		printf("error writing to staff log file\n");
	}
}

void mysubstr (char *buffer, char *copiedfrom, int length){
	int i=0;
	int clen = (int)strlen(copiedfrom);
	while(i<length && i<clen){
		buffer[i]=copiedfrom[i];
		i++;
	}
	buffer[i]='\0';
}

int CheckPastMaxClientsForIP(DWORD who){
	char IP[50];
	int counter=0;
	int i;

	strcpy(IP, GetClientIP(who));

	for (i=0; i<GetNumConnections(); i++){
		if (!GetStatus(GetPlayer(i), core.CMDLEVEL)>=1 && strcmp(GetClientIP(GetPlayer(i)),IP)==0){
			counter++;
		}
	}
	if (counter>conf.MaxSimultaneousClientForSingleIP){
			return 1;
	}
	return 0;
}

void TellOnlineStaff(char *msg){

	int i;
	for (i=0; i<GetNumConnections(); i++){
		if (GetStatus(GetPlayer(i), core.CMDLEVEL)>=1){
			core.SendSysMessage(GetPlayer(i), msg, 3, 0x480);
		}
	}

}

void PingClientIPAddress(DWORD who)
//void PingClientIPAddress(void *who)
{
	int i=0;
	int ping = MyPing(GetClientIP(who));
	printf("ip %s lag %d\n", GetClientIP(who), ping);
	
	for (i=0; i<GetNumConnections(); i++){
		if (GetPlayer(i)==who){
			MyMoveRequestStore[i].pinglag=ping;
			//printf("%d %d\n", i, MyMoveRequestStore[i].pinglag);
			break;
		}
	}
}

int InitialisePinger(){
	WSADATA wsaData;                              // WSADATA
    int nRet;                                     // General use return code

// Dynamically load the ICMP.DLL
    hndlIcmp = LoadLibrary("ICMP.DLL");
    if (hndlIcmp == NULL) {
        //::MessageBox(NULL, "Could not load ICMP.DLL", "Error:", MB_OK);
        return 0;
    }
// Retrieve ICMP function pointers
    pIcmpCreateFile  = (HANDLE (WINAPI *)(void))
        GetProcAddress((HMODULE)hndlIcmp,"IcmpCreateFile");
    pIcmpCloseHandle = (BOOL (WINAPI *)(HANDLE))
        GetProcAddress((HMODULE)hndlIcmp,"IcmpCloseHandle");
    pIcmpSendEcho = (DWORD (WINAPI *)
        (HANDLE,DWORD,LPVOID,WORD,PIPINFO,LPVOID,DWORD,DWORD))
        GetProcAddress((HMODULE)hndlIcmp,"IcmpSendEcho");
// Check all the function pointers
    if (pIcmpCreateFile == NULL     ||
        pIcmpCloseHandle == NULL    ||
    pIcmpSendEcho == NULL) {
        //::MessageBox(NULL, "Error loading ICMP.DLL", "Error:", MB_OK);
        FreeLibrary((HMODULE)hndlIcmp);
        return 0;
    }

// Init WinSock
    nRet = WSAStartup(0x0101, &wsaData );
    if (nRet) {
//        ::MessageBox(NULL, "WSAStartup() error:", "Error:", MB_OK);
        WSACleanup();
        FreeLibrary((HMODULE)hndlIcmp);
        return 0;
    }
// Check WinSock version
    if (0x0101 != wsaData.wVersion) {
     //   ::MessageBox(NULL, "No WinSock version 1.1 support found", "Error:", MB_OK);
        WSACleanup();
        FreeLibrary((HMODULE)hndlIcmp);
        return 0;
    }
	return 1;
}

void ClosePinger(){
	if (!pingerActive){
		return;
	}

	WSACleanup();
    FreeLibrary((HMODULE)hndlIcmp);
}

//int MyPing(char* strHost, ICMPECHO &icmpEcho) {
int MyPing(char* strHost) {

	
    ICMPECHO icmpEcho;
	struct in_addr iaDest;                        // Internet address structure
    LPHOSTENT pHost;                              // Pointer to host entry structure
    DWORD *dwAddress;                             // IP Address
    IPINFO ipInfo;                                // IP Options structure
//    ICMPECHO icmpEcho;                            // ICMP Echo reply buffer
    HANDLE hndlFile;                              // Handle for IcmpCreateFile()
	
	if (!pingerActive){
		return 0;
	}
// Lookup destination
// Use inet_addr() to determine if we're dealing with a name
// or an address
    iaDest.s_addr = inet_addr(strHost);
    if (iaDest.s_addr == INADDR_NONE)
        pHost = gethostbyname(strHost);
    else
        pHost = gethostbyaddr((const char *)&iaDest,
            sizeof(struct in_addr), AF_INET);
    if (pHost == NULL) {
        return 0;
    }

// Copy the IP address
    dwAddress = (DWORD *)(*pHost->h_addr_list);

// Get an ICMP echo request handle
    hndlFile = pIcmpCreateFile();

// Set some reasonable default values
    ipInfo.Ttl = 255;
    ipInfo.Tos = 0;
    ipInfo.IPFlags = 0;
    ipInfo.OptSize = 0;
    ipInfo.Options = NULL;
    //icmpEcho.Status = 0;
	icmpEcho.Status = 0;
// Reqest an ICMP echo
    pIcmpSendEcho(
        hndlFile,                                 // Handle from IcmpCreateFile()
        *dwAddress,                               // Destination IP address
        NULL,                                     // Pointer to buffer to send
        0,                                        // Size of buffer in bytes
        &ipInfo,                                  // Request options
        &icmpEcho,                                // Reply buffer
        sizeof(struct tagICMPECHO),
        1000);                                    // Time to wait in milliseconds
// Print the results
    //iaDest.s_addr = icmpEcho.Source;
	iaDest.s_addr = icmpEcho.Source;
    if (icmpEcho.Status) {
        return 0;
    }

// Close the echo request file handle
    pIcmpCloseHandle(hndlFile);
    return icmpEcho.RTTime;
}

