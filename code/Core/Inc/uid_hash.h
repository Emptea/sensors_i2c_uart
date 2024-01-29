#include "main.h"

#define UID_ADDR 0x1FFFF7AC
#define UID      (*(uint32_t *)UID_ADDR)

uint32_t uid_hash();
