/*
 * ========================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  UDP Chat based on win api and winsck library and threads.
 *
 *        Version:  1.0
 *        Created:  2009-11-24 23:02:40
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dariusz Synowiec 
 *
 * =======================================================
 */
// TODO: Strona tytulowa (UDP chat)
//    Dariusz Synowiec
//    Dariusz Wilk
//    Pawel Orlowski
// uzyty protokol (teoria) (udp, )
// uzyte rozkazy z winsocka
// uzyte elementy (edytor kompilator itd).
// dokumentacja z kodu

/* #####   HEADER FILE INCLUDES   ####################### */

#include <windows.h>
#include <windowsx.h>

#include <stdio.h>
#include <time.h>


/* #####   MACROS  -  LOCAL TO THIS SOURCE FILE   ####### */


#define	ON 1
#define	OFF 0

#define ECHO ON
#define MAX_BUF 20000

#define WINDOW_NAME "Udp_Chat"


/* #####   PROTOTYPES  -  LOCAL TO THIS SOURCE FILE   ### */

LRESULT CALLBACK MainWindowProcedure (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK TextWindowProcedure (HWND hwnd, UINT mesg, WPARAM wParam, LPARAM lParam);
void ResizeComponents(HWND hwnd);
int WinSockInit(void);
DWORD WINAPI UDPListenerThreadFunction(LPVOID lpParam);
u_long addressOf(const char * addrStr);


/* #####   VARIABLES  -  LOCAL TO THIS SOURCE FILE   #### */

char szClassName[ ] = "UDPChat";
HWND hText;
HWND hMessages;
HWND hMainWindow;     /* This is the handle for our window */
HWND hName;
HWND hHostName;
WNDPROC g_OldWndProc;

//char BuforT[MAX_BUF]; 
char* BuforT; 
char* inBuf;
char* name;
struct sockaddr_in sa;
SOCKET soc;
unsigned int nameLen;


/* #####   FUNCTION DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ##################### */


/* 
 * ===  FUNCTION  =======================================
 *         Name:  WinMain
 *  Description:  Main function
 * ======================================================
 */
int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nFunsterStil)
{
   char  host[200];

   MSG messages;            /* Here messages to the application are saved */
   WNDCLASSEX wincl;        /* Data structure for the windowclass */

   /* Bring up winsock */
   (void) WinSockInit();

   /* Open socket */
   soc = socket(PF_INET, SOCK_DGRAM, 0);


   sprintf(host, "127.0.0.1");          //loopback
   //   sprintf(host, "149.223.36.86");   //krzysiek
   //   sprintf(host, "R01184");          //krzysiek
   //   sprintf(host, "140.171.179.171"); //cytrix
   //   sprintf(host, "R01772");          //ja

   /* create default sending address */
   sa.sin_port = htons(13);
   sa.sin_family = AF_INET;
   sa.sin_addr.s_addr = addressOf(host);

   /* Allocate and clear buffers */
   BuforT = (char*) calloc(MAX_BUF, sizeof(char));
   inBuf  = (char*) calloc(MAX_BUF, sizeof(char));
   name   = (char*) calloc(MAX_BUF, sizeof(char));


   /*----------------------------------------------------
    *  Auto created by DevCpp BEGIN
    *----------------------------------------------------*/
   /* The Window structure */
   wincl.hInstance     = hThisInstance;
   wincl.lpszClassName = szClassName;
   wincl.lpfnWndProc   = MainWindowProcedure;      /* This function is called by windows */
   wincl.style         = CS_DBLCLKS;               /* Catch double-clicks */
   wincl.cbSize        = sizeof (WNDCLASSEX);

   /* Use default icon and mouse-pointer */
   wincl.hIcon        = LoadIcon (NULL, IDI_APPLICATION);
   wincl.hIconSm      = LoadIcon (NULL, IDI_APPLICATION);
   wincl.hCursor      = LoadCursor (NULL, IDC_ARROW);
   wincl.lpszMenuName = NULL;                   /* No menu */
   wincl.cbClsExtra   = 0;                      /* No extra bytes after the window class */
   wincl.cbWndExtra   = 0;                      /* structure or the window instance */
   /* Use Windows's default color as the background of the window */
   wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

   /* Register the window class, and if it fails quit the program */
   if (!RegisterClassEx (&wincl))
   {
      return 0;
   }

   /* The class is registered, let's create the program*/
   hMainWindow = CreateWindowEx (
         0,                   /* Extended possibilites for variation */
         szClassName,         /* Classname */
         WINDOW_NAME,          /* Title Text */
         WS_OVERLAPPEDWINDOW, /* default window */
         CW_USEDEFAULT,       /* Windows decides the position */
         CW_USEDEFAULT,       /* where the window ends up on the screen */
         544,                 /* The programs width */
         375,                 /* and height in pixels */
         HWND_DESKTOP,        /* The window is a child-window to desktop */
         NULL,                /* No menu */
         hThisInstance,       /* Program Instance handler */
         NULL                 /* No Window Creation data */
         );
   /*----------------------------------------------------
    *  Auto created by DevCpp END
    *----------------------------------------------------*/

   /* Adjust window name to: "%host% - %WINDOW_NAME%" */
   strcpy(BuforT, host);
   strcat(BuforT, " - ");
   strcat(BuforT, WINDOW_NAME);
   SetWindowText(hMainWindow, BuforT);

   /* Create host name text box */ 
   hHostName = CreateWindowEx (0, "EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 5, 5, 150, 20,
         hMainWindow, NULL, hThisInstance, NULL);

   /* Create user name text box */ 
   hName = CreateWindowEx (0, "EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 5, 25, 150, 20,
         hMainWindow, NULL, hThisInstance, NULL);    

   /* Create messages list text box */ 
   hMessages = CreateWindowEx (WS_EX_CLIENTEDGE, "EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | 
         WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL, 5, 45, 150, 130, hMainWindow, NULL, hThisInstance, NULL);

   /* Create new message text box */ 
   hText = CreateWindowEx (0, "EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 5, 160, 150, 20,
         hMainWindow, NULL, hThisInstance, NULL);    

   /* Adjust User name string to be "YourName" */
   sprintf(name, "YourName");
   SetWindowText (hName, name);
   nameLen = strlen(name);

   /* 
    * Override normal TEXT window class Procedure so we can easily catch enter key.
    * Note that it wll override all windows of class TEXT (so hHostName as well) 
    */
   g_OldWndProc = (WNDPROC) SetWindowLong (hText, GWL_WNDPROC, (LONG)TextWindowProcedure);
   SetWindowLong (hHostName, GWL_WNDPROC, (LONG)TextWindowProcedure);

   /* 
    * Register Win+C lobal hotkey, it will bring our window on top and set the focus on new message
    * text box
    */
   RegisterHotKey(hMainWindow, 1, MOD_WIN, 0x43);

   /* Adjust components sizes */
   ResizeComponents(hMainWindow);

   /* Make the window visible on the screen */
   ShowWindow (hMainWindow, nFunsterStil);

   /* Create listener thread.  */
   HANDLE hThread;
   DWORD dwThreadId;
   hThread = CreateThread(
         NULL,                      // default security attributes
         0,                         // use default stack size
         UDPListenerThreadFunction, // thread function name
         NULL,                      // argument to thread function
         0,                         // use default creation flags
         &dwThreadId);              // returns the thread identifier


   SetFocus(hText);

   /* Run the message loop. It will run until GetMessage() returns 0 */
   while (GetMessage (&messages, NULL, 0, 0))
   {
      /* Translate virtual-key messages into character messages */
      TranslateMessage(&messages);
      /* Send message to MainWindowProcedure */
      DispatchMessage(&messages);
   }

   /* The program return-value is 0 - The value that PostQuitMessage() gave */
   return messages.wParam;
}


