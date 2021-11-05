/***
 * Debug printf control.
 */


#ifndef DEBUG_H
#define DEBUG_H


#define DEBUG 1

#define debug_print(fmt, ...) do { if (DEBUG) printf("%s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, __VA_ARGS__); } while (0)


#endif
