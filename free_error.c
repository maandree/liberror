/* See LICENSE file for copyright and license details. */
#include "internal.h"


void
liberror_free_error(struct liberror_error *error)
{
	struct liberror_error *tmp;
	while (error) {
		if (error->backtrace && !--error->backtrace->refcount)
			free(error->backtrace);

		switch (error->details_type) {
		case LIBERROR_DETAILS_USER:
			if (error->details.user.data && error->details.user.free_data)
				error->details.user.free_data(error->details.user.data);
			break;

		case LIBERROR_DETAILS_ONE_FILE:
			free(error->details.one_file.name);
			break;

		case LIBERROR_DETAILS_TWO_FILES:
			free(error->details.two_files.name1);
			free(error->details.two_files.name2);
			break;

		case LIBERROR_DETAILS_NONE:
		default:
			break;
		}

		tmp = error;
		error = error->cause;
		if (tmp->dynamically_allocated)
			free(tmp);
	}
}
