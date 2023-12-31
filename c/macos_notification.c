#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include <mach/mach_port.h>
#include <mach/mach_interface.h>
#include <mach/mach_init.h>

#include <IOKit/pwr_mgt/IOPMLib.h>
#include <IOKit/IOMessage.h>

io_connect_t root_port;
IONotificationPortRef notifyPortRef;
io_object_t notifierObject;
CFRunLoopRef runLoop;

typedef int (*OnCanSleep)();
typedef void *(*OnWillSleep)();
typedef void *(*OnWillWake)();

typedef struct
{
    OnCanSleep canSleep;
    OnWillSleep willSleep;
    OnWillWake willWake;
} Callback;

void SleepCallback(void *refCon, io_service_t service, natural_t messageType, void *messageArgument)
{
    Callback *callback = (Callback *)refCon;

    switch (messageType)
    {
    case kIOMessageCanSystemSleep:
        if ((*callback).canSleep())
        {
            IOAllowPowerChange(root_port, (long)messageArgument);
        }
        else
        {
            IOCancelPowerChange(root_port, (long)messageArgument);
        }
        break;

    case kIOMessageSystemWillSleep:
        (*callback).willSleep();
        IOAllowPowerChange(root_port, (long)messageArgument);
        break;

    case kIOMessageSystemWillPowerOn:
        (*callback).willWake();
        break;

    case kIOMessageSystemHasPoweredOn:
        break;

    default:
        break;
    }
}

extern int startNotifications(Callback *callback)
{
    root_port = IORegisterForSystemPower(callback, &notifyPortRef, SleepCallback, &notifierObject);
    if (root_port == 0)
    {
        return 1;
    }
    CFRunLoopAddSource(CFRunLoopGetCurrent(),
                       IONotificationPortGetRunLoopSource(notifyPortRef), kCFRunLoopCommonModes);

    runLoop = CFRunLoopGetCurrent();
    CFRunLoopRun();
    return 0;
}

extern void stopNotifications()
{
    CFRunLoopRemoveSource(runLoop,
                          IONotificationPortGetRunLoopSource(notifyPortRef),
                          kCFRunLoopCommonModes);

    IODeregisterForSystemPower(&notifierObject);
    IOServiceClose(root_port);
    IONotificationPortDestroy(notifyPortRef);
    CFRunLoopStop(runLoop);
}