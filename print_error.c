/* See LICENSE file for copyright and license details. */
#include "internal.h"


static void
print_error_description(struct liberror_error *error, FILE *fp, const char *prefix)
{
	if (*error->description) {
		if (*error->source)
			fprintf(fp, "%sError in function %s: %s\n", prefix, error->source, error->description);
		else
			fprintf(fp, "%sError: %s\n", prefix, error->description);
	} else if (*error->source) {
		fprintf(fp, "%sError in function %s: %s error %lli\n", prefix, error->source, error->code_group, error->code);
	} else {
		fprintf(fp, "%sError: %s error %lli\n", prefix, error->code_group, error->code);
	}
}


void
liberror_print_error(struct liberror_error *error, FILE *fp, int reset, const char *prefix_)
{
	struct liberror_error *err = error;
	char *prefix = (char []){"    "};
	char *p, *q;

	if (!err) {
		err = liberror_get_error();
		if (!err)
			return;
	}

	if (prefix_ && *prefix_) {
		prefix = alloca(strlen(prefix) + sizeof(":     "));
		stpcpy(q = stpcpy(p = stpcpy(stpcpy(prefix, prefix_), ": "), "  "), "  ");
	} else {
		p = &prefix[0];
		q = &prefix[2];
	}

	if (!fp)
		fp = stderr;

	*p = *q = '\0';
	print_error_description(err, fp, prefix);

	*p = ' ';
	liberror_print_backtrace(err, fp, prefix);

	while ((err = err->cause)) {
		*p = *q = '\0';
		fprintf(fp, "%sCaused by:\n", prefix);

		*p = ' ';
		print_error_description(err, fp, prefix);
		
		*q = ' ';
		liberror_print_backtrace(err, fp, prefix);
	}

	if (reset) {
		if (error)
			liberror_free_error(error);
		else
			liberror_reset_error();
	}
}
