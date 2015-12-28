/* Folko's ConfigParser for POLHook (C) Folko <Folko@elitecoder.net> */

/*
    <CVS SERVER GENERATED>
    This project was stored on Folko's CVS server as $Source: /home/cvs/POLHookSolution/POLHook/Config.c,v $.

    Id tag follows (<filename> <revision> <last changed> <author> <status>):
    $Id: Config.c,v 1.6 2003/06/21 13:38:43 folko23 Exp $

    If you want to report any errors, please include this tag.
    </GENERATED>
*/

#include "POLHook.h"

Config conf;
Trigger *Triggers;
StaffSerial *StaffSerials;
MoveRequestStore *MyMoveRequestStore;

DWORD GetTimeStamp(unsigned char *image)
{
   IMAGE_DOS_HEADER *doshdr=(IMAGE_DOS_HEADER*)image;
   IMAGE_FILE_HEADER *filehdr=(IMAGE_FILE_HEADER*)(image+doshdr->e_lfanew+sizeof(IMAGE_NT_SIGNATURE));
   return filehdr->TimeDateStamp;
}

char *ReadConfig(void)
{
   FILE *cfg, *pol;
   DWORD timestamp=0;
   char POLFile[MAX_PATH];
   BYTE *POLImage;
   DWORD ImageSize=0;

   cfg=fopen("POLHook.cfg", "r");
   if(!cfg) return "Couldn't find 'POLHook.cfg'!";

   GetModuleFileName(NULL, POLFile, MAX_PATH);
   pol=fopen(POLFile, "rb");
   if(!pol) { fclose(cfg); return "Couldn't open POL!"; }

   fseek(pol, 0, SEEK_END);
   ImageSize=ftell(pol);
   rewind(pol);
   POLImage=(BYTE *)malloc(ImageSize);
   fread(POLImage, sizeof(BYTE), ImageSize, pol);
   fclose(pol);
   timestamp=GetTimeStamp(POLImage);
   free(POLImage);
   if(!timestamp) { fclose(cfg); return "Couldn't get POL's timestamp!"; }

   memset(&conf, 0, sizeof(conf));
   GetConfigString(cfg, "LogPath", conf.LogPath, 0);
   conf.AllowSpeechMul=(int)GetConfigInt(cfg, "AllowSpeechMul", 0);
   conf.EnableTriggers=(int)GetConfigInt(cfg, "EnableTriggers", 0);
   conf.HookCrypt=(int)GetConfigInt(cfg, "HookCrypt", 0);
   conf.MaxCmdLevel=(int)GetConfigInt(cfg, "MaxCmdLevel", 0);
   //barns
   conf.MaxSimultaneousClientForSingleIP=(int)GetConfigInt(cfg,"MaxSimultaneousClientForSingleIP",0);
   conf.MaxSimultaneousMoveRequests=(int)GetConfigInt(cfg,"MaxSimultaneousMoveRequests",0);
   conf.MaxStaffSerialCount=(int)GetConfigInt(cfg,"MaxStaffAllowed",0);
   printf("MaxStaffAllowed %d\n", conf.MaxStaffSerialCount);
   conf.MoveHookEnabled=(int)GetConfigInt(cfg,"MoveHookEnabled",0);
   conf.AOSCharProfileHookEnabled=(int)GetConfigInt(cfg,"AOSCharProfileHookEnabled",0);
   conf.GodModeHookEnabled=(int)GetConfigInt(cfg,"GodModeHookEnabled",0);
   conf.SingleClickHookEnabled=(int)GetConfigInt(cfg,"SingleClickHookEnabled",0);
   conf.DoubleClickHookEnabled=(int)GetConfigInt(cfg,"DoubleClickHookEnabled",0);
   conf.GetStatusHookEnabled=(int)GetConfigInt(cfg,"GetStatusHookEnabled",0);
   conf.AttackHookEnabled=(int)GetConfigInt(cfg,"AttackHookEnabled",0);
   conf.ClientVersionHookEnabled=(int)GetConfigInt(cfg,"ClientVersionHookEnabled",0);
   conf.TargetHookEnabled=(int)GetConfigInt(cfg,"TargetHookEnabled",0);
   conf.AccountLoginHookEnabled=(int)GetConfigInt(cfg,"AccountLoginHookEnabled",0);
   conf.EnablePinger=(int)GetConfigInt(cfg,"EnablePinger",0);

   StaffSerials=(StaffSerial *) realloc(StaffSerials, sizeof(StaffSerial)*(conf.MaxStaffSerialCount+1));
   memset(&StaffSerials[conf.MaxStaffSerialCount], 0, sizeof(StaffSerials));
   conf.MaxOnLinePlayers=(int)GetConfigInt(cfg,"MaxOnLinePlayers",0);
   printf("MaxOnLinePlayers %d\n", conf.MaxOnLinePlayers);
   MyMoveRequestStore=0;
   MyMoveRequestStore=(MoveRequestStore *) realloc(MyMoveRequestStore, sizeof(MoveRequestStore) * (conf.MaxOnLinePlayers+1));
   memset(&core, 0, sizeof(core));
   core.TimeStamp=timestamp;
   GetConfigString(cfg, "CORENAME", core.CORENAME, timestamp);
   GetConfigString(cfg, "CONCEALEDFIXPASSWORD", core.CONCEALEDFIXPASSWORD, timestamp);
   core.CLIENT_START=GetConfigInt(cfg, "CLIENT_START", timestamp);
   core.PACKET_TABLE_ADDRESS=GetConfigInt(cfg, "PACKET_TABLE_ADDRESS", timestamp);
   core.PACKET_HANDLER_ADDRESS=GetConfigInt(cfg, "PACKET_HANDLER_ADDRESS", timestamp);
   core.IGNITION_HANDLER=GetConfigInt(cfg, "IGNITION_HANDLER", timestamp);
   core.ORIGINAL_PACKETHANDLER=GetConfigInt(cfg, "ORIGINAL_PACKETHANDLER", timestamp);
   conf.UniCode=GetConfigInt(cfg, "UNICODE", timestamp);
   core.IP=(WORD)GetConfigInt(cfg, "IP", timestamp);
   core.ACCOUNT=(WORD)GetConfigInt(cfg, "ACCOUNT", timestamp);
   core.X=(WORD)GetConfigInt(cfg, "X", timestamp);
   core.Y=(WORD)GetConfigInt(cfg, "Y", timestamp);
   core.NAME=(WORD)GetConfigInt(cfg, "NAME", timestamp);
   core.CMDLEVEL=(WORD)GetConfigInt(cfg, "CMDLEVEL", timestamp);
   core.DEAD=(WORD)GetConfigInt(cfg, "DEAD", timestamp);
   core.HIDDEN=(WORD)GetConfigInt(cfg, "HIDDEN", timestamp);
   core.CONCEALED=(WORD)GetConfigInt(cfg, "CONCEALED", timestamp);
   core.FROZEN=(WORD)GetConfigInt(cfg, "FROZEN", timestamp);
   core.PARALYZED=(WORD)GetConfigInt(cfg, "PARALYZED", timestamp);
   core.SQUELCHED=(WORD)GetConfigInt(cfg, "SQUELCHED", timestamp);
   core.StartScript=GetConfigInt(cfg, "StartScript", timestamp);
   core.SendSysMessage=(_SendSysMessage)(LRESULT)GetConfigInt(cfg, "SendSysMessage", timestamp);
   core.SendPacket=(_SendPacket)(LRESULT)GetConfigInt(cfg, "SendPacket", timestamp);
   if(!strlen(core.CORENAME))
   {
      static char error[120];
      sprintf(error, "Couldn't find 'CORE %X' in POLHook.cfg!", timestamp);
      fclose(cfg);
      return error;
   }
   //StaffSerials=0;

   //conf.StaffSerialCount=ParseStaffSerials(cfg);

   if(conf.EnableTriggers)
      conf.TriggerCount=ParseTriggers(cfg);
   else
      conf.TriggerCount=0;

   fclose(cfg);
   return 0;
}

