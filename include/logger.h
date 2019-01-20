#include <stdio.h>

#define LOG_TRACE 5
#define LOG_DEBUG 4
#define LOG_INFO 3
#define LOG_WARN 2
#define LOG_ERROR 1
#define LOG_FATAL 0

#define BLD	"\033[1m"

#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"
#define RESET "\x1B[0m"

#define LOG_LEVEL LOG_INFO

#define log_trace(fmt, ...) \
	do { if (LOG_LEVEL >= LOG_TRACE) fprintf(stderr, "[TRACE] %s:%d:%s(): " fmt "\n", __FILE__, \
		__LINE__, __func__, __VA_ARGS__); } while (0)

#define log_debug(fmt, ...) \
	do { if (LOG_LEVEL >= LOG_DEBUG) fprintf(stderr, BLU "[DEBUG]" RESET " %s:%d:%s(): " fmt "\n", __FILE__, \
		__LINE__, __func__, __VA_ARGS__); } while (0)

#define log_info(fmt, ...) \
	do { if (LOG_LEVEL >= LOG_INFO) fprintf(stderr, CYN "[INFO]" RESET " %s(): " fmt "\n", \
		__func__, __VA_ARGS__); } while (0)

#define log_warn(fmt, ...) \
	do { if (LOG_LEVEL >= LOG_WARN) fprintf(stderr, YEL "[WARN]" RESET " %s:%d:%s(): " fmt "\n", __FILE__, \
		__LINE__, __func__, __VA_ARGS__); } while (0)

#define log_error(fmt, ...) \
	do { if (LOG_LEVEL >= LOG_ERROR) fprintf(stderr, RED "[ERROR]" RESET " %s:%d:%s(): " fmt "\n", __FILE__, \
		__LINE__, __func__, __VA_ARGS__); } while (0)

#define log_fatal(fmt, ...) \
	do { if (LOG_LEVEL >= LOG_FATAL) fprintf(stderr, BLD RED "[FATAL] %s:%d:%s(): " fmt RESET"\n", __FILE__, \
		__LINE__, __func__, __VA_ARGS__); } while (0)