/* 
 * ===  FUNCTION  =======================================
 *         Name:  TextWindowProcedure
 *  Description:  Function is responsible for intercepting enter key in text fields.
 * ======================================================
 */
LRESULT CALLBACK TextWindowProcedure (HWND hwnd, UINT mesg, WPARAM wParam, LPARAM lParam)
{
   switch (mesg)
   {
      case WM_KEYDOWN:
         {
            /* Let MainWindowProcedure handle the keys */
            CallWindowProc (MainWindowProcedure, hwnd, mesg, wParam, lParam);
         }
         break;
   }

   return CallWindowProc (g_OldWndProc, hwnd, mesg, wParam, lParam);
}


/* 
 * ===  FUNCTION  =======================================
 *         Name:  MainWindowProcedure
 *  Description:  This function is called by the Windows function DispatchMessage()
 * ======================================================
 */
LRESULT CALLBACK MainWindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   DWORD dlugoscM = 0;
   DWORD dlugoscT = 0;

   switch (message)                  /* handle the messages */
   {
      case WM_DESTROY:
         PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
         break;
      case WM_KEYDOWN:
         /* When a key was pressed */
         {
            switch ( (int) wParam )
            {
               case VK_RETURN: /* if it was <RETURN> */
                  if (hwnd == hText) /* if focus was set to new message window */
                  {
                     nameLen = GetWindowTextLength (hName);
                     GetWindowText (hName, name, nameLen + 1);

                     dlugoscM = GetWindowTextLength(hMessages);
                     dlugoscT = GetWindowTextLength(hText);

                     if (dlugoscT > MAX_BUF)
                     {
                        MessageBox(hMainWindow, "To long Message", "Error",MB_OK | MB_ICONERROR );
                        SetFocus(hText);
                        break;
                     }

                     /* Paste username to the buffer */
                     strncpy(BuforT, name, nameLen);

                     /* add message */
                     GetWindowText (hText, &BuforT[nameLen + 1], dlugoscT + 1);

                     /* add ":" and "\0" */
                     BuforT[nameLen] = ':';
                     BuforT[nameLen + dlugoscT + 1] = '\0';

                     DWORD e;
                     /* send the message */
                     e = sendto(soc, BuforT, nameLen + dlugoscT + 1, 0, (struct sockaddr *) & sa, sizeof (sa));

                     if (e == SOCKET_ERROR) {
                        MessageBox(hMainWindow, "Sendto Failed", "Error",MB_OK | MB_ICONERROR );
                     }

#if ECHO == ON
                     if (dlugoscM > 0)
                     {
                        BuforT[nameLen - 1] = '\r';
                        BuforT[nameLen + 0] = '\n';
                        Edit_SetSel(hMessages, dlugoscM, dlugoscM);
                        Edit_ReplaceSel(hMessages, &BuforT[nameLen - 1]);
                     }
                     else
                     {
                        SetWindowText (hMessages, &BuforT[nameLen + 1]);
                     }
#endif

                     SetWindowText(hText, "");                      

                     SendMessage(hMessages, WM_VSCROLL, SB_BOTTOM, 0);
                  }
                  else if (hwnd == hHostName) /* if focus was set to host address window */
                  {
                     dlugoscT = GetWindowTextLength(hHostName);

                     if (0 < dlugoscT)
                     {
                        u_long temp;
                        GetWindowText(hHostName, BuforT, dlugoscT + 1);

                        temp = addressOf(BuforT);
                        if (INADDR_NONE != temp)
                        {
                           /* change the destination adress */
                           sa.sin_addr.s_addr = temp;
                           /* update the window title */
                           strcat(BuforT, " - ");
                           strcat(BuforT, WINDOW_NAME);
                           SetWindowText(hMainWindow, BuforT);
                        }
                     }
                     else
                     {
                        MessageBox(hMainWindow, "Wpisz adres hosta.", "Error",MB_OK | MB_ICONERROR );
                     }
                     SetFocus(hText);
                  }
                  break;
               case VK_ESCAPE:
                  /* If <Esc> was pressed - minimize the window */
                  ShowWindow (hMainWindow, SW_MINIMIZE);
                  break;
            }
         }
         break;
      case WM_HOTKEY:
         /* 
          * Since we only have one hotkey registered, we don't have to check which hotkey was
          * pressed. Bring the window to the top.
          */
         ShowWindow (hMainWindow, SW_MINIMIZE);
         ShowWindow (hMainWindow, SW_SHOWNORMAL);
         SetFocus(hText);
         break;
      case WM_SIZE:
         /* We get this message each time size of the window has changed. Update controls sizes. */
         ResizeComponents(hMainWindow);
         break;
      default:                      /* for messages that we don't deal with */
         return DefWindowProc (hwnd, message, wParam, lParam);
   }

   return 0;
}

