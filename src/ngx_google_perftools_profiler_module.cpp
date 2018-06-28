// Copyright (c) 2018 chronolaw

// https://gperftools.github.io/gperftools/cpuprofile.html
// https://gperftools.github.io/gperftools/heapprofile.html
//
// sudo apt-get install google-perftools
// ./configure --add-module=ngx_google_perftools_profiler_module
//
// in nginx.conf:
// google_perftools profiler=all name=/tmp/ngx_prof interval=1s;
//
// /usr/local/bin/pprof --svg /usr/local/nginx/sbin/nginx ./ngx_prof.4342 >a.svg
// /usr/local/bin/pprof --svg /usr/local/nginx/sbin/nginx ./ngx_prof.4342.0007.heap >b.svg

#include "ngx_google_perftools_profiler_module.h"

static void *ngx_google_perftools_profiler_create_conf(ngx_cycle_t *cycle);
static char *ngx_google_perftools_profiler_init_conf(ngx_cycle_t *cycle, void *conf);
static ngx_int_t ngx_google_perftools_profiler_worker(ngx_cycle_t *cycle);

static char *ngx_google_perftools_profiler(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

// cpu, heap, or all
#define GPERF_CPU_PROFILER      0x01
#define GPERF_HEAP_PROFILER     0x02

typedef struct {
    ngx_uint_t  profiler;   // 1-cpu, 2-heap, 3-cpu and heap
    ngx_str_t   profiles;
    ngx_msec_t  time_interval;
} ngx_google_perftools_profiler_conf_t;


static ngx_command_t  ngx_google_perftools_profiler_commands[] = {

    { ngx_string("google_perftools"),
      //NGX_MAIN_CONF|NGX_DIRECT_CONF|NGX_CONF_TAKE2,
      NGX_MAIN_CONF|NGX_DIRECT_CONF|NGX_CONF_1MORE,
      ngx_google_perftools_profiler,  //ngx_conf_set_str_slot,
      0,
      offsetof(ngx_google_perftools_profiler_conf_t, profiles),
      NULL },

      ngx_null_command
};


static ngx_core_module_t  ngx_google_perftools_profiler_module_ctx = {
    ngx_string("google_perftools_profiler"),
    ngx_google_perftools_profiler_create_conf,
    ngx_google_perftools_profiler_init_conf
};


ngx_module_t  ngx_google_perftools_profiler_module = {
    NGX_MODULE_V1,
    &ngx_google_perftools_profiler_module_ctx,      /* module context */
    ngx_google_perftools_profiler_commands,         /* module directives */
    NGX_CORE_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    ngx_google_perftools_profiler_worker,           /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};


static void *
ngx_google_perftools_profiler_create_conf(ngx_cycle_t *cycle)
{
    ngx_google_perftools_profiler_conf_t  *gptcf;

    gptcf = (ngx_google_perftools_profiler_conf_t*)
            ngx_pcalloc(cycle->pool, sizeof(ngx_google_perftools_profiler_conf_t));
    if (gptcf == NULL) {
        return NULL;
    }

    /*
     * set by ngx_pcalloc()
     *
     *     gptcf->profiles = { 0, NULL };
     */

    gptcf->profiler = NGX_CONF_UNSET_UINT;
    gptcf->time_interval = NGX_CONF_UNSET_MSEC;

    return gptcf;
}

static char *
ngx_google_perftools_profiler_init_conf(ngx_cycle_t *cycle, void *conf)
{
    ngx_google_perftools_profiler_conf_t  *gptcf;

    gptcf = (ngx_google_perftools_profiler_conf_t*) conf;

    // default is no cpu or heap
    ngx_conf_init_uint_value(gptcf->profiler, 0);

    // default is 0 seconds for heap
    ngx_conf_init_msec_value(gptcf->time_interval, 0);

    return NGX_CONF_OK;
}

static char *
ngx_google_perftools_profiler(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_google_perftools_profiler_conf_t  *gptcf;
    ngx_str_t                             *value;
    ngx_uint_t                             i;

    gptcf = (ngx_google_perftools_profiler_conf_t*) conf;

    value = (ngx_str_t*)(cf->args->elts);

    // profiler=cpu|heap|all name=xxx interval=xxx
    for (i = 1; i < cf->args->nelts; i++) {

        if (ngx_strncmp(value[i].data, "name=", 5) == 0) {

            value[i].data += 5;
            value[i].len -= 5;

            if (value[i].len == 0) {
                ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                        "invalid profile name \"%V\"", &value[i]);
                return (char*)NGX_CONF_ERROR;
            }

            // profile name
            gptcf->profiles = value[i];

            continue;

        } else if (ngx_strncmp(value[i].data, "profiler=", 9) == 0) {

            value[i].data += 9;
            value[i].len -= 9;

            if (ngx_strncmp(value[i].data, "cpu", 3) == 0) {
                gptcf->profiler = GPERF_CPU_PROFILER;
            } else if (ngx_strncmp(value[i].data, "heap", 4) == 0) {
                gptcf->profiler = GPERF_HEAP_PROFILER;
            } else if (ngx_strncmp(value[i].data, "all", 3) == 0) {
                gptcf->profiler = GPERF_CPU_PROFILER|GPERF_HEAP_PROFILER;
            } else {
                return (char*)NGX_CONF_ERROR;
            }

            continue;

        } else if (ngx_strncmp(value[i].data, "interval=", 9) == 0) {

            value[i].data += 9;
            value[i].len -= 9;

            gptcf->time_interval = ngx_parse_time(&value[i], 0);
            if (gptcf->time_interval == (ngx_msec_t) NGX_ERROR) {
                return (char*)NGX_CONF_ERROR;
            }

            continue;
        }

        ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                           "invalid parameter \"%V\"", &value[i]);
        return (char*)NGX_CONF_ERROR;
    }

    return NGX_CONF_OK;
}

