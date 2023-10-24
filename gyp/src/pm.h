#ifndef _PM_H
#define _PM_H

#include <node.h>
#include <v8.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void InitPM();

v8::Handle<v8::Value> StartMonitoring(const v8::Arguments& args);
void Start();
v8::Handle<v8::Value> StopMonitoring(const v8::Arguments& args);
void Stop();

v8::Handle<v8::Value> RegisterNotifications(const v8::Arguments& args);
void Notify(char* msg);

#endif