void ReloadConfig(void)
{
   FILE *cfg;

   cfg=fopen("POLHook.cfg", "r");
   if(!cfg) return;

   conf.AllowSpeechMul=(int)GetConfigInt(cfg, "AllowSpeechMul", 0);
   conf.EnableTriggers=(int)GetConfigInt(cfg, "EnableTriggers", 0);
   conf.MaxCmdLevel=(int)GetConfigInt(cfg, "MaxCmdLevel", 0);
   conf.MaxSimultaneousClientForSingleIP=(int)GetConfigInt(cfg,"MaxSimultaneousClientForSingleIP",0);
   conf.MaxSimultaneousMoveRequests=(int)GetConfigInt(cfg,"MaxSimultaneousMoveRequests",0);
   conf.MoveHookEnabled=(int)GetConfigInt(cfg,"MoveHookEnabled",0);
   conf.AOSCharProfileHookEnabled=(int)GetConfigInt(cfg,"AOSCharProfileHookEnabled",0);
   conf.GodModeHookEnabled=(int)GetConfigInt(cfg,"GodModeHookEnabled",0);
   conf.SingleClickHookEnabled=(int)GetConfigInt(cfg,"SingleClickHookEnabled",0);
   conf.DoubleClickHookEnabled=(int)GetConfigInt(cfg,"DoubleClickHookEnabled",0);
   conf.GetStatusHookEnabled=(int)GetConfigInt(cfg,"GetStatusHookEnabled",0);
   conf.AttackHookEnabled=(int)GetConfigInt(cfg,"AttackHookEnabled",0);
   conf.ClientVersionHookEnabled=(int)GetConfigInt(cfg,"ClientVersionHookEnabled",0);
   conf.TargetHookEnabled=(int)GetConfigInt(cfg,"TargetHookEnabled",0);
   conf.AccountLoginHookEnabled=(int)GetConfigInt(cfg,"AccountLoginHookEnabled",0);
   if(Triggers)
      free(Triggers);

   if(conf.EnableTriggers)
      conf.TriggerCount=ParseTriggers(cfg);
   else
      conf.TriggerCount=0;
	
   printf("POLHook.cfg reloaded. Note that MaxOnlinePlayers and MaxStaff requires reboot to reload.\n");
   fclose(cfg);
   return;
}

