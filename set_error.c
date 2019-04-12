/* See LICENSE file for copyright and license details. */
#include "internal.h"


void
liberror_set_error(const char description[256], const char source[64], const char code_group[64], long long int code)
{
	struct liberror_error *cause;
	struct liberror_error *error;
	int have_cause;

	cause = liberror_get_error();
	have_cause = !!cause;
	cause = liberror_copy_error(cause);

	error = &liberror_error_;
	liberror_have_error_ = 1;

	memset(error, 0, sizeof(*error));

	if (*description) {
		stpcpy(error->description, description);
	} else if (!strcmp(code_group, "errno")) {
		if (code >= (long long int)INT_MIN && code <= (long long int)INT_MAX)
			strerror_r((int)code, error->description, sizeof(error->description));
	}
	stpcpy(error->source, source);
	stpcpy(error->code_group, code_group);
	error->code = code;
	liberror_save_backtrace_(error);
	error->cause = cause;
	error->failed_to_allocate_cause = have_cause && !cause;
}
