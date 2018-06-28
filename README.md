# ngx_google_perftools_profiler_module
A better tools for nginx with google_perftools

This module is inspired by official [`ngx_google_perftools_module`](https://nginx.org/en/docs/ngx_google_perftools_module.html).

[![Build Status](https://travis-ci.org/chronolaw/ngx_google_perftools_profiler_module.svg?branch=master)](https://travis-ci.org/chronolaw/ngx_google_perftools_profiler_module)

# Installation

At first you must install [gperftools](https://github.com/gperftools/gperftools) library, For example:

```apt
sudo apt-get install google-perftools
sudo apt-get install libgoogle-perftools-dev
```

Then configure nginx, add this module:

```shell
./configure --add-module=path/to/ngx_google_perftools_profiler_module
make && make install
```

# Configuration

In `nginx.conf`:

```conf
# profiler => cpu|heap|all
# name => file that keeps profiling information
# interval => time interval for dump heap profiling information, in seconds
google_perftools profiler=all name=ngx_prof interval=1s;
```
Profiles will be stored as `/path/to/file.<worker_pid>`.

Then you can use `pprof` to visualize results, for example:

```pprof
pprof --svg /usr/local/nginx/sbin/nginx ./ngx_prof.4342 >a.svg
pprof --svg /usr/local/nginx/sbin/nginx ./ngx_prof.4342.0007.heap >b.svg
```

# See Also

* [ngx_cpp_dev](https://github.com/chronolaw/ngx_cpp_dev) - Nginx cpp development kit, with the power of C++11 and Boost Library.
* [openresty_dev](https://github.com/chronolaw/openresty_dev) - OpenResty/Lua Programming
* [favorite-nginx](https://github.com/chronolaw/favorite-nginx) - Selected favorite nginx modules and resources.
* [annotated_nginx](https://github.com/chronolaw/annotated_nginx) - 注释nginx，学习研究源码

