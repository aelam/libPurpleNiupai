/**
 * @file history.c NP history functions
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
#include "niupai.h"
#include "history.h"

NPHistory *
np_history_new(void)
{
	NPHistory *history = g_new0(NPHistory, 1);

	history->trId = 1;

	history->queue = g_queue_new();

	return history;
}

void
np_history_destroy(NPHistory *history)
{
	NPTransaction *trans;

	while ((trans = g_queue_pop_head(history->queue)) != NULL)
		np_transaction_destroy(trans);

	g_queue_free(history->queue);
	g_free(history);
}

NPTransaction *
np_history_find(NPHistory *history, unsigned int trId)
{
	NPTransaction *trans;
	GList *list;

	for (list = history->queue->head; list != NULL; list = list->next)
	{
		trans = list->data;
		if (trans->trId == trId) {
            purple_debug_info("np", "!!FOUND trans->trId: %d command : %s\n",trans->trId,trans->command);
			return trans;
        }
	}

	return NULL;
}

void
np_history_add(NPHistory *history, NPTransaction *trans)
{
	GQueue *queue;
	int max_elems;

	g_return_if_fail(history != NULL);
	g_return_if_fail(trans   != NULL);

	queue = history->queue;

	trans->trId = history->trId++;
    purple_debug_info("np", "!!trans->trId: %d command : %s\n",trans->trId,trans->command);
	g_queue_push_tail(queue, trans);

	if (trans->cmdproc->servconn->type == NP_SERVCONN_NS)
		max_elems = NP_NS_HIST_ELEMS;
	else
		max_elems = NP_SB_HIST_ELEMS;

	if (queue->length > max_elems)
	{
		trans = g_queue_pop_head(queue);
		np_transaction_destroy(trans);
	}
}

