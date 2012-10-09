
//
//  nphttputil.h
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-24.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#ifndef __NP_NPHTTPUTIL_H__
#define __NP_NPHTTPUTIL_H__

#include "internal.h"
#include "session.h"
#include "npconfig.h"




PurpleUtilFetchUrlData *np_http_login0(NPNotification *notification,PurpleUtilFetchUrlCallback callback);

PurpleUtilFetchUrlData *http_get_friend_list(NPNotification *notification,PurpleUtilFetchUrlCallback callback);


PurpleUtilFetchUrlData *np_http_send_im(NPSession *session, const gchar *who, const gchar *message,PurpleMessageFlags flags);


#endif /* __NP_NPHTTPUTIL_H__ */
