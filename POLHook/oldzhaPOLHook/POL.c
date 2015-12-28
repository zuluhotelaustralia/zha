/* POL.c (C) Folko, 2003 */

/*
    <CVS SERVER GENERATED>
    This project was stored on Folko's CVS server as $Source: /home/cvs/POLHookSolution/POLHook/POL.c,v $.
    
    Id tag follows (<filename> <revision> <last changed> <author> <status>):
    $Id: POL.c,v 1.6 2003/07/08 02:56:17 folko23 Exp $
    
    If you want to report any errors, please include this tag.
    </GENERATED>
*/

#include "POLHook.h"

/* This pragma is only supported by MS compilers
 * Luckily enough, only MS compilers actually generate this silly warning.
 */
#pragma warning(disable : 4311)
#pragma warning(disable : 4312)

POLCore core;

void Pipe(DWORD who, BYTE *Packet)
{
   DWORD address = *(DWORD *) (Packet[0] * 8 + core.PACKET_HANDLER_ADDRESS);
   _PacketHandler PacketHandler = (_PacketHandler)address;
   PacketHandler(who, Packet);
}

DWORD GetWhoSerial(DWORD who) 
{ 

   DWORD value; 
   DWORD handle = *(DWORD *) ( who + 8); 

   if (!handle) 
      return -1; 
   value = *(DWORD *) (handle + 0x8); 
   value &= 0xFFFFFFFF; 
   return value; 
   
}

void SpeechPacket(BYTE *Packet, const char *Command, ...)
{
   BYTE SpeechTemplate[4096] = { 0x03, 0x00, 0x00, 0x00, 0x00, 0x85, 0x00, 0x03, }; /* Default speech packet */
	char cmdbuf[4096];
   WORD Size;
   va_list args;

   va_start(args, Command);
	vsprintf(cmdbuf, Command, args);
	va_end(args);

   Size=(WORD) strlen(cmdbuf) + 1;
   SpeechTemplate[1] = (BYTE) HIWORD(Size + 8);
   SpeechTemplate[2] = (BYTE) LOWORD(Size + 8);
   memcpy(SpeechTemplate + 8, cmdbuf, Size);
   memcpy(Packet, SpeechTemplate, Size + 9);
}

void AddPacket(BYTE PacketID, DWORD Leng)
{
   DWORD *address = (DWORD *) (PacketID * 8 + core.PACKET_TABLE_ADDRESS);
   *address = Leng;
}

long GetNumConnections(void)
{
   DWORD start = *(DWORD *) (core.CLIENT_START);
   DWORD end = *(DWORD *) (core.CLIENT_START + 4);
   return (end - start) / 4;
}

DWORD GetPlayer(long index)
{
   if (index >= GetNumConnections()) /* would cause a bad access violation */
      return 0;
   return *(DWORD *) (*(DWORD *) (core.CLIENT_START) + index * 4); /* ~KISS */
}

char *GetClientIP(DWORD who)
{
   struct sockaddr_in client;
   int len = 255;

   getpeername(GetSocket(who), (struct sockaddr *) &client, &len);
   return inet_ntoa(client.sin_addr);
}



SOCKET GetSocket(DWORD who)
{
   SOCKET s = *(SOCKET *) (who + core.IP);
   return s;
}

void Disconnect(DWORD who)
{
   closesocket(GetSocket(who));
}

DWORD GetStatus(DWORD who, const WORD Status)
{
   DWORD value;
   DWORD handle = *(DWORD *) ( who + 8);

   if (!handle)
      return -1;
   value = *(DWORD *) (handle + Status);
   if (Status == core.X || Status == core.Y) /* WORD's */
      value &= 0x0000FFFF;
   else                            /* BYTE's */
      value &= 0x000000FF;
   return value;
}

char *GetName(DWORD who)
{
   long handle = *(DWORD *) (who + 8);
   if (!handle)
      return 0;
   return *(char **) (handle + core.NAME);
}

char *GetAccount(DWORD who)
{
   DWORD handle = *(DWORD *) (who + 4);
   if (!handle)
      return 0;
   return *(char **) (handle + core.ACCOUNT);
}

void HookCore(void *hookaddress)
{
   BYTE CallNewStub[]=
   {
      0xE8, 0x00, 0x00, 0x00, 0x00, /* call NewStub */
      0x90,                         /* nop */
      0x90                          /* nop */
   };
   PutD(CallNewStub+1, CalcCall(core.ORIGINAL_PACKETHANDLER, (DWORD)hookaddress));
   WriteProcessMemory(GetCurrentProcess(), (void*)(core.ORIGINAL_PACKETHANDLER), &CallNewStub, sizeof(CallNewStub), 0);
}

