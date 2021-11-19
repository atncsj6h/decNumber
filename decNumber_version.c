#include "decNumber_version.h"

#define Q(str)    #str
#define QSTR(str) Q(str)

static const char* version_string = QSTR( VERSION_STRING );

const char* decNumber_version() {return version_string; }
