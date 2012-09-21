//
//  httpconn.h
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-19.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#ifndef __NP_HTTPCONN_H__
#define __NP_HTTPCONN_H__

typedef struct _NPHttpConn NPHttpConn;

#include "circbuffer.h"
#include "servconn.h"
#include "session.h"

/**
 * An HTTP Connection.
 */
struct _NPHttpConn
{
	NPSession *session; /**< The NP Session. */
	NPServConn *servconn; /**< The connection object. */
    
	PurpleProxyConnectData *connect_data;
    
	char *full_session_id; /**< The full session id. */
	char *session_id; /**< The trimmed session id. */
    
	int timer; /**< The timer for polling. */
    
	gboolean waiting_response; /**< The flag that states if we are waiting
                                a response from the server. */
	gboolean connected;        /**< The flag that states if the connection is on. */
	gboolean virgin;           /**< The flag that states if this connection
                                should specify the host (not gateway) to
                                connect to. */
    
	char *host; /**< The HTTP gateway host. */
	GList *queue; /**< The queue of data chunks to write. */
    
	int fd; /**< The connection's file descriptor. */
	guint inpa; /**< The connection's input handler. */
    
	char *rx_buf; /**< The receive buffer. */
	int rx_len; /**< The receive buffer length. */
    
	PurpleCircBuffer *tx_buf;
	guint tx_handler;
};

/**
 * Creates a new HTTP connection object.
 *
 * @param servconn The connection object.
 *
 * @return The new object.
 */
NPHttpConn *np_httpconn_new(NPServConn *servconn);

/**
 * Destroys an HTTP connection object.
 *
 * @param httpconn The HTTP connection object.
 */
void np_httpconn_destroy(NPHttpConn *httpconn);

/**
 * Writes a chunk of data to the HTTP connection.
 *
 * @param servconn    The server connection.
 * @param data        The data to write.
 * @param data_len    The size of the data to write.
 *
 * @return The number of bytes written.
 */
gssize np_httpconn_write(NPHttpConn *httpconn, const char *data, size_t data_len);

/**
 * Connects the HTTP connection object to a host.
 *
 * @param httpconn The HTTP connection object.
 * @param host The host to connect to.
 * @param port The port to connect to.
 */
gboolean np_httpconn_connect(NPHttpConn *httpconn,
							  const char *host, int port);

/**
 * Disconnects the HTTP connection object.
 *
 * @param httpconn The HTTP connection object.
 */
void np_httpconn_disconnect(NPHttpConn *httpconn);



#endif /* __NP_HTTPCONN_H__ */
