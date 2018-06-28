-- Copyright (C) 2018 by chrono

local ffi = require "ffi"
local ffi_cdef = ffi.cdef
local ffi_C = ffi.C

ffi_cdef[[
typedef unsigned char u_char;

void ngx_lua_ffi_cpu_profiler_start(const u_char* profile);
void ngx_lua_ffi_cpu_profiler_stop();

void ngx_lua_ffi_heap_profiler_start(const u_char* profile, int interval);
void ngx_lua_ffi_heap_profiler_dump(const u_char* reason);
void ngx_lua_ffi_heap_profiler_stop();
]]

local proto = {
    _VERSION = '0.0.1',
    cpu_profiler = {},
    heap_profiler = {},
    }

proto.cpu_profiler.start = function(name)
    if type(name) ~= "string" then
        return nil, "profiler needs a name"
    end

    if #name == 0 then
        return nil, "profiler name can not be empty"
    end

    local profile = name .. '\0'
    ffi_C.ngx_lua_ffi_cpu_profiler_start(profile)

    return true
end

proto.cpu_profiler.stop = function()
    ffi_C.ngx_lua_ffi_cpu_profiler_stop()
    return true
end

proto.heap_profiler.start = function(name, n)
    if type(name) ~= "string" then
        return nil, "profiler needs a name"
    end

    if #name == 0 then
        return nil, "profiler name can not be empty"
    end

    local profile = name .. '\0'
    ffi_C.ngx_lua_ffi_heap_profiler_start(profile, tonumber(n) or 0)

    return true
end

proto.heap_profiler.dump = function(s)
    if type(s) ~= "string" then
        return nil, "needs a reason"
    end

    if #s == 0 then
        return nil, "reason can not be empty"
    end

    local reason = s .. '\0'
    ffi_C.ngx_lua_ffi_heap_profiler_dump(reason)

    return true
end

proto.heap_profiler.stop = function()
    ffi_C.ngx_lua_ffi_heap_profiler_stop()
    return true
end

return proto
