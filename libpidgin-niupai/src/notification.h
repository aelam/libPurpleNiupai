//
//  notification.h
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-19.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#ifndef __NP_NOTIFICATION_H__
#define __NP_NOTIFICATION_H__

typedef struct _NPNotification NPNotification;

#include "session.h"
#include "servconn.h"
#include "cmdproc.h"

#define NP_LOGIN_STRING         "LOGIN,45557212,47a17558eaf9d9b82073aa1195fabf9c,1,5e93998eddcbd95fbd5925f8c77b235f"

struct _NPNotification
{
	NPSession *session;
    
	/**
	 * This is a convenience pointer that always points to
	 * servconn->cmdproc
	 */
	NPCmdProc *cmdproc;
	NPServConn *servconn;
    
	gboolean in_use;
};

void np_notification_end(void);
void np_notification_init(void);

NPNotification *np_notification_new(NPSession *session);
void np_notification_destroy(NPNotification *notification);
gboolean np_notification_connect(NPNotification *notification,
                                  const char *host, int port);
void np_notification_disconnect(NPNotification *notification);

/**
 * Closes a notification.
 *
 * It's first closed, and then disconnected.
 *
 * @param notification The notification object to close.
 */
void np_notification_close(NPNotification *notification);


#endif /* __NP_NOTIFICATION_H__ */