int ParseTriggers(FILE *cfg)
{
   int count=0, intrigger=0, incomment=0;
   char line[200];
   char *found;

   Triggers=0;
   rewind(cfg);
   while(fgets(line, sizeof(line), cfg))
   {
      if(found=strstr(line, "//"))
         found[0]='\0';
      if(strstr(line, "/*"))
         incomment=1;

      if(!incomment)
      {
         if(strstr(line, "TRIGGER"))
         {
            Triggers=(Trigger *) realloc(Triggers, sizeof(Trigger)*(count+1));
            memset(&Triggers[count], 0, sizeof(Trigger));
            intrigger=1;
         }
         if(found=strstr(line, "Phrase")) {
            int i=0;
            if(!strchr(found, '='))
               continue;
            found=strchr(line, '"')+1;
            while(found[i++]!='"') Triggers[count].phrase[i-1]=found[i-1];
            Triggers[count].phrase[i-1]='\0';
         }else if(found=strstr(line, "Response")) {
            int i=0;
            if(!strchr(found, '='))
               continue;
            found=strchr(line, '"')+1;
            while(found[i++]!='"') Triggers[count].resp[i-1]=found[i-1];
            Triggers[count].resp[i-1]='\0';
         }else if(found=strstr(line, "Display")) {
            if(!strchr(found, '='))
               continue;
            found=strchr(line, '=')+1;
            sscanf(found, "%d", &Triggers[count].display);
         }
      }
      if(strchr(line, '}') && intrigger)
      {
         count++;
         intrigger=0;
      }
      if(strstr(line, "*/"))
         incomment=0;
   }
   if(!count)
      free(Triggers);
   return count;
}

