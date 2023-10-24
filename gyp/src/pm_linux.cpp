#include <dbus/dbus-glib.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 

#include "pm.h"
#include "constants.h"


/**********************************
 * Local defines
 **********************************/
#define RESUME_SIGNAL   "Resuming"
#define SLEEP_SIGNAL   	"Sleeping"
#define POWER_INTERFACE "org.freedesktop.UPower"

#define RULE  ("type='signal',interface='"POWER_INTERFACE"'")
 

 /**********************************
 * Local Variables
 **********************************/

char *notify_msg;

pthread_t thread;
pthread_mutex_t notify_mutex;
pthread_cond_t notify_cv;
bool isActive = false;

DBusConnection *conn;


/**********************************
 * Local Helper Functions protoypes
 **********************************/
void InitDbus();
DBusHandlerResult signal_filter(DBusConnection *connection, DBusMessage *msg, void *user_data);
void NotifyAsync(uv_work_t* req);
void NotifyFinished(uv_work_t* req);
void* ThreadFunc(void* ptr);

/**********************************
 * Public Functions
 **********************************/
void InitPM()
{
	pthread_mutex_init(&notify_mutex, NULL);
    pthread_cond_init(&notify_cv, NULL);

    uv_work_t* req = new uv_work_t();
    uv_queue_work(uv_default_loop(), req, NotifyAsync, (uv_after_work_cb)NotifyFinished);

    pthread_create(&thread, NULL, ThreadFunc, NULL);

    Start();
}

void Stop()
{
	isActive = false;
    pthread_mutex_lock(&notify_mutex);
    pthread_cond_signal(&notify_cv);
    pthread_mutex_unlock(&notify_mutex);
}

void Start()
{
	isActive = true;
}

/**********************************
* Local Functions
***********************************/
DBusHandlerResult signal_filter(DBusConnection *connection, DBusMessage *msg, void *user_data)
{
	if(isActive)
	{
	    if (dbus_message_is_signal(msg, POWER_INTERFACE, RESUME_SIGNAL)) 
	    {
	    	pthread_mutex_lock(&notify_mutex);
		    notify_msg = (char *) WAKE_NOTIFY;
            pthread_cond_signal(&notify_cv);
		    pthread_mutex_unlock(&notify_mutex);
	    }
	    else if (dbus_message_is_signal(msg, POWER_INTERFACE, SLEEP_SIGNAL)) 
	    {
	    	pthread_mutex_lock(&notify_mutex);
		    notify_msg = (char *) SLEEP_NOTIFY;
            pthread_cond_signal(&notify_cv);
		    pthread_mutex_unlock(&notify_mutex);
	    }
	}
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}
 

void NotifyAsync(uv_work_t* req)
{
    pthread_mutex_lock(&notify_mutex);
    pthread_cond_wait(&notify_cv, &notify_mutex);
    pthread_mutex_unlock(&notify_mutex);
}


void NotifyFinished(uv_work_t* req)
{
    if (isActive) 
    {
        pthread_mutex_lock(&notify_mutex);
        Notify(notify_msg);
        pthread_mutex_unlock(&notify_mutex);
        uv_queue_work(uv_default_loop(), req, NotifyAsync, (uv_after_work_cb)NotifyFinished);
    }
}

void InitDbus()
{
	DBusError error;

    dbus_error_init(&error);
    DBusConnection *conn = dbus_bus_get(DBUS_BUS_SYSTEM, &error);
 
    if (dbus_error_is_set(&error)) 
    {
        g_error("Cannot get System BUS connection: %s", error.message);
        dbus_error_free(&error);
        return;
    }

    dbus_connection_setup_with_g_main(conn, NULL);
 
    std::string rule = RULE;
    dbus_bus_add_match(conn, rule.c_str(), &error);
 
    if (dbus_error_is_set(&error)) 
    {
        g_error("Cannot add D-BUS match rule, cause: %s", error.message);
        dbus_error_free(&error);
        return;
    }
 
    dbus_connection_add_filter(conn, signal_filter, NULL, NULL);

}

void* ThreadFunc(void* ptr)
{
    InitDbus();

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
 
    g_main_loop_run(loop);

    return NULL;
}


