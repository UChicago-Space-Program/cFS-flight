/*
 * This is a compatibility header to build libcsp within the cFS environment.
 * It defines _GNU_SOURCE to enable POSIX extensions and includes headers
 * that are commonly missing in the libcsp source files when built under
 * the strict cFS build rules.
 */

#define _GNU_SOURCE

#include <endian.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
