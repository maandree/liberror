/* See LICENSE file for copyright and license details. */
#include "internal.h"


static void
print_error(struct liberror_error *error, FILE *fp, char *prefix)
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

	*strchr(prefix, '\0') = ' ';

	switch (error->details_type) {
	case LIBERROR_DETAILS_USER:
		if (error->details.user.print_data)
			error->details.user.print_data(error->details.user.data, fp, prefix);
		break;

	case LIBERROR_DETAILS_ONE_FILE:
		if (error->details.one_file.fd >= 0 || error->details.one_file.name) {
			fprintf(fp, "%sDetails:\n", prefix);
			if (error->details.one_file.name) {
				fprintf(fp, "%s  %s name: %s\n", prefix,
				        error->details.one_file.role, error->details.one_file.name);
			}
			if (error->details.one_file.fd >= 0) {
				fprintf(fp, "%s  %s descriptor: %i\n", prefix,
				        error->details.one_file.role, error->details.one_file.fd);
			}
		}
		break;

	case LIBERROR_DETAILS_TWO_FILES:
		if (error->details.two_files.fd1 >= 0 || error->details.two_files.name1 ||
		    error->details.two_files.fd2 >= 0 || error->details.two_files.name2) {
			fprintf(fp, "%sDetails:\n", prefix);
			if (error->details.two_files.fd1 >= 0) {
				fprintf(fp, "%s  %s descriptor: %i\n", prefix,
				        error->details.two_files.role1, error->details.two_files.fd1);
			}
			if (error->details.two_files.name1) {
				fprintf(fp, "%s  %s name: %s\n", prefix,
				        error->details.two_files.role1, error->details.two_files.name1);
			}
			if (error->details.two_files.fd2 >= 0) {
				fprintf(fp, "%s  %s descriptor: %i\n", prefix,
				        error->details.two_files.role2, error->details.two_files.fd2);
			}
			if (error->details.two_files.name2) {
				fprintf(fp, "%s  %s name: %s\n", prefix,
				        error->details.two_files.role2, error->details.two_files.name2);
			}
		}
		break;

	case LIBERROR_DETAILS_NONE:
	default:
		break;
	}

	liberror_print_backtrace(error, fp, prefix);
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
	print_error(err, fp, prefix);

	while ((err = err->cause)) {
		*p = *q = '\0';
		fprintf(fp, "%sCaused by:\n", prefix);
		*p = ' ';
		print_error(err, fp, prefix);
	}

	if (reset) {
		if (error)
			liberror_free_error(error);
		else
			liberror_reset_error();
	}
}