/* 
 * ===  FUNCTION  =======================================
 *         Name:  ResizeComponents
 *  Description:  Function resizes all components to their proper size using main window dimensions.
 * ======================================================
 */
void ResizeComponents(HWND hwnd)
{
   RECT rect;
   GetClientRect (hwnd, &rect);

   SetWindowPos(hHostName , HWND_TOP , 5 , rect.top + 5     , rect.right - 10 , 20               , 0);
   SetWindowPos(hName     , HWND_TOP , 5 , rect.top + 30    , rect.right - 10 , 20               , 0);
   SetWindowPos(hMessages , HWND_TOP , 5 , rect.top + 55    , rect.right - 10 , rect.bottom - 90 , 0);
   SetWindowPos(hText     , HWND_TOP , 5 , rect.bottom - 25 , rect.right - 10 , 20               , 0);
}

/* 
 * ===  FUNCTION  =======================================
 *         Name:  WinSockInit
 *  Description:  Loads winsock library.
 * ======================================================
 */
int WinSockInit() {
   int retVal = 0;
   WORD version = MAKEWORD(1, 1);
   WSADATA wsaData;

   retVal = WSAStartup(version, &wsaData);

   if (0 != retVal) {
      MessageBox(hMainWindow, "Init Failed", "Error",MB_OK | MB_ICONERROR );
   }

   return retVal;
}

