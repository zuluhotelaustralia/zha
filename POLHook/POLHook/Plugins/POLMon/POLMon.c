/* POLMon 2.4 (C) Folko <Folko@elitecoder.net> */

/*
    <CVS SERVER GENERATED>
    This project was stored on Folko's CVS server as $Source: /home/cvs/POLHookSolution/Plugins/POLMon/POLMon.c,v $.

    Id tag follows (<filename> <revision> <last changed> <author> <status>):
    $Id: POLMon.c,v 1.4 2003/06/21 13:48:36 folko23 Exp $

    If you want to report any errors, please include this tag.
    </GENERATED>
*/

/*  History
   2.4 / 21 June 2003
      - POLMon will now only update if it's not the focused window, unless you click "Update".

   2.3 / 24 April 2003
      - Converted to POLHookPlugin.
      - Fixed "Connections: " position.

   2.2 / 12 March 2003
      - POLMon's listview now resizing when window resizes.

   2.1 / 06 February 2003:
      - Added some sanity checks.

   2.0 / 14 November 2002:
      - Converted whole project to C.

   1.3: - Fixed an important bug in the message loop.
   1.2: - Made compatible with commonly used compilers.
   1.1: - Added AddColumn() to add a column.
   1.0: - Initial release.
*/
#include <windows.h>
#include <process.h>
#include <winsock.h>
#include <commctrl.h>
#include <stdio.h>
#include "Plugin.h"
#include "POLMon.h"

PluginData POLHook;
HANDLE POLMonThread;
void (**HookChain) (DWORD, BYTE *);

int WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
   return 1;
}

void KillPlugin(void)
{
   TerminateThread(POLMonThread, 0);
   return;
}

void ReloadPlugin(void)
{
   TerminateThread(POLMonThread, 0);
   POLMonThread=(HANDLE)_beginthread(POLMon, 0, 0);
   return;
}

int POLHookPlugin(int version, PluginData pdata, void (**pHookChain) (DWORD, BYTE *) )
{
   if(version!=PLUGIN_VERSION)
      return 0;
   HookChain=pHookChain;
   POLHook=pdata;
   POLMonThread=(HANDLE)_beginthread(POLMon, 0, 0);
   return 1;
}

