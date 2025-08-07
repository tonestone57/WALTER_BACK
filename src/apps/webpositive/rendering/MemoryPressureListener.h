#ifndef MEMORY_PRESSURE_LISTENER_H
#define MEMORY_PRESSURE_LISTENER_H

#include <MemoryPressureListener.h>

class BWebPage;

class MemoryPressureListener : public BMemoryPressureListener {
public:
    MemoryPressureListener(BWebPage* page);
    virtual ~MemoryPressureListener();

    virtual void MemoryPressureChanged(int32 level);

private:
    BWebPage* fPage;
};

#endif // MEMORY_PRESSURE_LISTENER_H
