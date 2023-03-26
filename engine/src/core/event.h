#ifndef EVENT_H
#define EVENT_H

#include "def.h"

typedef enum EventCode
{
    EVENT_CODE_NONE,
    EVENT_CODE_WINDOW_CLOSED,
    EVENT_CODE_WINDOW_RESIZED,
    EVENT_CODE_WINDOW_MINIMIZED,
    EVENT_CODE_KEY_PRESSED,
    EVENT_CODE_KEY_RELEASED,
    EVENT_CODE_MOUSE_BUTTON_PRESSED,
    EVENT_CODE_MOUSE_MOVED,

    EVENT_CODE_MAX_ENUM
} EventCode;

typedef void (*PFN_eventCallback)(EventCode code, void* data);

b8 eventsStartup();
void eventsShutdown();

void addEvent(EventCode code, PFN_eventCallback callback);
void removeEvent(EventCode code);
void fireEvent(EventCode code, void* data);

#endif