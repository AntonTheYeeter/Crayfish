#ifndef EVENT_H
#define EVENT_H

#include "def.h"

typedef enum EventType
{
    EVENT_TYPE_NONE,
    EVENT_TYPE_WINDOW_RESIZED,

    EVENT_TYPE_MAX_ENUM
} EventType;

typedef struct Event
{
    EventType type;
} Event;

static Event e;

Event pollEvents();
void resetEvent(Event* event);
void fireEvent(EventType type);

#endif