
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

//void np_update_cookies_and_account(NPSession *session,gchar *buffer,gsize len,GError **error);

#endif /* __NP_NPHTTPUTIL_H__ */
