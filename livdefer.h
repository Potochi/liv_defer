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

/* This macro sets up the defer stack. Should be used at the beginning of a
 * function definition */
#define LIV_DEFER_START_MARK(stack_size)                     \
	static const int _liv_defer_stack_size = stack_size; \
	void *_liv_defer_stack[_liv_defer_stack_size];       \
	_liv_defer_stack[0] = &&_liv_defer_return;           \
	volatile int _liv_defer_counter = 1

/* Defer the execution of some code. *name* should be unique in the local
 * scope */
#define LIV_DEFER(name, body)                                          \
	do {                                                           \
		_liv_defer_stack[_liv_defer_counter++] =               \
			&&LIV_CONCAT(_liv_defer_label_, name);         \
		if (_liv_defer_counter > _liv_defer_stack_size) {      \
			LIV_DEFER_ABORT_FUNCTION(                      \
				"%s:%d:1: Defer stack size exceeded. " \
				"Consider increasing it's size.\n",    \
				__FILE__, __LINE__);                   \
		}                                                      \
		break;                                                 \
		LIV_CONCAT(_liv_defer_label_, name)                    \
			: body;                                        \
		goto _liv_defer_stack_handler;                         \
	} while (0)

/* Execute the deferred code and return a value */
#define LIV_RETURN_DEFER(name, value)                             \
	do {                                                      \
		_liv_defer_stack[0] =                             \
			&&LIV_CONCAT(_defer_return_label_, name); \
		goto _liv_defer_stack_handler;                    \
		LIV_CONCAT(_defer_return_label_, name)            \
			: return value;                           \
	} while (0)

/* Bypass the defer stack and return a value without executing deferred code
 * blocks */
#define LIV_RETURN(value)     \
	do {                  \
		return value; \
	} while (0)

/* Jump and return the default value without executing the deferred code */
#define LIV_RETURN_DEFAULT              \
	do {                            \
		goto _liv_defer_return; \
	} while (0)

/* Execute the deferred code and return the default value */
#define LIV_DEFER_DEFAULT                      \
	do {                                   \
		goto _liv_defer_stack_handler; \
	} while (0)

/* This macro sets up the defer stack handler and the default return value.
 * Should be used at the end of a function */
#define LIV_DEFER_END_MARK(value)                                     \
	do {                                                          \
_liv_defer_stack_handler:                                             \
		if (_liv_defer_counter >= 0) {                        \
			goto *_liv_defer_stack[--_liv_defer_counter]; \
		}                                                     \
	} while (0);                                                  \
_liv_defer_return:                                                    \
	return value

#endif // LIVDEFER_H
