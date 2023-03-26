#include "event.h"

#include "logger.h"

typedef struct Event
{
    EventCode code;
    PFN_eventCallback callback;
} Event;

#define MAX_EVENT_COUNT 16000
static Event allEvents[MAX_EVENT_COUNT];
static u32 eventsAdded;

b8 eventsStartup()
{
    return TRUE;
}

void eventsShutdown()
{

}

void addEvent(EventCode code, PFN_eventCallback callback)
{
    allEvents[eventsAdded].code = code;
    allEvents[eventsAdded].callback = callback;

    eventsAdded++;
}

void removeEvent(EventCode code)
{
    for(u32 i = 0; i < eventsAdded; i++)
    {
        if(allEvents[i].code == code)
        {
            allEvents[i].code = EVENT_CODE_NONE;
            allEvents[i].callback = PNULL;

            for(u32 j = i; j < eventsAdded; j++)
            {
                allEvents[j] = allEvents[j + 1];
            }

            eventsAdded--;
            return;
        }
    }

    CF_ERROR("Event to remove not found!");
}

void fireEvent(EventCode code, void* data)
{
    for(u32 i = 0; i < eventsAdded; i++)
    {
        if(allEvents[i].code == code)
        {
            allEvents[i].callback(code, data);
        }
    }
}