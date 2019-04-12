/* See LICENSE file for copyright and license details. */
#ifndef LIBERROR_H
#define LIBERROR_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>


/**
 * Opaque backtrace structure
 */
struct liberror_backtrace;

/**
 * Value that specifies which feild in a
 * `union liberror_details` that is used
 */
enum liberror_details_type {
	LIBERROR_DETAILS_NONE,
	LIBERROR_DETAILS_USER,
	LIBERROR_DETAILS_ONE_FILE,
	LIBERROR_DETAILS_TWO_FILES
};

/**
 * Error details
 */
union liberror_details {
	/**
	 * For `LIBERROR_DETAILS_USER`
	 */
	struct {
		const char *library;
		void *data;
		void (*free_data)(void *);
		void *(*copy_data)(void *);
	} user;

	/**
	 * For `LIBERROR_DETAILS_ONE_FILE`
	 */
	struct {
		int fd;
		char *name;
		const char *role;
	} one_file;

	/**
	 * For `LIBERROR_DETAILS_TWO_FILES`
	 */
	struct {
		int fd1;
		int fd2;
		char *name1;
		char *name2;
		const char *role1;
		const char *role2;
	} two_files;
};

/**
 * Error structure
 */
struct liberror_error {
	/**
	 * Backtrace for the error, `NULL` if the it could
	 * not be allocated or if the program is not linked
	 * with `-lerror-backtrace`
	 */
	struct liberror_backtrace *backtrace;

	/**
	 * Description of the error
	 */
	char description[256];

	/**
	 * The function that failed
	 */
	char source[64];

	/**
	 * Name of error code group, for normal `errno`
	 * errors this is "error", for getaddrinfo(3) errors
	 * this is "addrinfo", for custom errors it is the
	 * name of the library or application
	 */
	char code_group[64];

	/**
	 * The error code
	 */
	long long int code;

	/**
	 * The error that caused this error, `NULL` if
	 * none or it could not be allocated (if and only
	 * if so, `.failed_to_allocate_cause` will be set
	 * to a non-zero value, specifically 1)
	 */
	struct liberror_error *cause;

	/**
	 * Whether allocation of `.cause` failed
	 */
	int failed_to_allocate_cause;

	/**
	 * Whether the error is physically allocated
	 */
	int dynamically_allocated;

	/**
	 * Which value in `.details` that is used
	 */
	enum liberror_details_type details_type;

	/**
	 * Error detail
	 */
	union liberror_details details;
};


/**
 * Get the current error for the thread
 * 
 * @return  The current error, `NULL` if none
 */
struct liberror_error *liberror_get_error(void);

/**
 * Create a copy of an error
 * 
 * This function will only fail of enough memory
 * cannot be allocated, however `errno` will not
 * be changed
 * 
 * @param   error  The error to copy
 * @return         Copy of the error, `NULL` on failure
 */
struct liberror_error *liberror_copy_error(struct liberror_error *);

/**
 * Deallocate and error and the error stored as
 * its cause (recursively)
 * 
 * @param  error  The error to deallocate
 */
void liberror_free_error(struct liberror_error *);

/**
 * Deallocate the current error for the thread
 * and reset the error for the thread
 * 
 * This function shall be called after handling
 * the error
 */
void liberror_reset_error(void);

/**
 * Print the backtrace of an error
 * 
 * If the backtrace could not be allocated,
 * nothing will be printed
 * 
 * This function will never change `errno`
 * 
 * Note: this library does not actually save
 * a backtrace, `-lerror-backtrace` is needed
 * for that functionallity (it will replace
 * some things in this library, so no other
 * action is required)
 * 
 * @param  error   The error
 * @param  fp      The file to print the backtrace to
 * @param  indent  Text to print at the beginning of each line
 */
void liberror_print_backtrace(struct liberror_error *, FILE *, const char *);

/**
 * Set the current error for the thread
 * 
 * If the thread already has an error saved,
 * it will be stored as the cause of the new
 * error
 * 
 * @param  description  Description of the error, empty for default description
 * @param  source       The function that failed
 * @param  code_group   Name of error code group, for normal `errno` errors
 *                      this shall be "error", for getaddrinfo(3) errors
 *                      this shall be "addrinfo", for custom errors it shall
 *                      be the name of the library or application
 * @param  code         The error code
 */
void liberror_set_error(const char[256], const char[64], const char[64], long long int);

/**
 * Set the current error for the thread
 * 
 * This function can be used as an alternative
 * to `liberror_set_error` for `errno` errors
 * 
 * If the thread already has an error saved,
 * it will be stored as the cause of the new
 * error
 * 
 * @param  description  Description of the error, empty for default description
 * @param  source       The function that failed
 * @param  code         The `errno` value
 */
void liberror_set_error_errno(const char[256], const char[64], int);

/**
 * The an error, its backtrace, and its
 * cause (recursively)
 * 
 * If `error` is `NULL` and the thread does
 * not have any error set, this function
 * will not do anything
 * 
 * @param  error   The error, the thread's current error if `NULL`
 * @param  fp      Output stream, standard error if `NULL`
 * @param  reset   Whether `error` shall be deallocated, `error`
 *                 is null and `reset` is non-zero, the thread's
 *                 error will be reset
 * @param  prefix  Prefix for each printed line, ": " will be
 *                 appended to this prefix; if `NULL` or empty,
 *                 no prefix is used; this should normally be
 *                 `argv[0]` from the main() function
 */
void liberror_print_error(struct liberror_error *, FILE *, int, const char *);


#endif