void HookGameCrypt(void *newcryptaddress)
{
   if(core.IGNITION_HANDLER)
   {
      if(strstr(core.CORENAME, "095")) {
         BYTE newstub[4];

         PutD(newstub, (DWORD)newcryptaddress);
         WriteProcessMemory(GetCurrentProcess(), (void*)(core.IGNITION_HANDLER), &newstub, sizeof(newstub), 0);
      } else {
         BYTE newstub[5];

         ReadProcessMemory(GetCurrentProcess(), (void*)(core.IGNITION_HANDLER), &newstub, sizeof(newstub), 0);
         PutD(newstub+1, CalcCall(core.IGNITION_HANDLER, (DWORD)newcryptaddress));
         WriteProcessMemory(GetCurrentProcess(), (void*)(core.IGNITION_HANDLER), &newstub, sizeof(newstub), 0);
      }
   }
}

/* Couldn't use a macro for this because it was too long for LCC.. */
void PutD(BYTE *Buffer, DWORD val)
{
   *(Buffer+0)=((BYTE)(val>>0)&0xFF);
   *(Buffer+1)=((BYTE)(val>>8)&0xFF);
   *(Buffer+2)=((BYTE)(val>>16)&0xFF);
   *(Buffer+3)=((BYTE)(val>>24)&0xFF);
}

void AddTextCommand(char *phrase, void (*handler) (DWORD, char *) )
{
   strcpy(Commands[CommandCount].phrase, phrase);
   Commands[CommandCount].Handler=handler;
   CommandCount++;
}

void StartScript(DWORD who, char *script, ...)
{
   typedef void (*pUCStartScript) (DWORD, char *script, WCHAR *uctext, char lang[3]);
   typedef void (*pStartScript) (DWORD, char *script);

   pUCStartScript _UCStartScript=(pUCStartScript)core.StartScript;
   pStartScript _StartScript=(pStartScript)core.StartScript;
	char sbuf[16384]; /* POL's buffer * 2 + alignment */
   va_list args;

   va_start(args, script);
	vsprintf(sbuf, script, args);
	va_end(args);

   if(conf.UniCode)
      _UCStartScript(who, sbuf, 0, 0);
   else
      _StartScript(who, sbuf);
   return;
}



/*
CPing::CPing() {
    bValid = false;
    WSADATA wsaData;                              // WSADATA
    int nRet;                                     // General use return code

// Dynamically load the ICMP.DLL
    hndlIcmp = LoadLibrary("ICMP.DLL");
    if (hndlIcmp == NULL) {
        ::MessageBox(NULL, "Could not load ICMP.DLL", "Error:", MB_OK);
        return;
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
        ::MessageBox(NULL, "Error loading ICMP.DLL", "Error:", MB_OK);
        FreeLibrary((HMODULE)hndlIcmp);
        return;
    }

// Init WinSock
    nRet = WSAStartup(0x0101, &wsaData );
    if (nRet) {
        ::MessageBox(NULL, "WSAStartup() error:", "Error:", MB_OK);
        WSACleanup();
        FreeLibrary((HMODULE)hndlIcmp);
        return;
    }
// Check WinSock version
    if (0x0101 != wsaData.wVersion) {
        ::MessageBox(NULL, "No WinSock version 1.1 support found", "Error:", MB_OK);
        WSACleanup();
        FreeLibrary((HMODULE)hndlIcmp);
        return;
    }
    bValid = true;
}


CPing::~CPing() {
    WSACleanup();
    FreeLibrary((HMODULE)hndlIcmp);
}


bool CPing::Ping(char* strHost, ICMPECHO &icmpEcho) {
    struct in_addr iaDest;                        // Internet address structure
    LPHOSTENT pHost;                              // Pointer to host entry structure
    DWORD *dwAddress;                             // IP Address
    IPINFO ipInfo;                                // IP Options structure
//    ICMPECHO icmpEcho;                            // ICMP Echo reply buffer
    HANDLE hndlFile;                              // Handle for IcmpCreateFile()

    if(!bValid) {
        return false;
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
        return false;
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
    iaDest.s_addr = icmpEcho.Source;
    if (icmpEcho.Status) {
        return false;
    }

// Close the echo request file handle
    pIcmpCloseHandle(hndlFile);
    return true;
}
*/



