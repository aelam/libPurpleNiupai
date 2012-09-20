//
//  notification.c
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-19.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#include "notification.h"
#include "NIDebuggingTools.h"

void np_notification_end(void)
{
    
}

void np_notification_init(void)
{
    
}

static void
destroy_cb(NPServConn *servconn)
{
	NPServConn *notification = NULL;
    
//	notification = servconn->cmdproc->data;
	g_return_if_fail(notification != NULL);
    
//	np_notification_destroy(notification);
}


NPNotification *np_notification_new(NPSession *session)
{
    NPNotification *notification;

    //TODO
	NPServConn *servconn;
    
	g_return_val_if_fail(session != NULL, NULL);
    
	notification = g_new0(NPNotification, 1);
    notification->session = session;
    notification->servconn = np_servconn_new(session, NP_SERVCONN_NS);
    np_servconn_set_destroy_cb(servconn, destroy_cb);

	return notification;
}

void np_notification_destroy(NPNotification *notification)
{
    // TODO
    // Service Connect destroy
    g_free(notification);
}

/**************************************************************************
 * Connect
 **************************************************************************/

static void
connect_cb(NPServConn *servconn)
{
    //TODO
    NIDPRINT("=========== \n");

}

gboolean np_notification_connect(NPNotification *notification,
                                 const char *host, int port)
{
    NIDPRINT("host : %s port : %d \n",host,port);
    
    NPServConn *servconn;
    
    g_return_val_if_fail(notification != NULL, FALSE);
    
    servconn = notification->servconn;
    
    np_servconn_set_connect_cb(servconn, connect_cb);
    
    notification->in_use = np_servconn_connect(servconn, host, port, TRUE);

    return notification->in_use;
}

void np_notification_disconnect(NPNotification *notification)
{
    g_return_if_fail(notification != NULL);
	g_return_if_fail(notification->in_use);
    
	np_servconn_disconnect(notification->servconn);
    
	notification->in_use = FALSE;

}
