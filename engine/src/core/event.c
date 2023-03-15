#include "event.h"

Event pollEvents()
{
    return e;
}

void resetEvent(Event* event)
{
    e.type = EVENT_TYPE_NONE;
    event->type = EVENT_TYPE_NONE;
}

void fireEvent(EventType type)
{
    e.type |= type;
}