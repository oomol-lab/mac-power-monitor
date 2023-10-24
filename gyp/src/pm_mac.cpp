#include <mach/mach_port.h>
#include <mach/mach_interface.h>
#include <mach/mach_init.h>

#include <IOKit/pwr_mgt/IOPMLib.h>
#include <IOKit/IOMessage.h>

#include <pthread.h>

#include <uv.h>

#include "pm.h"
#include "constants.h"

io_connect_t  root_port; // a reference to the Root Power Domain IOService
IONotificationPortRef  notifyPortRef; // notification port allocated by IORegisterForSystemPower
pthread_t lookupThread;
pthread_mutex_t notify_mutex;
pthread_cond_t notify_cv;
char *notify_msg;
bool isRunning = false;

void NotifyCallBack(void * refCon, io_service_t service, natural_t messageType, void * messageArgument )
{
    // fprintf(stderr, "messageType %08lx, arg %08lx\n",
    //        (long unsigned int)messageType,
    //        (long unsigned int)messageArgument );

    switch ( messageType )
    {

        case kIOMessageCanSystemSleep:
            /* Idle sleep is about to kick in. This message will not be sent for forced sleep.
             Applications have a chance to prevent sleep by calling IOCancelPowerChange.
             Most applications should not prevent idle sleep.

             Power Management waits up to 30 seconds for you to either allow or deny idle
             sleep. If you don't acknowledge this power change by calling either
             IOAllowPowerChange or IOCancelPowerChange, the system will wait 30
             seconds then go to sleep.
             */

            //Uncomment to cancel idle sleep
            //IOCancelPowerChange( root_port, (long)messageArgument );
            // we will allow idle sleep
            // fprintf(stderr, "kIOMessageCanSystemSleep\n");
            IOAllowPowerChange( root_port, (long)messageArgument );
            break;

        case kIOMessageSystemWillSleep:
            /* The system WILL go to sleep. If you do not call IOAllowPowerChange or
             IOCancelPowerChange to acknowledge this message, sleep will be
             delayed by 30 seconds.

             NOTE: If you call IOCancelPowerChange to deny sleep it returns
             kIOReturnSuccess, however the system WILL still go to sleep.
             */
            // fprintf(stderr, "kIOMessageSystemWillSleep\n");

            pthread_mutex_lock(&notify_mutex);
	        notify_msg = (char*) SLEEP_NOTIFY;
	        pthread_cond_signal(&notify_cv);
	        pthread_mutex_unlock(&notify_mutex);

            IOAllowPowerChange( root_port, (long)messageArgument );
            break;

        case kIOMessageSystemWillPowerOn:
            //System has started the wake up process...
            // fprintf(stderr, "kIOMessageSystemWillPowerOn\n");

            // pthread_mutex_lock(&notify_mutex);
	        // strcpy(notify_msg, "waking");
	        // pthread_cond_signal(&notify_cv);
	        // pthread_mutex_unlock(&notify_mutex);

            break;

        case kIOMessageSystemHasPoweredOn:
            //System has finished waking up...
            // fprintf(stderr, "kIOMessageSystemHasPoweredOn\n");

            pthread_mutex_lock(&notify_mutex);
            notify_msg = (char*) WAKE_NOTIFY;
	        pthread_cond_signal(&notify_cv);
	        pthread_mutex_unlock(&notify_mutex);

            break;

        default:
            break;

    }
}

void NotifyAsync(uv_work_t* req)
{
    pthread_mutex_lock(&notify_mutex);
    pthread_cond_wait(&notify_cv, &notify_mutex);
    pthread_mutex_unlock(&notify_mutex);
}

void NotifyFinished(uv_work_t* req)
{
    if (isRunning)
    {
        pthread_mutex_lock(&notify_mutex);
        Notify(notify_msg);
        pthread_mutex_unlock(&notify_mutex);
        uv_queue_work(uv_default_loop(), req, NotifyAsync, (uv_after_work_cb)NotifyFinished);
    }
}

void *RunLoop(void * arg)
{

    // add the notification port to the application runloop
    CFRunLoopAddSource( CFRunLoopGetCurrent(),
                       IONotificationPortGetRunLoopSource(notifyPortRef), kCFRunLoopCommonModes );

    /* Start the run loop to receive sleep notifications. Don't call CFRunLoopRun if this code
     is running on the main thread of a Cocoa or Carbon application. Cocoa and Carbon
     manage the main thread's run loop for you as part of their event handling
     mechanisms.
     */
    CFRunLoopRun();

    // We should never get here
    fprintf(stderr, "Unexpectedly back from CFRunLoopRun()!\n");

    return NULL;
}

void Start()
{
    isRunning = true;
}

void Stop()
{
    isRunning = false;
    pthread_mutex_lock(&notify_mutex);
    pthread_cond_signal(&notify_cv);
    pthread_mutex_unlock(&notify_mutex);
}

void InitPM()
{
    // notifier object, used to deregister later
    io_object_t            notifierObject;
    // this parameter is passed to the callback
    void*                  refCon = NULL;

    // register to receive system sleep notifications

    root_port = IORegisterForSystemPower( refCon, &notifyPortRef, NotifyCallBack, &notifierObject );
    if ( root_port == 0 )
    {
        fprintf(stderr, "IORegisterForSystemPower failed\n");
    }

    pthread_mutex_init(&notify_mutex, NULL);
    pthread_cond_init(&notify_cv, NULL);

    int rc = pthread_create(&lookupThread, NULL, RunLoop, NULL);
    if (rc)
    {
        fprintf(stderr, "ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
    }

    uv_work_t* req = new uv_work_t();
    uv_queue_work(uv_default_loop(), req, NotifyAsync, (uv_after_work_cb)NotifyFinished);

    Start();
}