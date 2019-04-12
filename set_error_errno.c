/* See LICENSE file for copyright and license details. */
#include "internal.h"


void
liberror_set_error_errno(const char description[256], const char source[64], int code)
{
	liberror_set_error(description, source, "errno", (long long int)code);
}
