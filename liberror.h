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
	/**
	 * No details
	 */
	LIBERROR_DETAILS_NONE,

	/**
	 * User-specific details
	 */
	LIBERROR_DETAILS_USER,

	/**
	 * Details for function that operates
	 * on a single file
	 */
	LIBERROR_DETAILS_ONE_FILE,

	/**
	 * Details for function that operates
	 * on two files
	 */
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
		/**
		 * The library that defines the data
		 */
		const char *library;

		/**
		 * The data for the details
		 */
		void *data;

		/**
		 * Function that is used to deallocate `.data`
		 * 
		 * @param  data  The pointer `.data`
		 */
		void (*free_data)(void *);

		/**
		 * Function that is used to copy `.data`
		 * 
		 * @param   data  The pointer `.data`
		 * @return        Copy of `data`, `NULL` on failure
		 */
		void *(*copy_data)(void *);

		/**
		 * Function that is used to print `.data`
		 * 
		 * @param  data    The pointer `.data`, may be `NULL`
		 * @param  fp      File to print the details to, may not be `NULL`
		 * @param  prefix  Text to print at the beginning of each line,
		 *                 may not be `NULL`
		 */
		void (*print_data)(void *, FILE *, const char *);
	} user;

	/**
	 * For `LIBERROR_DETAILS_ONE_FILE`
	 */
	struct {
		/**
		 * The specified file descriptor,
		 * negative if none
		 */
		int fd;

		/**
		 * The filename or other string that
		 * identifies the file, `NULL` if
		 * none
		 */
		char *name;

		/**
		 * Description of the file's role in the
		 * function call, for example "Directory"
		 * for the mkdir(3) function
		 */
		const char *role;
	} one_file;

	/**
	 * For `LIBERROR_DETAILS_TWO_FILES`
	 */
	struct {
		/**
		 * The first specified file descriptor,
		 * negative if none
		 */
		int fd1;

		/**
		 * The second specified file descriptor,
		 * negative if none
		 */
		int fd2;

		/**
		 * The filename or other string that
		 * identifies the first file, `NULL` if
		 * none
		 */
		char *name1;

		/**
		 * The filename or other string that
		 * identifies the second file, `NULL`
		 * if none
		 */
		char *name2;

		/**
		 * Description of the role of the first
		 * file in the function call, for example
		 * "Target" for the symlink(3) function
		 */
		const char *role1;

		/**
		 * Description of the role of the first
		 * file in the function call, for example
		 * "Link" for the symlink(3) function
		 */
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
 * Saved error state for a thread
 * 
 * This structure shall be regardes as opaque
 */
struct liberror_state {
	/**
	 * The backtrace for the next error
	 */
	struct liberror_backtrace *backtrace;

	/**
	 * The thread's error
	 */
	struct liberror_error error;

	/**
	 * Whether the thread hade an error
	 */
	int have_error;

	/**
	 * The thread's value of `errno`
	 */
	int errnum;
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
 * Get backtrace and save backtrace
 * 
 * This function will never change `errno`
 * 
 * Note: this library does not actually save
 * a backtrace, `-lerror-backtrace` is needed
 * for that functionallity (it will replace
 * some things in this library, so no other
 * action is required)
 * 
 * @param   error  The error the backtrace shall be stored in,
 *                 if `NULL`, the backtrafe is saved for the
 *                 next error in the thread
 * @return         0 on success, -1 on failure
 */
int liberror_save_backtrace(struct liberror_error *);

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

/**
 * Save the thread's liberror error, pending backtrace,
 * and `errno`, and then reset the error information
 * for the thread
 * 
 * Asynchronously called functions such as signal handlers
 * should call this function the first thing they do
 * 
 * @param  state  Output parameter for the error state
 */
void liberror_start(struct liberror_state *);

/**
 * Restore the thread's liberror error, pending backtrace,
 * and `errno`
 * 
 * Asynchronously called functions such as signal handlers
 * should call this function the last thing they do before
 * returning
 * 
 * @param  state  The saved error state to restore
 */
void liberror_end(const struct liberror_state *);


#endif
