#pragma once

// Wrapping header for google log for cross platform handling
#ifdef WIN32
// For rationale see : https://hpc.nih.gov/development/glog.html
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <windows.h>
#include <glog/logging.h>
#else // WIN32
#include <glog/logging.h>
#endif // WIN32