static ngx_int_t
ngx_google_perftools_profiler_worker(ngx_cycle_t *cycle)
{
    u_char                                  *profile;
    ngx_google_perftools_profiler_conf_t    *gptcf;

    gptcf = (ngx_google_perftools_profiler_conf_t *)
                ngx_get_conf(cycle->conf_ctx, ngx_google_perftools_profiler_module);

    if (gptcf->profiles.len == 0 || gptcf->profiler == 0) {
        return NGX_OK;
    }

    profile = (u_char*)
            ngx_alloc(gptcf->profiles.len + NGX_INT_T_LEN + 2, cycle->log);
    if (profile == NULL) {
        return NGX_OK;
    }

    // profile
    ngx_sprintf(profile, "%V.%d%Z", &gptcf->profiles, ngx_pid);

    // cpu profiler
    if (gptcf->profiler & GPERF_CPU_PROFILER) {

        if (getenv("CPUPROFILE")) {
            /* disable inherited Profiler enabled in master process */
            ProfilerStop();
        }

        if (ProfilerStart(profile)) {
            /* start ITIMER_PROF timer */
            ProfilerRegisterThread();

        } else {
            ngx_log_error(NGX_LOG_CRIT, cycle->log, ngx_errno,
                          "ProfilerStart(%s) failed", profile);
        }
    }

    // heap profiler
    if (gptcf->profiler & GPERF_HEAP_PROFILER) {

        if (getenv("HEAPPROFILE")) {
            /* disable inherited Profiler enabled in master process */
            HeapProfilerStop();
        }

        // change interval with C++ var
        FLAGS_heap_profile_time_interval = gptcf->time_interval / 1000;

        HeapProfilerStart(profile);
    }

    ngx_free(profile);

    return NGX_OK;
}

// The code below is for lua-resty-gperftoos

/////////////////////////////////////////////////////////////////

void
ngx_lua_ffi_cpu_profiler_start(u_char* profile)
{
    if (ProfilerStart(profile)) {
        ProfilerRegisterThread();
    }
}

void
ngx_lua_ffi_cpu_profiler_stop()
{
    ProfilerStop();
}

void
ngx_lua_ffi_heap_profiler_start(u_char* profile, int interval)
{
    FLAGS_heap_profile_time_interval = interval;

    HeapProfilerStart(profile);
}

void
ngx_lua_ffi_heap_profiler_dump(u_char* reason)
{
    if (IsHeapProfilerRunning()) {
        HeapProfilerDump(reason);
    }
}

void
ngx_lua_ffi_heap_profiler_stop()
{
    if (IsHeapProfilerRunning()) {
        HeapProfilerStop();
    }
}

/////////////////////////////////////////////////////////////////
