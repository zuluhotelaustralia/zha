/* POL.h (C) Folko, 2003 */

/*
    <CVS SERVER GENERATED>
    This project was stored on Folko's CVS server as $Source: /home/cvs/POLHookSolution/POLHook/POL.h,v $.
    
    Id tag follows (<filename> <revision> <last changed> <author> <status>):
    $Id: POL.h,v 1.4 2003/06/21 13:38:43 folko23 Exp $
    
    If you want to report any errors, please include this tag.
    </GENERATED>
*/

#ifndef _POL_H_INCLUDED
#define _POL_H_INCLUDED

//FROM CPING
typedef struct tagIPINFO{
	u_char Ttl;				// Time To Live
	u_char Tos;				// Type Of Service
	u_char IPFlags;			// IP flags
	u_char OptSize;			// Size of options data
	u_char *Options;	// Options data buffer
}IPINFO;

typedef IPINFO* PIPINFO;

typedef struct tagICMPECHO{
	u_long Source;			// Source address
	u_long Status;			// IP status
	u_long RTTime;			// Round trip time in milliseconds
	u_short DataSize;		// Reply data size
	u_short Reserved;		// Unknown
	void *pData;		// Reply data buffer
	IPINFO	ipInfo;			// Reply options
}ICMPECHO;

typedef ICMPECHO* PICMPECHO;

	// ICMP.DLL Export Function Pointers
HANDLE (WINAPI *pIcmpCreateFile)(VOID);
int (WINAPI *pIcmpCloseHandle)(HANDLE);
DWORD (WINAPI *pIcmpSendEcho)(HANDLE,DWORD,LPVOID,WORD,PIPINFO,LPVOID,DWORD,DWORD);
HANDLE hndlIcmp;// LoadLibrary() handle to ICMP.DLL

typedef void (*_SendSysMessage) (DWORD who, char *text, WORD font, WORD color);
typedef void (*_SendPacket) (DWORD who, BYTE *Packet, WORD len);
typedef void (*_PacketHandler) (DWORD, BYTE *);

#define MAKESERIAL(Buffer) (*(Buffer)<<24|*(Buffer+1)<<16|*(Buffer+2)<<8|*(Buffer+3))
#define MAKEWORDPARAM(Buffer) (*(Buffer)<<8|*(Buffer+1))

/* Calculate the DWORD needed to do a call */
#define CalcCall(from, to) ((to)-(from)-5)

typedef struct
{
	char phrase[100];
   void (*Handler) (DWORD, char *);
}TextCommand;

extern TextCommand Commands[20];
extern int CommandCount;

/* Store the big endian DWORD in val as a little endian DWORD in Buffer */
void PutD(BYTE *Buffer, DWORD val);

void HookGameCrypt(void *newcryptaddress);
void HookCore(void *hookaddress);
void FillVars(void);
void AddTextCommand(char *phrase, void (*handler) (DWORD, char *) );


void Pipe(DWORD who, BYTE *Packet);
void SpeechPacket(BYTE *Packet, const char *Command, ...);
DWORD GetWhoSerial(DWORD who); 
void AddPacket(BYTE PacketID, DWORD Leng);
long GetNumConnections(void);
DWORD GetPlayer(long index);
char *GetClientIP(DWORD who);
//DWORD GetSocketName(DWORD who);
SOCKET GetSocket(DWORD who);
void Disconnect(DWORD who);
DWORD GetStatus(DWORD who, const WORD Status);
char *GetName(DWORD who);
char *GetAccount(DWORD who);
void StartScript(DWORD who, char *script, ...);


/*
//FROM CPING
typedef struct tagIPINFO{
	u_char Ttl;				// Time To Live
	u_char Tos;				// Type Of Service
	u_char IPFlags;			// IP flags
	u_char OptSize;			// Size of options data
	u_char *Options;	// Options data buffer
}IPINFO;

typedef IPINFO* PIPINFO;

typedef struct tagICMPECHO{
	u_long Source;			// Source address
	u_long Status;			// IP status
	u_long RTTime;			// Round trip time in milliseconds
	u_short DataSize;		// Reply data size
	u_short Reserved;		// Unknown
	void *pData;		// Reply data buffer
	IPINFO	ipInfo;			// Reply options
}ICMPECHO;

typedef ICMPECHO* PICMPECHO;

class CPing{
public:
	CPing();
	~CPing();
	bool Ping(char* strHost, ICMPECHO &icmpEcho);
private:
	// ICMP.DLL Export Function Pointers
	HANDLE (WINAPI *pIcmpCreateFile)(VOID);
	int (WINAPI *pIcmpCloseHandle)(HANDLE);
	DWORD (WINAPI *pIcmpSendEcho)
		(HANDLE,DWORD,LPVOID,WORD,PIPINFO,LPVOID,DWORD,DWORD);
	HANDLE hndlIcmp;			// LoadLibrary() handle to ICMP.DLL
	bool bValid; // if it doesn't construct properly, it won't be valid
};
*/

#endif	/* _POL_H_INCLUDED */
