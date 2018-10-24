

/***************************** Include Files *********************************/
#include <shell.h>
#include <string.h>
#include <fsl_debug_console.h>

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/
volatile uint8_t isGetCmd = false;
volatile uint8_t isPwdMode = false;
static const shell_command_t *shell_cmd_table;
uint32_t shell_cmdline_pos;
char shell_cmdline[SHELL_CMDLINE_SIZE];

/*****************************************************************************/

/** @brief 
 *		   
 *
 *  @param
 *  @return Void.
 *  @note
 */

void shell_task(void *arg) {
	(void)arg;
	if(isGetCmd) {
		shell_exec();
		shell_clear_buffer();		
		isGetCmd = false;
		LREP(SHELL_PROMPT);
	}
}

/*****************************************************************************/
/** @brief 
 *		   Push a command into the debug command queue
 *
 *  @param
 *  @return Void.
 *  @note
 */

uint8_t shell_push_command(uint8_t ch)
{
	if(ch != EOF)
	{
		//if (((char)ch != SHELL_CR) && (shell_cmdline_pos < SHELL_CMDLINE_SIZE))
		if (((char)ch != SHELL_CR) && (char)ch != SHELL_LF && 
				(shell_cmdline_pos < SHELL_CMDLINE_SIZE))
		{
			switch(ch)
			{
			case SHELL_BACKSPACE:
			case SHELL_DELETE:
			if (shell_cmdline_pos > 0U)
			{
				shell_cmdline_pos -= 1U;
				debug_putchar(SHELL_BACKSPACE);
				debug_putchar(' ');
				debug_putchar(SHELL_BACKSPACE);
			}
			break;

			default:
			if ((shell_cmdline_pos + 1U) < SHELL_CMDLINE_SIZE)
			{
				/* Only printable characters. */
				if (((char)ch >= SHELL_SPACE) && ((char)ch <= SHELL_DELETE))
				{
					shell_cmdline[shell_cmdline_pos] = (char)ch;
					shell_cmdline_pos++;
					if(isPwdMode == false) {
						debug_putchar((char)ch);
					} else {
						debug_putchar('*');
					}
				}
			}
			  break;
			}
		}
		else
		{
			shell_cmdline[shell_cmdline_pos] = '\0';
			debug_putchar(SHELL_CR);
			debug_putchar(SHELL_LF);
			isGetCmd = true;
			return true;
		}

		return true;
	}

	return true;
}
/*****************************************************************************/
/** @brief 
 *		   Pop a command out of the debug command queue
 *
 *  @param
 *  @return Debug command.
 *  @note
 */

void shell_pop_command(uint8_t*    pu8Command)
{

}





static int32_t shell_make_argv(char *cmdline, char *argv[])
{
	int32_t argc = 0;
	int32_t i;
	bool in_text_flag = false;

	if ((cmdline != NULL) && (argv != NULL)) {
		for (i = 0u; cmdline[i] != '\0'; ++i) {
			if (cmdline[i] == ' ') {
				in_text_flag = false;
				cmdline[i] = '\0';
			} else {
				if (argc < SHELL_ARGS_MAX) {
					if (in_text_flag == false) {
						in_text_flag = true;
						argv[argc] = &cmdline[i];
						argc++;
					}
				} else { /* Return argc.*/
				  break;
				}
			}
		}
		argv[argc] = 0;
	}
	return argc;
}

uint8_t shell_exec(void)
{
	uint8_t err = 0;
	char *argv[SHELL_ARGS_MAX + 1u]; /* One extra for 0 terminator.*/
	int32_t argc;
	argc = shell_make_argv(shell_cmdline, argv);
	if (argc != 0)
	{
		const shell_command_t *cur_command = shell_cmd_table;
		while (cur_command->name)
		{
			//if (strcasecmp(cur_command->name, argv[0]) == 0) /* Command is found. */
			if (strcmp(cur_command->name, argv[0]) == 0) /* Command is found. */
			{
				if (((argc - 1u) >= cur_command->min_args) && ((argc - 1u) <= cur_command->max_args))
				{
					if (cur_command->cmd_ptr)
					{
					   ((void(*)(int32_t cmd_ptr_argc, char **cmd_ptr_argv))(cur_command->cmd_ptr))(argc, argv);
					}
				}
				else /* Wrong command syntax. */
				{
					LREP(SHELL_ERR_SYNTAX, argv[0]);
					err++;					
				}

				break;
			}
			cur_command++;
		}

		if (cur_command->name == 0)
		{
			LREP(SHELL_ERR_CMD, argv[0]);
			return SHELL_CMD_ERR_INVALID_CMD;
		}
	}
	if(err > 0)
		return SHELL_CMD_ERR_INVALID_SYNTAX;
	return SHELL_CMD_ERR_NONE;
}

void shell_help(void)
{
  const shell_command_t *cur_command = shell_cmd_table;

  while (cur_command->name)
  {
    LREP(">%7s %-14s- %s\r\n", cur_command->name,
                                      cur_command->syntax,
                                      cur_command->description);
    cur_command++;
  }
}


void shell_set_command(const char *cmd, uint32_t size) {
	if(size > SHELL_CMDLINE_SIZE) return;
	shell_cmdline_pos = size;
	memcpy((char*)shell_cmdline, (const char*)cmd, size);
	shell_cmdline[size] = '\0';
	isGetCmd = false;
}

void shell_push_buffer(const char *cmd, uint32_t size) {
	if(size > SHELL_CMDLINE_SIZE) return;
	shell_cmdline_pos = size;
	memcpy((char*)shell_cmdline, (const char*)cmd, size);
	shell_cmdline[size] = '\0';	
}

void shell_clear_buffer() {
	shell_cmdline_pos = 0u;
	shell_cmdline[0] = 0u;
}

void shell_init(const shell_command_t *const cmd_table, void (*init)(void))
{
	shell_clear_buffer();
	shell_cmd_table = cmd_table;
	init();
}

/*****************************************************************************/

