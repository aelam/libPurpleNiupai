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

NPNotification *np_notification_new(NPSession *session)
{
    return NULL;
}

void np_notification_destroy(NPNotification *notification)
{
    
}

gboolean np_notification_connect(NPNotification *notification,
                                 const char *host, int port)
{
    NIDPRINT("host : %s port : %d \n",host,port);
    return TRUE;
}

void np_notification_disconnect(NPNotification *notification)
{
    
}
