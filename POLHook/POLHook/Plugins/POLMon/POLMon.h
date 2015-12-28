/*
    <CVS SERVER GENERATED>
    This project was stored on Folko's CVS server as $Source: /home/cvs/POLHookSolution/Plugins/POLMon/POLMon.h,v $.
    
    Id tag follows (<filename> <revision> <last changed> <author> <status>):
    $Id: POLMon.h,v 1.1.1.1 2003/04/24 15:18:17 folko23 Exp $
    
    If you want to report any errors, please include this tag.
    </GENERATED>
*/

#ifndef _POLMON_H_INCLUDED
#define _POLMON_H_INCLUDED

#pragma comment(lib, "WS2_32")
#pragma comment(lib, "COMCTL32")

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, UINT wParam, LONG lParam);
DWORD UnhandledExceptionHandler(PEXCEPTION_POINTERS info);
void AddColumn(HWND list, int subitem, int width, char *name);
int CALLBACK SortList(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
void POLMon(void *nil);
#endif /* _POLHOOK_H_INCLUDED */
