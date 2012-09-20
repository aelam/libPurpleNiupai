//
//  servconn.h
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-19.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#ifndef __SERV_CONN_H__
#define __SERV_CONN_H__

typedef struct _NPServConn NPServConn;

/**
 * Connection error types.
 */
typedef enum
{
	NP_SERVCONN_ERROR_NONE,
	NP_SERVCONN_ERROR_CONNECT,
	NP_SERVCONN_ERROR_WRITE,
	NP_SERVCONN_ERROR_READ
} NPServConnError;


/**
 * Connection types.
 */
typedef enum
{
	NP_SERVCONN_NS,
	NP_SERVCONN_SB
} NPServConnType;



#include "internal.h"
#include "proxy.h"

#include "session.h"
#include "httpconn.h"
#include "cmdproc.h"

struct _NPServConn {
    NPSession *session; /**< The NP session of this connection. */
    NPServConnType type;
    NPCmdProc *cmdproc;  /**< The command processor of this connection. */

    PurpleProxyConnectData *connect_data;

	gboolean connected;   /**< A flag that states if it's connected. */
	gboolean processing;  /**< A flag that states if something is working
                           with this connection. */
	gboolean wasted;      /**< A flag that states if it should be destroyed. */
    
	char *host; /**< The host this connection is connected or should be
                 connected to. */
	int num; /**< A number id of this connection. */

    NPHttpConn *httpconn; /**< The HTTP connection this connection should use. */

    int fd; /**< The connection's file descriptor. */
	int inpa; /**< The connection's input handler. */
    
	char *rx_buf; /**< The receive buffer. */
	int rx_len; /**< The receive buffer lenght. */

	size_t payload_len; /**< The length of the payload.
                         It's only set when we've received a command that
                         has a payload. */
    
	PurpleCircBuffer *tx_buf;
	guint tx_handler;
	guint timeout_sec;
	guint timeout_handle;
    
	void (*connect_cb)(NPServConn *); /**< The callback to call when connecting. */
	void (*disconnect_cb)(NPServConn *); /**< The callback to call when disconnecting. */
	void (*destroy_cb)(NPServConn *); /**< The callback to call when destroying. */

};

/**
 * Creates a new connection object.
 *
 * @param session The session.
 * @param type The type of the connection.
 */
NPServConn *np_servconn_new(NPSession *session, NPServConnType type);

/**
 * Destroys a connection object.
 *
 * @param servconn The connection.
 */
void np_servconn_destroy(NPServConn *servconn);

/**
 * Connects to a host.
 *
 * @param servconn The connection.
 * @param host The host.
 * @param port The port.
 * @param force Force this servconn to connect to a new server.
 */
gboolean np_servconn_connect(NPServConn *servconn, const char *host, int port,
                              gboolean force);

/**
 * Disconnects.
 *
 * @param servconn The connection.
 */
void np_servconn_disconnect(NPServConn *servconn);

/**
 * Sets the connect callback.
 *
 * @param servconn The servconn.
 * @param connect_cb The connect callback.
 */
void np_servconn_set_connect_cb(NPServConn *servconn,
								 void (*connect_cb)(NPServConn *));
/**
 * Sets the disconnect callback.
 *
 * @param servconn The servconn.
 * @param disconnect_cb The disconnect callback.
 */
void np_servconn_set_disconnect_cb(NPServConn *servconn,
									void (*disconnect_cb)(NPServConn *));
/**
 * Sets the destroy callback.
 *
 * @param servconn The servconn that's being destroyed.
 * @param destroy_cb The destroy callback.
 */
void np_servconn_set_destroy_cb(NPServConn *servconn,
								 void (*destroy_cb)(NPServConn *));

/**
 * Writes a chunck of data to the servconn.
 *
 * @param servconn The servconn.
 * @param buf The data to write.
 * @param size The size of the data.
 */
gssize np_servconn_write(NPServConn *servconn, const char *buf,
						  size_t size);

/**
 * Function to call whenever an error related to a switchboard occurs.
 *
 * @param servconn The servconn.
 * @param error The error that happened.
 */
void np_servconn_got_error(NPServConn *servconn, NPServConnError error,
                            const char *reason);

/**
 * Process the data in servconn->rx_buf.  This is called after reading
 * data from the socket.
 *
 * @param servconn The servconn.
 *
 * @return @c NULL if servconn was destroyed, 'servconn' otherwise.
 */
NPServConn *np_servconn_process_data(NPServConn *servconn);

/**
 * Set a idle timeout fot this servconn
 *
 * @param servconn The servconn
 * @param seconds The idle timeout in seconds
 */
void np_servconn_set_idle_timeout(NPServConn *servconn, guint seconds);


#endif /* __SERV_CONN_H__ */
