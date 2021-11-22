#ifndef UTILS_H
#define UTILS_H

#include "../improvc/improvc.h"
#include "../network/network.h"
#include <errno.h>

void print_resume(router_t* router);
void die(char *s);

#include "utils.c"

#endif