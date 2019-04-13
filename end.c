/* See LICENSE file for copyright and license details. */
#include "internal.h"


void
liberror_end(const struct liberror_state *state)
{
	liberror_saved_backtrace = state->backtrace;
	liberror_have_error_ = state->have_error;
	errno = state->errnum;
	memcpy(&liberror_error_, &state->error, sizeof(liberror_error_));
}
