/* Folko's Plugin Engine (C) Folko, 2003 */

/*
    <CVS SERVER GENERATED>
    This project was stored on Folko's CVS server as $Source: /home/cvs/POLHookSolution/POLHook/Plugins.c,v $.
    
    Id tag follows (<filename> <revision> <last changed> <author> <status>):
    $Id: Plugins.c,v 1.9 2003/06/21 13:38:43 folko23 Exp $
    
    If you want to report any errors, please include this tag.
    </GENERATED>
*/
#include "POLHook.h"

int PluginCount=0;
PluginInfo Plugins[MAX_PLUGINS];

void LoadPlugins(void)
{
   WIN32_FIND_DATA files;
   HANDLE found;
   
   PluginCount=0;
   memset(&files, 0, sizeof(files));

   found=FindFirstFile("POLHook\\*.dll", &files);
   if(found==INVALID_HANDLE_VALUE)
      return;
   do{
      char *res=LoadPlugin(files.cFileName);
      if(res && stricmp(files.cFileName, "POLHook.DLL")!=0)
         printf("%s failed to load: %s.\n", files.cFileName, res);
   }while( FindNextFile(found, &files) );
   FindClose(found);
}

char *LoadPlugin(char *name)
{
   int i;
   HMODULE plugin;
   int (*start) (int, PluginData, void (*HookChain[255]) (DWORD, BYTE *) );
   char fullname[MAX_PATH];
   PluginData data;

   for(i=0; i<(int)strlen(name); i++)
      name[i]=tolower(name[i]);

   for(i=0; i<PluginCount; i++)
   {
      if(strncmp(Plugins[i].Name, name, strlen(Plugins[i].Name))==0)
         return "That plugin is already loaded.";
   }
   strcpy(fullname, name);
   if(!strchr(name, '\\'))
   {
      strcpy(fullname, "POLHook\\");
      strcat(fullname, name);
   }
   plugin=LoadLibrary(fullname);
   if(!plugin)
      return "Couldn't load that plugin.";
   start=(void *)GetProcAddress(plugin, "POLHookPlugin");
   if(!start)
      return "That is not a valid POLHook plugin.";

   data.core=core;
   data.Uptime=conf.Uptime;
   data.AddPacket=AddPacket;
   data.Pipe=Pipe;
   data.SpeechPacket=SpeechPacket;
   data.Disconnect=Disconnect;
   data.GetAccount=GetAccount;
   data.GetClientIP=GetClientIP;
   data.GetName=GetName;
   data.GetNumConnections=GetNumConnections;
   data.GetPlayer=GetPlayer;
   data.GetSocket=GetSocket;
   data.GetStatus=GetStatus;
   data.StartScript=StartScript;
   if(!start(PLUGIN_VERSION, data, HookChain))
      return "That plugin is incompatible with the current POLHook version.";
   Plugins[PluginCount].DLL=plugin;

   for(i=0; i<(int)strlen(name); i++)
      if(name[i]=='.')
         name[i]='\0';
   strcpy(Plugins[PluginCount].Name, name);
   PluginCount++;
   return 0;
}

char *KillPlugin(char *name)
{
   int i, j;
   void *(*kill) (void);

   for(i=0; i<PluginCount; i++)
      if(strncmp(Plugins[i].Name, name, strlen(Plugins[i].Name))==0)
      {
         kill=(void*)GetProcAddress(Plugins[i].DLL, "KillPlugin");
         if(kill)
            kill();
         else
            return "That plugin is not killable.";
         FreeLibrary(Plugins[i].DLL);
         PluginCount--;
         for(j=i; j<PluginCount; j++)
            Plugins[j]=Plugins[j+1];
         return 0;
      }
   return "Couldn't find that plugin.";
}

char *ReloadPlugin(char *name)
{
   int i;
   void *(*reload) (void);

   for(i=0; i<PluginCount; i++)
      if(strncmp(Plugins[i].Name, name, strlen(Plugins[i].Name))==0)
      {
         reload=(void*)GetProcAddress(Plugins[i].DLL, "ReloadPlugin");
         if(reload)
            reload();
         else
            return "That plugin is not reloadable.";
         return 0;
      }
   return "Couldn't find that plugin.";
}
