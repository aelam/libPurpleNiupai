/**
 * @file command.c NP command functions
 *
 * purple
 *
 * Purple is the legal property of its developers, whose names are too numerous
 * to list here.  Please refer to the COPYRIGHT file distributed with this
 * source distribution.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
 */
#include "internal.h"

#include "command.h"

static gboolean
is_num(const char *str)
{
	const char *c;
	for (c = str; *c; c++) {
		if (!(g_ascii_isdigit(*c)))
			return FALSE;
	}

	return TRUE;
}

NPCommand *
np_command_from_string(const char *string)
{
	NPCommand *cmd;
    char *tmp;
	char *param_start;

	g_return_val_if_fail(string != NULL, NULL);

    tmp = g_strdup(string);

	cmd = g_new0(NPCommand, 1);
    cmd->command = tmp;

	
	param_start = strchr(cmd->command, ':');

    if (param_start) {
        char *param;
		int c;
        
		*param_start++ = '\0';

        cmd->params = g_strsplit(param_start, ",", 2);

        for (c = 0; cmd->params[c]; c++);
		cmd->param_count = c;
        
        param = cmd->params[0];
                
        cmd->trId = is_num(param) ? atoi(param) : 0;
    } else {
        if(strcmp(string, "LOGIN_OK") == 0) {
            cmd->trId = 1;
        } else if (strcmp(string, "HEART") == 0) {
            cmd->trId = 2;
        } else {
            cmd->trId = 0;
        }
        
    }
    

	np_command_ref(cmd);

	return cmd;
}

static void
np_command_destroy(NPCommand *cmd)
{
	g_free(cmd->payload);
	g_free(cmd->command);
	g_strfreev(cmd->params);
	g_free(cmd);
}

NPCommand *
np_command_ref(NPCommand *cmd)
{
	g_return_val_if_fail(cmd != NULL, NULL);

	cmd->ref_count++;
	return cmd;
}

void
np_command_unref(NPCommand *cmd)
{
	g_return_if_fail(cmd != NULL);
	g_return_if_fail(cmd->ref_count > 0);

	cmd->ref_count--;

	if (cmd->ref_count == 0)
	{
		np_command_destroy(cmd);
	}
}

