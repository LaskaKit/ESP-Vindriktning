#include <string.h>

#include "cmdproc.h"


static const cmd_t *find_cmd(const cmd_t * commands, const char *name)
{
    const cmd_t *cmd;
    for (cmd = commands; cmd->cmd != NULL; cmd++) {
        if (strcmp(name, cmd->name) == 0) {
            return cmd;
        }
    }
    return NULL;
}

static int split(char *input, char *args[], int maxargs)
{
    int argc = 0;
    char *next = strtok(input, " ");
    while ((next != NULL) && (argc < maxargs)) {
        args[argc++] = next;
        next = strtok(NULL, " ");
    }
    return argc;
}

int cmd_process(const cmd_t * commands, char *line)
{
    char *argv[CMD_MAX_ARGS];

    // parse line
    int argc = split(line, argv, CMD_MAX_ARGS);
    if (argc == 0) {
        // no command present
        return CMD_NO_CMD;
    }
    // find matching entry
    const cmd_t *cmd = find_cmd(commands, argv[0]);
    if (cmd == NULL) {
        // no command found
        return CMD_UNKNOWN;
    }
    // execute
    int res = cmd->cmd(argc, argv);
    return res;
}