void POLMon(void *nil)
{
   WNDCLASSEX wc;
   HINSTANCE hInstance;
   HWND hWnd;
   MSG msg;

   InitCommonControls();
   printf("Starting POLMon in unsafe mode.. use it at your own risk!\n");
   hInstance = GetModuleHandle(NULL);
   memset(&wc, 0, sizeof(WNDCLASSEX));
   wc.cbSize = sizeof(WNDCLASSEX);
   wc.style = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc = WndProc;
   wc.cbClsExtra = 0;
   wc.cbWndExtra = 0;
   wc.hInstance = hInstance;
   wc.hIcon = NULL;
   wc.hCursor = LoadCursor(NULL, IDC_ARROW);
   wc.hbrBackground = (HBRUSH) GetStockObject(GRAY_BRUSH);
   wc.lpszMenuName = NULL;
   wc.lpszClassName = "POLHookClass";
   RegisterClassEx(&wc);
   hWnd =
      CreateWindowEx(0, "POLHookClass", "Folko's POLMon for POLHook", WS_VISIBLE | WS_OVERLAPPEDWINDOW, 0, 0, 700, 450, NULL,
                    NULL, hInstance, NULL);
   if (!hWnd)
   {
      printf("Couldn't create POLMon's window..\n");
      _endthread();
      return;
   }
   ShowWindow(hWnd, SW_SHOW);
   UpdateWindow(hWnd);

   while (GetMessage(&msg, NULL, 0, 0) > 0)
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }
   _endthread();
   return;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, UINT wParam, LONG lParam)
{
   static HINSTANCE hInstance;
   static HWND list, bcastwindow, bcastbutton, updatebutton;
   static HMENU menu;
   static RECT rect;
   static int force;

   switch (uMsg)
   {
   case WM_CREATE:
      {
         force=0;
         hInstance = GetModuleHandle(NULL);
         GetClientRect(hWnd, &rect);

         SetTimer(hWnd, 1, 10 * 1000, NULL);    /* update status all 10 secs */

         menu = CreatePopupMenu();
         AppendMenu(menu, MF_STRING | MF_POPUP, 1, "Kick");
         AppendMenu(menu, MF_STRING | MF_POPUP, 2, "SendSysMessage (from bcast window)");

         bcastwindow =
            CreateWindowEx(0, "Edit", 0, WS_VISIBLE | WS_CHILD | WS_BORDER, 0, 20, 200, 20, hWnd, NULL, hInstance, NULL);

         bcastbutton =
            CreateWindowEx(0, "Button", "Broadcast", WS_VISIBLE | WS_CHILD | WS_BORDER, 205, 20, 75, 20, hWnd, NULL,
                           hInstance, NULL);

         list =
            CreateWindowEx(0, WC_LISTVIEW, "",
                           WS_CHILD | WS_BORDER | WS_VISIBLE | LVS_AUTOARRANGE | LVS_REPORT | LVS_SINGLESEL, 0, 50, 690, 340,
                           hWnd, NULL, hInstance, NULL);

         AddColumn(list, 0, 130, "IP");
         AddColumn(list, 1, 130, "Name");
         AddColumn(list, 2, 130, "Account");
         AddColumn(list, 3, 60, "CmdLevel");
         AddColumn(list, 4, 70, "X");
         AddColumn(list, 5, 70, "Y");
         AddColumn(list, 6, 40, "Dead");

         updatebutton =
            CreateWindowEx(0, "Button", "Update", WS_VISIBLE | WS_CHILD | WS_BORDER, 0, 395, 75, 20, hWnd, NULL, hInstance, NULL);
      }break;
   case WM_CONTEXTMENU:
      {
         if (wParam == (WPARAM) list)
            TrackPopupMenuEx(menu, TPM_LEFTBUTTON, LOWORD(lParam), HIWORD(lParam), hWnd, NULL);
      }break;
   case WM_PAINT:
      {
         char status[50];
         PAINTSTRUCT ps;
         HDC hDC = BeginPaint(hWnd, &ps);

         SetBkColor(hDC, RGB(128, 128, 128));

         sprintf(status, "Connections: %d", POLHook.GetNumConnections());
         TextOut(hDC, 0, 5, status, (int) strlen(status));

         EndPaint(hWnd, &ps);
      }break;
   case WM_SIZE:
      {
         MoveWindow(list, 0, 50, LOWORD(lParam), HIWORD(lParam)-70, 1);
         MoveWindow(updatebutton, 0, HIWORD(lParam)-20, 75, 20, 1);
      }break;
   case WM_TIMER:
      {
         int i;
         HWND focus=GetFocus();

         if((hWnd==focus || list==focus) && !force) /* Dont update if we're the focus */
            break;
         force=0;
         ListView_DeleteAllItems(list);
         ListView_SetItemCount(list, POLHook.GetNumConnections());
         for (i = 0; i < POLHook.GetNumConnections(); i++)
         {
            char temp[10];
            DWORD who = POLHook.GetPlayer(i);
            LVITEM cur;

            memset(&cur, 0, sizeof(LVITEM));
            cur.mask = LVIF_TEXT | LVIF_PARAM;
            cur.iItem = i;
            cur.pszText = POLHook.GetClientIP(who);;
            cur.lParam = i;
            ListView_InsertItem(list, &cur);

            ListView_SetItemText(list, i, 1, POLHook.GetName(who) ? POLHook.GetName(who) : (char *) "No char selected yet");    /* the type cast is for gcc compatibility */
            ListView_SetItemText(list, i, 2, POLHook.GetAccount(who) ? POLHook.GetAccount(who) : (char *) "Not logged in yet");

            itoa(POLHook.GetStatus(who, POLHook.pol.CMDLEVEL), temp, 10);
            ListView_SetItemText(list, i, 3, POLHook.GetStatus(who, POLHook.pol.CMDLEVEL) != (signed)-1 ? temp : (char *) "");

            itoa(POLHook.GetStatus(who, POLHook.pol.X), temp, 10);
            ListView_SetItemText(list, i, 4, POLHook.GetStatus(who, POLHook.pol.X) != (signed)-1 ? temp : (char *) "");

            itoa(POLHook.GetStatus(who, POLHook.pol.Y), temp, 10);
            ListView_SetItemText(list, i, 5, POLHook.GetStatus(who, POLHook.pol.Y) != (signed)-1 ? temp : (char *) "");

            itoa(POLHook.GetStatus(who, POLHook.pol.DEAD), temp, 10);
            ListView_SetItemText(list, i, 6, POLHook.GetStatus(who, POLHook.pol.DEAD) != (signed)-1 ? temp : (char *) "");
            Sleep(1);   /* 'tis necessary for your insane 300-players-online-shards :) */
         }
         InvalidateRect(hWnd, &rect, 1);
      }break;
   case WM_NOTIFY:
      {
         NMHDR *nm = (NMHDR *) (LRESULT) lParam;

         if (nm->code == LVN_COLUMNCLICK)
         {
            NM_LISTVIEW *pnmv = (NM_LISTVIEW FAR *) (LRESULT) lParam;

            ListView_SortItems(list, SortList, pnmv->iSubItem);
         }
      }break;
   case WM_COMMAND:
      {
         LVITEM sel;

         if (HIWORD(wParam) == 0 && lParam == 0)    /* menu */
         {
            DWORD who;
            int index = ListView_GetNextItem(list, -1, LVNI_SELECTED);

            if (index == -1)
            {
               MessageBox(hWnd, "You have to select a connection first.\n", "Error", MB_ICONERROR);
               return 0;
            }
            memset(&sel, 0, sizeof(LVITEM));
            sel.mask = LVIF_PARAM;
            sel.iItem = index;
            ListView_GetItem(list, &sel);

            who = POLHook.GetPlayer((DWORD) sel.lParam);
            if (!who)
            {
               MessageBox(hWnd, "Player logged off\n", "Error", MB_ICONERROR);
               return 0;
            }
            switch (LOWORD(wParam))
            {
            case 1: /* Kick */
               {
                  POLHook.Disconnect(who);
                  SendMessage(hWnd, WM_TIMER, 1, 0);
               }break;
            case 2: /* SysMessage */
               {
                  char text[1024];

                  GetWindowText(bcastwindow, text, sizeof(text));
                  POLHook.pol.SendSysMessage(who, text, 3, 0x480);
               }break;
            }
         } else if (HIWORD(wParam) == 0 && lParam == (LPARAM) bcastbutton) {  /* broadcast */
            int i;
            char text[1024];

            GetWindowText(bcastwindow, text, sizeof(text));
            for (i = 0; i < POLHook.GetNumConnections(); i++)
            {
               POLHook.pol.SendSysMessage(POLHook.GetPlayer(i), text, 2, 0x480);
               Sleep(1);
            }
         } else if (HIWORD(wParam) == 0 && lParam == (LPARAM) updatebutton) {  /* update */
            force=1;
            SendMessage(hWnd, WM_TIMER, 1, 0);
         }
      }break;
   case WM_DESTROY:
      {
         PostQuitMessage(0);
      }break;
   default:
      return DefWindowProc(hWnd, uMsg, wParam, lParam);
   }
   return 0;
}

