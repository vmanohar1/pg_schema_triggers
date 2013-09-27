/*-------------------------------------------------------------------------
 *
 * trigger_funcs.h
 *    Declarations for event trigger creation and invocation functions.
 *
 *
 * pg_schema_triggers/trigger_funcs.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef SCHEMA_TRIGGERS_TRIGGER_FUNCS_H
#define SCHEMA_TRIGGERS_TRIGGER_FUNCS_H


#include "postgres.h"


typedef struct EventInfo {
	char *eventname;
} EventInfo;


Oid CreateEventTriggerEx(const char *eventname, const char *trigname, Oid trigfunc);
void FireEventTriggers(const char *eventname, const char *tag, EventInfo *info);
EventInfo* GetCurrentEventInfo(const char *eventname);


#endif	/* SCHEMA_TRIGGERS_TRIGGER_FUNCS_H */