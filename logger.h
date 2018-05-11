#include <stdio.h>

#define LOG_TRACE 4
#define LOG_DEBUG 3
#define LOG_INFO 2
#define LOG_WARN 1
#define LOG_FATAL 0

#define LOG_LEVEL LOG_INFO

#define log_trace(fmt, ...) \
	do { if (LOG_LEVEL >= LOG_TRACE) fprintf(stderr, "[TRACE] %s:%d:%s(): " fmt, __FILE__, \
		__LINE__, __func__, __VA_ARGS__); } while (0)

#define log_debug(fmt, ...) \
	do { if (LOG_LEVEL >= LOG_DEBUG) fprintf(stderr, "[DEBUG] %s:%d:%s(): " fmt, __FILE__, \
		__LINE__, __func__, __VA_ARGS__); } while (0)

#define log_info(fmt, ...) \
	do { if (LOG_LEVEL >= LOG_INFO) fprintf(stderr, "[INFO] %s:%d:%s(): " fmt, __FILE__, \
		__LINE__, __func__, __VA_ARGS__); } while (0)

#define log_warn(fmt, ...) \
	do { if (LOG_LEVEL >= LOG_WARN) fprintf(stderr, "[WARN] %s:%d:%s(): " fmt, __FILE__, \
		__LINE__, __func__, __VA_ARGS__); } while (0)

#define log_fatal(fmt, ...) \
	do { if (LOG_LEVEL >= LOG_FATAL) fprintf(stderr, "[FATAL] %s:%d:%s(): " fmt, __FILE__, \
		__LINE__, __func__, __VA_ARGS__); } while (0)
