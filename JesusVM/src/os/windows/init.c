#include "os/init.h"

SYSTEM_INFO systemInfo;

void OSInit() {
	GetSystemInfo(&systemInfo);
}