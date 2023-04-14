/* vim: set ts=8 sw=8: */

#ifndef LIVDEFER_H
#define LIVDEFER_H

/* This function is used to signal to the program that an error has occured.
 * The function signature should be "void defer_error(const char* fmt, ...)".
 *
 * This signals a serious issue and the execution of the program should not
 * continue.
 */
#ifndef LIV_DEFER_ABORT_FUNCTION
#error "LivDefer abort function not defined."
#endif

#define _LIV_CONCAT(A, B) A##B
#define LIV_CONCAT(A, B) _LIV_CONCAT(A, B)

/* Names used by the library. In case of collisions modify here or define
 * them before including the header */

#ifndef N_LIV_DEFER_STACK_SIZE
#define N_LIV_DEFER_STACK_SIZE _liv_defer_stack_size
#endif

#ifndef N_LIV_DEFER_STACK
#define N_LIV_DEFER_STACK _liv_defer_stack
#endif

#ifndef N_LIV_DEFER_COUNTER
#define N_LIV_DEFER_COUNTER _liv_defer_counter
#endif

#ifndef N_LIV_DEFER_RETURN
#define N_LIV_DEFER_RETURN _liv_defer_return
#endif

#ifndef N_LIV_DEFER_LABEL
#define N_LIV_DEFER_LABEL _liv_defer_label
#endif

#ifndef N_LIV_DEFER_STACK_HANDLER
#define N_LIV_DEFER_STACK_HANDLER _liv_defer_stack_handler
#endif

/* This macro sets up the defer stack. Should be used at the beginning of a
 * function definition */
#define LIV_DEFER_START_MARK(stack_size)                      \
	static const int N_LIV_DEFER_STACK_SIZE = stack_size; \
	void *N_LIV_DEFER_STACK[N_LIV_DEFER_STACK_SIZE];      \
	N_LIV_DEFER_STACK[0] = &&N_LIV_DEFER_RETURN;          \
	volatile int N_LIV_DEFER_COUNTER = 1

/* Defer the execution of some code. *name* should be unique in the local
 * scope */
#define LIV_DEFER(name, body)                                          \
	do {                                                           \
		N_LIV_DEFER_STACK[N_LIV_DEFER_COUNTER++] =             \
			&&LIV_CONCAT(N_LIV_DEFER_LABEL, name);         \
		if (N_LIV_DEFER_COUNTER > N_LIV_DEFER_STACK_SIZE) {    \
			LIV_DEFER_ABORT_FUNCTION(                      \
				"%s:%d:1: Defer stack size exceeded. " \
				"Consider increasing it's size.\n",    \
				__FILE__, __LINE__);                   \
		}                                                      \
		break;                                                 \
		LIV_CONCAT(N_LIV_DEFER_LABEL, name)                    \
			: body;                                        \
		goto N_LIV_DEFER_STACK_HANDLER;                        \
	} while (0)

/* Execute the deferred code and return a value */
#define LIV_RETURN_DEFER(name, value)                                          \
	do {                                                                   \
		N_LIV_DEFER_STACK[0] = &&LIV_CONCAT(N_LIV_DEFER_RETURN, name); \
		goto N_LIV_DEFER_STACK_HANDLER;                                \
		LIV_CONCAT(_defer_return_label_, name)                         \
			: return value;                                        \
	} while (0)

/* Bypass the defer stack and return a value without executing deferred code
 * blocks */
#define LIV_RETURN(value)     \
	do {                  \
		return value; \
	} while (0)

/* Jump and return the default value without executing the deferred code */
#define LIV_RETURN_DEFAULT               \
	do {                             \
		goto N_LIV_DEFER_RETURN; \
	} while (0)

/* Execute the deferred code and return the default value */
#define LIV_DEFER_DEFAULT                       \
	do {                                    \
		goto N_LIV_DEFER_STACK_HANDLER; \
	} while (0)

/* This macro sets up the defer stack handler and the default return value.
 * Should be used at the end of a function */
#define LIV_DEFER_END_MARK(value)                                       \
	do {                                                            \
N_LIV_DEFER_STACK_HANDLER:                                              \
		if (N_LIV_DEFER_COUNTER >= 0) {                         \
			goto *N_LIV_DEFER_STACK[--N_LIV_DEFER_COUNTER]; \
		}                                                       \
	} while (0);                                                    \
N_LIV_DEFER_RETURN:                                                     \
	return value

#endif // LIVDEFER_H
