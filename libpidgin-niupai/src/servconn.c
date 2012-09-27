//
//  servconn.c
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-19.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#include "servconn.h"
#include "npconfig.h"
#include "memcpy_extra.h"

static void read_cb(gpointer data, gint source, PurpleInputCondition cond);
static void servconn_timeout_renew(NPServConn *servconn);

/**************************************************************************
 * Main
 **************************************************************************/

NPServConn *
np_servconn_new(NPSession *session, NPServConnType type)
{
	NPServConn *servconn;
    
	g_return_val_if_fail(session != NULL, NULL);
    
	servconn = g_new0(NPServConn, 1);
    
	servconn->type = type;
    
	servconn->session = session;
	servconn->cmdproc = np_cmdproc_new(session);
	servconn->cmdproc->servconn = servconn;
    
//	servconn->httpconn = np_httpconn_new(servconn);
    
	servconn->num = session->servconns_count++;
    
	servconn->tx_buf = purple_circ_buffer_new(NP_BUF_LEN);
	servconn->tx_handler = 0;
	servconn->timeout_sec = 0;
	servconn->timeout_handle = 0;
    
	servconn->fd = -1;
    
	return servconn;
}

void
np_servconn_destroy(NPServConn *servconn)
{
    purple_debug_warning("np","========= \n");

	g_return_if_fail(servconn != NULL);
    
	if (servconn->processing)
	{
		servconn->wasted = TRUE;
		return;
	}
    
	np_servconn_disconnect(servconn);
    
	if (servconn->destroy_cb)
		servconn->destroy_cb(servconn);
    
//	if (servconn->httpconn != NULL)
//		np_httpconn_destroy(servconn->httpconn);
    
	g_free(servconn->host);
    
	purple_circ_buffer_destroy(servconn->tx_buf);
	if (servconn->tx_handler > 0)
		purple_input_remove(servconn->tx_handler);
	if (servconn->timeout_handle > 0)
		purple_timeout_remove(servconn->timeout_handle);
    
	np_cmdproc_destroy(servconn->cmdproc);
	g_free(servconn);
}

void
np_servconn_set_connect_cb(NPServConn *servconn,
							void (*connect_cb)(NPServConn *))
{
	g_return_if_fail(servconn != NULL);
	servconn->connect_cb = connect_cb;
}

void
np_servconn_set_disconnect_cb(NPServConn *servconn,
							   void (*disconnect_cb)(NPServConn *))
{
	g_return_if_fail(servconn != NULL);
    
	servconn->disconnect_cb = disconnect_cb;
}

void
np_servconn_set_destroy_cb(NPServConn *servconn,
							void (*destroy_cb)(NPServConn *))
{
	g_return_if_fail(servconn != NULL);
    
	servconn->destroy_cb = destroy_cb;
}

/**************************************************************************
 * Utility
 **************************************************************************/

void
np_servconn_got_error(NPServConn *servconn, NPServConnError error,
                       const char *reason)
{
	NPSession *session = servconn->session;
	NPServConnType type = servconn->type;
    
	const char *names[] = { "Notification", "Switchboard" };
	const char *name;
    
	name = names[type];
    
	if (reason == NULL) {
		switch (error)
		{
			case NP_SERVCONN_ERROR_CONNECT:
				reason = _("Unable to connect"); break;
			case NP_SERVCONN_ERROR_WRITE:
				reason = _("Writing error"); break;
			case NP_SERVCONN_ERROR_READ:
				reason = _("Reading error"); break;
			default:
				reason = _("Unknown error"); break;
		}
	}

    purple_debug_warning("np","Connection error from %s server (%s): %s\n", name, servconn->host, reason);
//	purple_debug_error("np", "Connection error from %s server (%s): %s\n",
//                       name, servconn->host, reason);
    
	if (type == NP_SERVCONN_SB)
	{
        //TODO
//		NPSwitchBoard *swboard;
//		swboard = servconn->cmdproc->data;
//		if (swboard != NULL)
//			swboard->error = NP_SB_ERROR_CONNECTION;
	}
    
	/* servconn->disconnect_cb may destroy servconn, so don't use it again */
	np_servconn_disconnect(servconn);
    
	if (type == NP_SERVCONN_NS)
	{
		char *tmp = g_strdup_printf(_("Connection error from %s server:\n%s"),
		                            name, reason);
		np_session_set_error(session, NP_ERROR_SERVCONN, tmp);
		g_free(tmp);
	}
}

