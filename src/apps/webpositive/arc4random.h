#ifndef ARC4RANDOM_H
#define ARC4RANDOM_H

#include <sys/types.h>

void arc4random_buf(void* buf, size_t n);

#endif // ARC4RANDOM_H