/* 
 * ===  FUNCTION  =======================================
 *         Name:  UDPListenerThreadFunction
 *  Description:  THread listens on port 13 and accepts all udp transmissions. It displays every
 *                message in hMessages window.
 * ======================================================
 */
DWORD WINAPI UDPListenerThreadFunction(LPVOID lpParam) {
   DWORD dlugoscM = 0;
   DWORD dlugoscT = 0;
   struct sockaddr_in A;
   int s, d;

   s = socket(AF_INET, SOCK_DGRAM, 0);

   A.sin_family = AF_INET;
   A.sin_port = htons(13);
   A.sin_addr.s_addr = INADDR_ANY;

   d = bind(s, (struct sockaddr *) & A, sizeof (A));

   if (d >= 0) {
      int dw = sizeof (A);
      while(1)
      {
         memset(inBuf,0,100);
         d = recvfrom(s, &inBuf[2], 100, 0, (sockaddr *) & A, &dw);

#if ECHO == ON
         if (127 != A.sin_addr.s_net)
#endif
         {
            dlugoscM = GetWindowTextLength(hMessages);

            if (dlugoscM > 0)
            {
               inBuf[0] = '\r';
               inBuf[1] = '\n';
               Edit_SetSel(hMessages, dlugoscM, dlugoscM);
               Edit_ReplaceSel(hMessages, inBuf);
            }
            else
            {
               SetWindowText (hMessages, &inBuf[2]);
            }

            SendMessage(hMessages, WM_VSCROLL, SB_BOTTOM, 0);
         }
      }
   }
}


/* 
 * ===  FUNCTION  =======================================
 *         Name:  addressOf
 *  Description:  Tries to translate a char array containing host address to a u_long address.
 *         Note:  User still have to perform htonl.
 * ======================================================
 */
u_long addressOf(const char * addrStr)
{
   u_long retVal;
   struct hostent* phe;

   char* p1;
   char* p2;

   retVal = inet_addr(addrStr);
   if (INADDR_NONE == retVal)
   {
      phe = gethostbyname(addrStr);

      if (NULL == phe)
      {
         MessageBox(hMainWindow, "Nie znalalem hosta", "Error",MB_OK | MB_ICONERROR );
      }
      else
      {

         p1 = (char *) & retVal;
         p2 = &phe->h_addr[0];

         for (int i=0; i<sizeof(retVal); i++) 
         {
            p1[i]=p2[i];
         }
      }
   }

   return retVal;
}