/**************************************************************************
 * Connect
 **************************************************************************/

static void
connect_cb(gpointer data, gint source, const char *error_message)
{
//    purple_debug_warning("np","========= \n data = %p",data);

	NPServConn *servconn;
    
	servconn = data;
	servconn->connect_data = NULL;
    
    purple_debug_warning("np","[data = %d] ",servconn->rx_len);
    purple_debug_warning("np","[data = %s] ",servconn->rx_buf);

	servconn->fd = source;
    
	if (source >= 0)
	{
		servconn->connected = TRUE;
        
		/* Someone wants to know we connected. */
		servconn->connect_cb(servconn);
//        purple_debug_warning("np","servconn->fd = %s ",servconn->cmdproc->data);
//        purple_debug_warning("np","data = %s ",data);
		servconn->inpa = purple_input_add(servconn->fd, PURPLE_INPUT_READ,
                                          read_cb, data);
		servconn_timeout_renew(servconn);
	}
	else
	{
		purple_debug_error("np", "Connection error: %s\n", error_message);
		np_servconn_got_error(servconn, NP_SERVCONN_ERROR_CONNECT, error_message);
	}
}

gboolean
np_servconn_connect(NPServConn *servconn, const char *host, int port, gboolean force)
{
	NPSession *session;
    
	g_return_val_if_fail(servconn != NULL, FALSE);
	g_return_val_if_fail(host     != NULL, FALSE);
	g_return_val_if_fail(port      > 0,    FALSE);
    
	session = servconn->session;
    
	if (servconn->connected)
		np_servconn_disconnect(servconn);
    
	g_free(servconn->host);
	servconn->host = g_strdup(host);
    
//    purple_debug_warning("np","===> session->http_method : %d",session->http_method);
//	if (session->http_method)
//	{
//		/* HTTP Connection. */
//        
//		if (!servconn->httpconn->connected || force)
//			if (!np_httpconn_connect(servconn->httpconn, host, port))
//				return FALSE;
//        
//		servconn->connected = TRUE;
//		servconn->httpconn->virgin = TRUE;
//		servconn_timeout_renew(servconn);
//        
//		/* Someone wants to know we connected. */
//		servconn->connect_cb(servconn);
//        
//		return TRUE;
//	}
    
	servconn->connect_data = purple_proxy_connect(NULL, session->account,
                                                  host, port, connect_cb, servconn);
    
	return (servconn->connect_data != NULL);
}

void
np_servconn_disconnect(NPServConn *servconn)
{
    purple_debug_warning("np","========= \n");

	g_return_if_fail(servconn != NULL);
    
	if (servconn->connect_data != NULL)
	{
		purple_proxy_connect_cancel(servconn->connect_data);
		servconn->connect_data = NULL;
	}
    
	if (!servconn->connected)
	{
		/* We could not connect. */
		if (servconn->disconnect_cb != NULL)
			servconn->disconnect_cb(servconn);
        
		return;
	}
    
//	if (servconn->session->http_method)
//	{
//		/* Fake disconnection. */
//		if (servconn->disconnect_cb != NULL)
//			servconn->disconnect_cb(servconn);
//        
//		return;
//	}
    
	if (servconn->inpa > 0)
	{
		purple_input_remove(servconn->inpa);
		servconn->inpa = 0;
	}
    
	if (servconn->timeout_handle > 0)
	{
		purple_timeout_remove(servconn->timeout_handle);
		servconn->timeout_handle = 0;
	}
    
	close(servconn->fd);
    
	servconn->rx_buf = NULL;
	servconn->rx_len = 0;
	servconn->payload_len = 0;
    
	servconn->connected = FALSE;
    
	if (servconn->disconnect_cb != NULL)
		servconn->disconnect_cb(servconn);
}

static gboolean
servconn_idle_timeout_cb(NPServConn *servconn)
{
	servconn->timeout_handle = 0;
	np_servconn_disconnect(servconn);
	return FALSE;
}


