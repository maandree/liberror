/* See LICENSE file for copyright and license details. */
#include "internal.h"


struct liberror_error *
liberror_get_error(void)
{
	if (liberror_have_error_)
		return &liberror_error_;
	return NULL;
}