//ParseStaffSerials(cfg);
int ParseStaffSerials(FILE *cfg)
{
  
   int count=0, instaff=0, incomment=0;
   char line[200];
   char *found;
   char *stopstr;

   StaffSerials=0;
   rewind(cfg);
   printf("Loading Staff Serials from %s\n", "POLHook.cfg");
   while(fgets(line, sizeof(line), cfg))
   {
      if(found=strstr(line, "//"))
         found[0]='\0';
      if(strstr(line, "/*"))
         incomment=1;

      if(!incomment)
      {
        if(strstr(line, "STAFF"))
        {
            StaffSerials=(StaffSerial *) realloc(StaffSerials, sizeof(StaffSerial)*(count+1));
            memset(&StaffSerials[count], 0, sizeof(StaffSerials));
            instaff=1;
        }
		else if(found=strstr(line, "Serial")) {
		    char tempserial[100];
			char *convstr;
            int i=0;
            if(!strchr(found, '='))
               continue;
            found=strchr(line, '=')+1;
			while(found[i++]!=';') {
				tempserial[i-1]=found[i];
			}
			tempserial[i]='\0';
			convstr= tempserial;
			StaffSerials[count].staffserial = strtoul( convstr, &stopstr, 16 );
		}
      }
      if(strchr(line, '}') && instaff)
      {
         count++;
         instaff=0;
      }
      if(strstr(line, "*/"))
         incomment=0;
   }
   if(!count)
      free(StaffSerials);
   return count;
}



void GetConfigString(FILE *cfg, char *name, char *dest, DWORD timestamp)
{
   char line[200];
   char result[100];
   char *found;
   char time[25];
   int incore=0, incomment=0;

   sprintf(time, "CORE %X", timestamp);
   rewind(cfg);
   while(fgets(line, sizeof(line), cfg))
   {
      if(found=strstr(line, "//"))
         found[0]='\0';
      if(strstr(line, "/*"))
         incomment=1;
      if(strstr(line, time) || !timestamp)
         incore=1;

      if((found=strstr(line, name)) && !incomment && incore)
      {
         int i=0;
         if(!strchr(found, '='))
            continue;
         found=strchr(line, '"')+1;
         while(found[i++]!='"') result[i-1]=found[i-1];
         result[i-1]='\0';
         strcpy(dest, result);
         return;
      }
      if(strchr(line, '}') && incore && !incomment && timestamp)
         incore=0;
      if(strstr(line, "*/"))
         incomment=0;
   }
   return;
}

DWORD GetConfigInt(FILE *cfg, char *name, DWORD timestamp)
{
   char line[200];
   char *found;
   char time[25];
   int incore=0, incomment=0;
   DWORD result=0;

   sprintf(time, "CORE %X", timestamp);
   rewind(cfg);
   while(fgets(line, sizeof(line), cfg))
   {
      if(found=strstr(line, "//"))
         found[0]='\0';
      if(strstr(line, "/*"))
         incomment=1;

      if(strstr(line, time) || !timestamp)
         incore=1;
      if((found=strstr(line, name)) && incore && !incomment)
      {
         int num=0;
         if(!strchr(found, '='))
            continue;
         found=strchr(line, '=')+1;
         if(strstr(found, "0x"))
            num=sscanf(found, "%X", &result);
         else
            num=sscanf(found, "%lu", &result);
         if(num==1)
            return result;
      }
      if(strchr(line, '}') && incore && !incomment && timestamp)
         incore=0;
      if(strstr(line, "*/"))
         incomment=0;
   }
   return 0;
}