static void
servconn_timeout_renew(NPServConn *servconn)
{
	if (servconn->timeout_handle) {
		purple_timeout_remove(servconn->timeout_handle);
		servconn->timeout_handle = 0;
	}
    
	if (servconn->connected && servconn->timeout_sec) {
		servconn->timeout_handle = purple_timeout_add_seconds(
                                                              servconn->timeout_sec, (GSourceFunc)servconn_idle_timeout_cb, servconn);
	}
}

void
np_servconn_set_idle_timeout(NPServConn *servconn, guint seconds)
{
	servconn->timeout_sec = seconds;
	if (servconn->connected)
		servconn_timeout_renew(servconn);
}

static void
servconn_write_cb(gpointer data, gint source, PurpleInputCondition cond)
{
	NPServConn *servconn = data;
	gssize ret;
	gsize writelen;
    
	writelen = purple_circ_buffer_get_max_read(servconn->tx_buf);
    
	if (writelen == 0) {
		purple_input_remove(servconn->tx_handler);
		servconn->tx_handler = 0;
		return;
	}
    
	ret = write(servconn->fd, servconn->tx_buf->outptr, writelen);
    
	if (ret < 0 && errno == EAGAIN)
		return;
	else if (ret <= 0) {
		np_servconn_got_error(servconn, NP_SERVCONN_ERROR_WRITE, NULL);
		return;
	}
    
	purple_circ_buffer_mark_read(servconn->tx_buf, ret);
	servconn_timeout_renew(servconn);
}

gssize
np_servconn_write(NPServConn *servconn, const char *buf, size_t len)
{
	gssize ret = 0;
	g_return_val_if_fail(servconn != NULL, 0);
    
//	if (!servconn->session->http_method)
    if (1)
	{
		if (servconn->tx_handler == 0) {
			switch (servconn->type)
			{
				case NP_SERVCONN_NS:
				case NP_SERVCONN_SB:
					ret = write(servconn->fd, buf, len);
					break;
#if 0
				case NP_SERVCONN_DC:
					ret = write(servconn->fd, &buf, sizeof(len));
					ret = write(servconn->fd, buf, len);
					break;
#endif
				default:
					ret = write(servconn->fd, buf, len);
					break;
			}
		} else {
			ret = -1;
			errno = EAGAIN;
		}
        
		if (ret < 0 && errno == EAGAIN)
			ret = 0;
		if (ret >= 0 && ret < len) {
			if (servconn->tx_handler == 0)
				servconn->tx_handler = purple_input_add(
                                                        servconn->fd, PURPLE_INPUT_WRITE,
                                                        servconn_write_cb, servconn);
			purple_circ_buffer_append(servconn->tx_buf, buf + ret,
                                      len - ret);
		}
	}
//	else
//	{
//		ret = np_httpconn_write(servconn->httpconn, buf, len);
//	}
    
	if (ret == -1)
	{
		np_servconn_got_error(servconn, NP_SERVCONN_ERROR_WRITE, NULL);
	}
    
	servconn_timeout_renew(servconn);
	return ret;
}

static void
read_cb(gpointer data, gint source, PurpleInputCondition cond)
{
	NPServConn *servconn;
	char buf[NP_BUF_LEN];
	gssize len;
	servconn = data;
    
	if (servconn->type == NP_SERVCONN_NS)
		servconn->session->account->gc->last_received = time(NULL);

	len = read(servconn->fd, buf, sizeof(buf) - 1);
    if (len < 0 && errno == EAGAIN)
		return;
	if (len <= 0) {
		purple_debug_error("np", "servconn %03d read error, "
                           "len: %" G_GSSIZE_FORMAT ", errno: %d, error: %s\n",
                           servconn->num, len, errno, g_strerror(errno));
		np_servconn_got_error(servconn, NP_SERVCONN_ERROR_READ, NULL);
        
		return;
	}
    
	buf[len] = '\0';
    
	servconn->rx_buf = g_realloc(servconn->rx_buf, len + servconn->rx_len + 1);
	memcpy(servconn->rx_buf + servconn->rx_len, buf, len + 1);
	servconn->rx_len += len;

    // Log Buffer
    for(int i = 0;i < len; i++) {
        fprintf(stdout, "[%c]",buf[i]);
    }
    fprintf(stdout, "\n");

    for(int i = 0;i < len; i++) {
        fprintf(stdout, "[%x]",buf[i]);
    }
    fprintf(stdout, "\n");
    
	servconn = np_servconn_process_data(servconn);
	if (servconn)
		servconn_timeout_renew(servconn);
}

