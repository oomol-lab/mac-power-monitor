#include <uv.h>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>

#include "pm.h"
#include "constants.h"


#define MAX_THREAD_WINDOW_NAME 64

HWND handle;
DWORD threadId;
HANDLE threadHandle;

HANDLE notifyEvent;

char *notify_msg;

bool isRunning = false;

DWORD WINAPI ListenerThread( LPVOID lpParam );

void NotifyAsync(uv_work_t* req);
void NotifyFinished(uv_work_t* req);


void NotifyAsync(uv_work_t* req)
{
    WaitForSingleObject(notifyEvent, INFINITE);
}


void NotifyFinished(uv_work_t* req)
{
    if (isRunning)
    {
        Notify(notify_msg);
    }

    uv_queue_work(uv_default_loop(), req, NotifyAsync, (uv_after_work_cb)NotifyFinished);
}

static LRESULT FAR PASCAL WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if(message == WM_POWERBROADCAST)
    {
		// printf("%d\n", wParam);
		if (wParam == PBT_APMRESUMESUSPEND)
		{
			// printf("wake\n");

            notify_msg = (char *)WAKE_NOTIFY;
            SetEvent(notifyEvent);
		}

		if (wParam == PBT_APMRESUMEAUTOMATIC)
		{
			// printf("waking\n");
            // strcpy(notify_msg, "waking");
            // SetEvent(notifyEvent);
		}

		if (wParam == PBT_APMSUSPEND)
		{
			// printf("sleeping\n");

            notify_msg = (char *)SLEEP_NOTIFY;
            SetEvent(notifyEvent);
		}

		if (wParam == PBT_APMPOWERSTATUSCHANGE)
		{
			// printf("PBT_APMPOWERSTATUSCHANGE\n");
            // SetEvent(notifyEvent);
		}

        //Do something
        return TRUE;
    }
    else
        return DefWindowProc(hWnd, message, wParam, lParam);
}

DWORD WINAPI ListenerThread( LPVOID lpParam ) 
{
    char className[MAX_THREAD_WINDOW_NAME];
    _snprintf_s(className, MAX_THREAD_WINDOW_NAME, "ListnerThreadPmNotify_%d", GetCurrentThreadId());

    WNDCLASS wc = {0};

    // Set up and register window class
    wc.lpfnWndProc = (WNDPROC)WindowProc;
    wc.lpszClassName = className;
    RegisterClass(&wc);
    HWND hWin = CreateWindow(className, className, 0, 0, 0, 0, 0, NULL, NULL, NULL, 0);

    BOOL bRet;
    MSG msg;
    while( (bRet = GetMessage( &msg, hWin, 0, 0 )) != 0)
    { 
        if (bRet == -1)
        {
            // handle the error and possibly exit
        }
        else
        {
            TranslateMessage(&msg); 
            DispatchMessage(&msg); 
        }
    }

    return 0;
}

void Start()
{
    isRunning = true;
}

void Stop()
{
    isRunning = false;
    SetEvent(notifyEvent);
}

void InitPM()
{
    notifyEvent = CreateEvent(NULL, false /* auto-reset event */, false /* non-signalled state */, "");
    
    threadHandle = CreateThread( 
        NULL,                   // default security attributes
        0,                      // use default stack size  
        ListenerThread,         // thread function name
        NULL,                   // argument to thread function 
        0,                      // use default creation flags 
        &threadId);   

    uv_work_t* req = new uv_work_t();
    uv_queue_work(uv_default_loop(), req, NotifyAsync, (uv_after_work_cb)NotifyFinished);

    Start();
}