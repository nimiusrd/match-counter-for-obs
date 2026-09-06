#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/bmem.h>
#include <util/dstr.h>

void *bmalloc(size_t size)
{
	return malloc(size);
}

void *brealloc(void *ptr, size_t size)
{
	return realloc(ptr, size);
}

void bfree(void *ptr)
{
	free(ptr);
}

void *bmemdup(const void *ptr, size_t size)
{
	void *copy = bmalloc(size);
	if (copy)
		memcpy(copy, ptr, size);
	return copy;
}

void dstr_catf(struct dstr *dst, const char *format, ...)
{
	va_list args;
	va_list args_copy;

	va_start(args, format);
	va_copy(args_copy, args);
	int length = vsnprintf(NULL, 0, format, args_copy);
	va_end(args_copy);

	if (length > 0) {
		size_t new_length = dst->len + (size_t)length;
		dstr_ensure_capacity(dst, new_length + 1);
		vsnprintf(dst->array + dst->len, (size_t)length + 1, format, args);
		dst->len = new_length;
	}

	va_end(args);
}
