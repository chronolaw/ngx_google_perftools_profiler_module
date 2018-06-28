// Copyright (c) 2018 chronolaw

#ifndef _NGX_GOOGLE_PERFTOOLS_PROFILER_H_INCLUDED_
#define _NGX_GOOGLE_PERFTOOLS_PROFILER_H_INCLUDED_

#ifndef  __cplusplus
    #error "ngx_google_perftools_profiler_module needs C++ !"
#endif

extern "C" {
#include <ngx_config.h>
#include <ngx_core.h>
}

// #include <google/profiler.h>
// #include <google/heap-profiler.h>

extern "C" {

int ProfilerStart(u_char* fname);
void ProfilerStop(void);
void ProfilerRegisterThread(void);

void HeapProfilerStart(u_char* prefix);
int IsHeapProfilerRunning();
void HeapProfilerDump(u_char *reason);
void HeapProfilerStop();

}

// copied from gperftools/src/base/commandlineflags.h

#define DECLARE_VARIABLE(type, name)                                          \
  namespace FLAG__namespace_do_not_use_directly_use_DECLARE_##type##_instead {  \
  extern type FLAGS_##name;                                \
  }                                                                           \
  using FLAG__namespace_do_not_use_directly_use_DECLARE_##type##_instead::FLAGS_##name

// var for time interval
typedef int64_t             int64;
DECLARE_VARIABLE(int64, heap_profile_time_interval);

#endif  // _NGX_GOOGLE_PERFTOOLS_PROFILER_H_INCLUDED_
