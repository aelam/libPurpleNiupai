//
//  httpconn.c
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-19.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#include "httpconn.h"
#include "npconfig.h"

typedef struct
{
	NPHttpConn *httpconn;
	char *body;
	size_t body_len;
} NPHttpQueueData;


static void
np_httpconn_process_queue(NPHttpConn *httpconn)
{
	httpconn->waiting_response = FALSE;
    
	if (httpconn->queue != NULL)
	{
		NPHttpQueueData *queue_data;
        
		queue_data = (NPHttpQueueData *)httpconn->queue->data;
        
		httpconn->queue = g_list_remove(httpconn->queue, queue_data);
        
		np_httpconn_write(queue_data->httpconn,
						   queue_data->body,
						   queue_data->body_len);
        
		g_free(queue_data->body);
		g_free(queue_data);
	}
}

static gboolean
np_httpconn_parse_data(NPHttpConn *httpconn, const char *buf,
						size_t size, char **ret_buf, size_t *ret_size,
						gboolean *error)
{

    // TODO
    return TRUE;
}

static void
read_cb(gpointer data, gint source, PurpleInputCondition cond)
{
    
}

static void
httpconn_write_cb(gpointer data, gint source, PurpleInputCondition cond)
{
    
}

static gboolean
write_raw(NPHttpConn *httpconn, const char *data, size_t data_len)
{
    
    NIDPRINT("=============> ");
    return TRUE;
}

static char *
np_httpconn_proxy_auth(NPHttpConn *httpconn)
{
    NIDPRINT("=============> ");

    return "np_httpconn_proxy_auth";
}

static gboolean
np_httpconn_poll(gpointer data)
{
    
    return TRUE;
}

gssize
np_httpconn_write(NPHttpConn *httpconn, const char *body, size_t body_len)
{
    NIDPRINT("=============> ");

    return 100;
}

NPHttpConn *
np_httpconn_new(NPServConn *servconn)
{
	NPHttpConn *httpconn;
    
	g_return_val_if_fail(servconn != NULL, NULL);
    
	httpconn = g_new0(NPHttpConn, 1);
    
//	purple_debug_info("np", "new httpconn (%p)\n", httpconn);
    
	/* TODO: Remove this */
	httpconn->session = servconn->session;
    
	httpconn->servconn = servconn;
    
	httpconn->tx_buf = purple_circ_buffer_new(NP_BUF_LEN);
	httpconn->tx_handler = 0;
    
	httpconn->fd = -1;
    
	return httpconn;
}

void
np_httpconn_destroy(NPHttpConn *httpconn)
{
	g_return_if_fail(httpconn != NULL);
    
	purple_debug_info("np", "destroy httpconn (%p)\n", httpconn);
    
	if (httpconn->connected)
		np_httpconn_disconnect(httpconn);
    
	g_free(httpconn->full_session_id);
    
	g_free(httpconn->session_id);
    
	g_free(httpconn->host);
    
	while (httpconn->queue != NULL) {
		NPHttpQueueData *queue_data;
        
		queue_data = (NPHttpQueueData *) httpconn->queue->data;
        
		httpconn->queue = g_list_delete_link(httpconn->queue, httpconn->queue);
        
		g_free(queue_data->body);
		g_free(queue_data);
	}
    
	purple_circ_buffer_destroy(httpconn->tx_buf);
	if (httpconn->tx_handler > 0)
		purple_input_remove(httpconn->tx_handler);
    
	g_free(httpconn);
}

static void
connect_cb(gpointer data, gint source, const gchar *error_message)
{
	NPHttpConn *httpconn;
    
	httpconn = data;
	httpconn->connect_data = NULL;
	httpconn->fd = source;
    
	if (source >= 0)
	{
		httpconn->inpa = purple_input_add(httpconn->fd, PURPLE_INPUT_READ,
                                          read_cb, data);
        
		httpconn->timer = purple_timeout_add_seconds(2, np_httpconn_poll, httpconn);
        
		np_httpconn_process_queue(httpconn);
	}
	else
	{
		purple_debug_error("np", "HTTP: Connection error: %s\n",
		                   error_message ? error_message : "(null)");
		np_servconn_got_error(httpconn->servconn, NP_SERVCONN_ERROR_CONNECT, error_message);
	}
}

gboolean
np_httpconn_connect(NPHttpConn *httpconn, const char *host, int port)
{
	g_return_val_if_fail(httpconn != NULL, FALSE);
	g_return_val_if_fail(host     != NULL, FALSE);
	g_return_val_if_fail(port      > 0,    FALSE);
    
	if (httpconn->connected)
		np_httpconn_disconnect(httpconn);
    
	httpconn->connect_data = purple_proxy_connect(NULL, httpconn->session->account,
                                                  host, 80, connect_cb, httpconn);
    
	if (httpconn->connect_data != NULL)
	{
		httpconn->waiting_response = TRUE;
		httpconn->connected = TRUE;
	}
    
	return httpconn->connected;
}

void
np_httpconn_disconnect(NPHttpConn *httpconn)
{
	g_return_if_fail(httpconn != NULL);
    
	if (!httpconn->connected)
		return;
    
	if (httpconn->connect_data != NULL)
	{
		purple_proxy_connect_cancel(httpconn->connect_data);
		httpconn->connect_data = NULL;
	}
    
	if (httpconn->timer)
	{
		purple_timeout_remove(httpconn->timer);
		httpconn->timer = 0;
	}
    
	if (httpconn->inpa > 0)
	{
		purple_input_remove(httpconn->inpa);
		httpconn->inpa = 0;
	}
    
	close(httpconn->fd);
	httpconn->fd = -1;
    
	g_free(httpconn->rx_buf);
	httpconn->rx_buf = NULL;
	httpconn->rx_len = 0;
    
	httpconn->connected = FALSE;
    
	/* np_servconn_disconnect(httpconn->servconn); */
}

