#ifndef SHELL_H_
#define SHELL_H_

/***************************** Include Files *********************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/************************** Constant Definitions *****************************/
#define SHELL_PROMPT "SHELL> "
#define SHELL_CMDLINE_SIZE 256
#define SHELL_ARGS_MAX 16

#define SHELL_ERR_SYNTAX   ("Error: Invalid syntax for: %s\r\n")
#define SHELL_ERR_CMD      ("Error: No such command: %s\r\n")

#define SHELL_BACKSPACE    ((char)(0x08))  /* Backspace. */
#define SHELL_DELETE       ((char)(0x7F))  /* Delete. */
#define SHELL_CTRLC        ((char)(0x03))  /* Ctrl + C. */
#define SHELL_CR           ((char)(0x0D))  /* CR. */
#define SHELL_LF           ((char)(0x0A))  /* LF. */
#define SHELL_ESC          ((char)(0x1B))  /* Esc. */
#define SHELL_SPACE        ((char)(0x20))  /* Space. */

/**************************** Type Definitions *******************************/
typedef void(*shell_cmd_function_t)(int32_t argc, char **argv);

typedef enum SHELL_CMD_STT_ {
	SHELL_CMD_ERR_NONE = 0,
	SHELL_CMD_ERR_INVALID_SYNTAX,
	SHELL_CMD_ERR_INVALID_CMD
}SHELL_CMD_STT;

typedef struct shell_command
{
    char *name;          	/**< @brief Command name (null-terminated string). */
    int8_t                 	min_args;       /**< @brief Minimum number of arguments the command accepts.*/
    int8_t                 	max_args;       /**< @brief Maximum number of arguments the command accepts.*/
    shell_cmd_function_t   	cmd_ptr;        /**< @brief Pointer to the actual command function defined by
                                                *   the @ref shell_cmd_function_t type    */
    char *description;   	/**< @brief Brief description of the command (null-terminated string). @n
                                                * This field is used by the @ref shell_help() function.@n*/
    char *syntax;        	/**< @brief Syntax of the command (null-terminated string). @n
                                                * This field is used by the @ref shell_help() function.
                                                * The standard command line syntax information
                                                * which will be helpful to describe the possible command
                                                * line parameters in a help display is:
                                                * - @c [] = When a parameter is surrounded with square
                                                * brackets, this means the parameter is optional.
                                                * - @c <> = When a parameter is surrounded with angle
                                                * brackets, this means the parameter is required for
                                                * normal operations of command.
                                                * - @c | = The vertical bar means a choice between
                                                * parameter value is acceptable.*/
} shell_command_t;


void 		shell_init(const shell_command_t *const cmd_table, void (*init)(void));
void 		shell_clear_buffer();
uint8_t 	shell_exec(void);
void 		shell_help(void);
void 		shell_set_command(const char *cmd, uint32_t size) ;
void 		shell_push_buffer(const char *cmd, uint32_t size) ;
void 		shell_task(void *arg);
uint8_t		shell_push_command(uint8_t ch);

/************************** Variable Definitions *****************************/
extern uint32_t 	debug_port;
extern volatile uint8_t isPwdMode;

/*****************************************************************************/




#endif /* DEBUG_H_ */

