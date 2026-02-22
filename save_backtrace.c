/* See LICENSE file for copyright and license details. */
#include "internal.h"


#if defined(__GNUC__)
# pragma GCC diagnostic ignored "-Wsuggest-attribute=const"
#endif


WEAK int
liberror_save_backtrace(struct liberror_error *error)
{
	(void) error;
	return 0;
}
