#ifdef __CONFIG_H
#else
#define __CONFIG_H

#include <stdlib.h>
#include <argp.h>
#include <stdbool.h>
#include <linux/limits.h>

static char module_id[MAX_MODULE_NAME + 1];
static char server_queue[MAX_QUEUE_NAME + 1];

#endif