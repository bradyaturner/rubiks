#include <stdio.h>

#define LOG_TRACE 5
#define LOG_DEBUG 4
#define LOG_INFO 3
#define LOG_WARN 2
#define LOG_ERROR 1
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

#define log_error(fmt, ...) \
	do { if (LOG_LEVEL >= LOG_ERROR) fprintf(stderr, "[ERROR] %s:%d:%s(): " fmt, __FILE__, \
		__LINE__, __func__, __VA_ARGS__); } while (0)

#define log_fatal(fmt, ...) \
	do { if (LOG_LEVEL >= LOG_FATAL) fprintf(stderr, "[FATAL] %s:%d:%s(): " fmt, __FILE__, \
		__LINE__, __func__, __VA_ARGS__); } while (0)
