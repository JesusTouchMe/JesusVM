#pragma once

#include "pch.h"

#define ArrayEquals(a1, len1, a2, len2) (memcmp(a1, a2, (len1 > len2) ? len1 : len2) == 0)