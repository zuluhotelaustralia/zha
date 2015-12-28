#include <stdio.h>
#include <stdlib.h>

#include "CPing.h"

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


int main(int argc, char *argv[]){
    CPing ping;
	ICMPECHO yer;

    if (argc != 2){
        fprintf(stderr, "Usage: %s <domain>\n", argv[0]);
        exit(1);
    }

    if (ping.Ping(argv[1], yer)){
        printf("The host %s is alive and ping was %d\n", argv[1], yer.RTTime);
    }else{
        printf("The host %s is not responding to ping\n", argv[1]);
    }

    return 0;
}
