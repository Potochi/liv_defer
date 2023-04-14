/* vim: set ts=8 sw=8: */
#define LIV_DEFER_ABORT_FUNCTION err_fn

#include "../livdefer.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void err_fn(const char *fmt, ...)
{
	va_list params;
	va_start(params, fmt);
	vfprintf(stderr, fmt, params);
	exit(1);
}

int hello_world_all()
{
	LIV_DEFER_START_MARK(10);

	int hello = 100;

	LIV_DEFER(world, printf(", world!\n"));
	LIV_DEFER(hello, printf("Hello"));

	LIV_DEFER_END_MARK(hello);
}

int hello_world_first()
{
	LIV_DEFER_START_MARK(10);

	int hello = 100;

	LIV_DEFER(world, printf(", world!\n"));

	LIV_DEFER_DEFAULT;

	LIV_DEFER(hello, printf("Hello"));

	LIV_DEFER_END_MARK(hello);
}

int main(int argc, char **argv)
{
	int result = hello_world_all();
	printf("hello_world returned: %d\n", result);

	result = hello_world_first();
	printf("hello_world returned: %d\n", result);

	return 0;
}
