# ngx_google_perftools_profiler_module
A better tools for nginx with google_perftools

This module is inspired by official [`ngx_google_perftools_module`](https://nginx.org/en/docs/ngx_google_perftools_module.html).

# Installation

At first you must install [gperftools](https://github.com/gperftools/gperftools) library, For example:

```apt
sudo apt-get install google-perftools
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
google_perftools profiler=all name=/path/to/file interval=1s;
```
Profiles will be stored as `/path/to/file.<worker_pid>`.

# See Also