void AddColumn(HWND list, int subitem, int width, char *name)
{
   LV_COLUMN col;

   memset(&col, 0, sizeof(LV_COLUMN));
   col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
   col.fmt = LVCFMT_LEFT;
   col.cx = width;
   col.pszText = name;
   col.iSubItem = subitem;
   ListView_InsertColumn(list, subitem, &col);
}

int CALLBACK SortList(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   DWORD who1 = POLHook.GetPlayer((DWORD) lParam1), who2 = POLHook.GetPlayer((DWORD) lParam2);

   if (!who1 && !who2)
      return 0;
   if (!who2)
      return 1;
   if (!who1)
      return -1;

   switch (lParamSort)
   {
   case 0:
      {
         char IP1[20], IP2[20];

         strcpy(IP1, POLHook.GetClientIP(who1));
         strcpy(IP2, POLHook.GetClientIP(who2));
         return strcmp(IP1, IP2);
      }break;
   case 1:
      {
         char *name1, *name2;

         name1 = POLHook.GetName(who1);
         name2 = POLHook.GetName(who2);
         if (!name1 && !name2)
            return 0;
         if (!name1)
            return 1;
         if (!name2)
            return -1;
         return strcmp(name1, name2);
      }break;
   case 2:
      {
         char *name1, *name2;

         name1 = POLHook.GetAccount(who1);
         name2 = POLHook.GetAccount(who2);
         if (!name1 && !name2)
            return 0;
         if (!name1)
            return 1;
         if (!name2)
            return -1;
         return strcmp(name1, name2);
      }break;
   case 3:
      {
         if (POLHook.GetStatus(who1, POLHook.pol.CMDLEVEL) > POLHook.GetStatus(who2, POLHook.pol.CMDLEVEL))
            return -1;
         else if (POLHook.GetStatus(who1, POLHook.pol.CMDLEVEL) < POLHook.GetStatus(who2, POLHook.pol.CMDLEVEL))
            return 1;
         else
            return 0;
      }break;
   case 4:
      {
         if (POLHook.GetStatus(who1, POLHook.pol.X) > POLHook.GetStatus(who2, POLHook.pol.X))
            return 1;
         else if (POLHook.GetStatus(who1, POLHook.pol.X) < POLHook.GetStatus(who2, POLHook.pol.X))
            return -1;
         else
            return 0;
      }break;
   case 5:
      {
         if (POLHook.GetStatus(who1, POLHook.pol.Y) > POLHook.GetStatus(who2, POLHook.pol.Y))
            return 1;
         else if (POLHook.GetStatus(who1, POLHook.pol.Y) < POLHook.GetStatus(who2, POLHook.pol.Y))
            return -1;
         else
            return 0;
      }break;
   case 6:
      {
         if (POLHook.GetStatus(who1, POLHook.pol.DEAD) > POLHook.GetStatus(who2, POLHook.pol.DEAD))
            return 1;
         else if (POLHook.GetStatus(who1, POLHook.pol.DEAD) < POLHook.GetStatus(who2, POLHook.pol.DEAD))
            return -1;
         else
            return 0;
      }break;
   }
   return 0;
}
