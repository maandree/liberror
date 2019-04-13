/* See LICENSE file for copyright and license details. */
#include "internal.h"


void
liberror_start(struct liberror_state *state)
{
	state->backtrace = liberror_saved_backtrace;
	liberror_saved_backtrace = NULL;
	state->have_error = liberror_have_error_;
	liberror_have_error_ = 0;
	state->errnum = errno;
	errno = 0;
	memcpy(&state->error, &liberror_error_, sizeof(liberror_error_));
	memset(&liberror_error_, 0, sizeof(liberror_error_));
}
