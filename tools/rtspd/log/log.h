/**
 * Copyright (c) 2017 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `log.c` for details.
 */

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>

#define LOG_VERSION "0.1.0"

typedef void (*log_LockFn)(void *udata, int lock);

enum { LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL };

#define log_trace(...) log_log(LOG_TRACE, "rtspd", __VA_ARGS__)
#define log_debug(...) log_log(LOG_DEBUG, "rtspd", __VA_ARGS__)
#define log_info(...)  log_log(LOG_INFO,  "rtspd", __VA_ARGS__)
#define log_warn(...)  log_log(LOG_WARN,  "rtspd", __VA_ARGS__)
#define log_error(...) log_log(LOG_ERROR, "rtspd", __VA_ARGS__)
#define log_fatal(...) log_log(LOG_FATAL, "rtspd", __VA_ARGS__)

void log_set_udata(void *udata);
void log_set_lock(log_LockFn fn);
void log_set_fp(FILE *fp);
void log_set_level(int level);
void log_set_quiet(int enable);

void log_log(int level, const char *name, const char *fmt, ...);

#endif