NPServConn *np_servconn_process_data(NPServConn *servconn)
{
	char *cur, *end, *old_rx_buf;
    u_int16_t expected_len; // 2+real_content
    
	end = old_rx_buf = servconn->rx_buf;
    
	servconn->processing = TRUE;



    if (servconn->rx_len < 2) {
        //
        // Do nothing
        // the first 2-bytes is the length we expecting
        //
    } else {
        u_int16_t len0 = old_rx_buf[0] & 0x00FF;
        u_int16_t len1 = old_rx_buf[1] & 0x00FF;
        expected_len = (len0 << 8) | len1;

        if (servconn->rx_len < expected_len + 2) {
            //
            // Let socket read more bytes
            //
        } else {
            purple_debug_warning("np","this pack is readed done;\n");
            gchar *real_content = (gchar *)g_malloc(expected_len);
            memcpy(real_content, old_rx_buf+2, expected_len);
            real_content[expected_len] = '\0';
            np_cmdproc_process_cmd_text(servconn->cmdproc, real_content);
            purple_debug_warning("np","socket message : %s\n",real_content);
            free(real_content);

            // move pointer to 
            servconn->rx_len = servconn->rx_len - expected_len - 2;
            cur = old_rx_buf + expected_len + 2;
            for (int i = 0; i < MIN(servconn->rx_len, 10); i++) {
                purple_debug_warning("nps","cur = %c\n",cur[i]);
            }

        }
        
    }

    
	if (servconn->connected && !servconn->wasted)
	{
		if (servconn->rx_len > 0)
			servconn->rx_buf = g_memdup(cur, servconn->rx_len);
		else
			servconn->rx_buf = NULL;
	}
    
	servconn->processing = FALSE;
    
	if (servconn->wasted) {
		np_servconn_destroy(servconn);
		servconn = NULL;
	}
    
	g_free(old_rx_buf);
	return servconn;
    
}

#if 0
static int
create_listener(int port)
{
	int fd;
	int flags;
	const int on = 1;
    
#if 0
	struct addrinfo hints;
	struct addrinfo *c, *res;
	char port_str[5];
    
	snprintf(port_str, sizeof(port_str), "%d", port);
    
	memset(&hints, 0, sizeof(hints));
    
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
    
	if (getaddrinfo(NULL, port_str, &hints, &res) != 0)
	{
		purple_debug_error("np", "Could not get address info: %s.\n",
                           port_str);
		return -1;
	}
    
	for (c = res; c != NULL; c = c->ai_next)
	{
		fd = socket(c->ai_family, c->ai_socktype, c->ai_protocol);
        
		if (fd < 0)
			continue;
        
		setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        
		if (bind(fd, c->ai_addr, c->ai_addrlen) == 0)
			break;
        
		close(fd);
	}
    
	if (c == NULL)
	{
		purple_debug_error("np", "Could not find socket: %s.\n", port_str);
		return -1;
	}
    
	freeaddrinfo(res);
#else
	struct sockaddr_in sockin;
    
	fd = socket(AF_INET, SOCK_STREAM, 0);
    
	if (fd < 0)
		return -1;
    
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) != 0)
	{
		close(fd);
		return -1;
	}
    
	memset(&sockin, 0, sizeof(struct sockaddr_in));
	sockin.sin_family = AF_INET;
	sockin.sin_port = htons(port);
    
	if (bind(fd, (struct sockaddr *)&sockin, sizeof(struct sockaddr_in)) != 0)
	{
		close(fd);
		return -1;
	}
#endif
    
	if (listen (fd, 4) != 0)
	{
		close (fd);
		return -1;
	}
    
	flags = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#ifndef _WIN32
	fcntl(fd, F_SETFD, FD_CLOEXEC);
#endif
    
	return fd;
}
#endif



