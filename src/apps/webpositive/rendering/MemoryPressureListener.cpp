#include "rendering/MemoryPressureListener.h"
#include "API/WebPage.h"

MemoryPressureListener::MemoryPressureListener(BWebPage* page)
    :
    fPage(page)
{
    watch_memory_pressure(B_MEM_PRESSURE_NORMAL, this);
}

MemoryPressureListener::~MemoryPressureListener()
{
    stop_watching_memory_pressure(this);
}

void
MemoryPressureListener::MemoryPressureChanged(int32 level)
{
    fPage->HandleMemoryPressure(level);
}
