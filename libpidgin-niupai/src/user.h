//
//  user.h
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-19.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#ifndef __NP_USER_H__
#define __NP_USER_H__

typedef struct _NPUser NPUser;


#include "niupai_session.h"
#include "userlist.h"

struct _NPUser {
    NPUserList *userList;
    
    
	guint8 refcount;        /**< The reference count of this object */
    
	char *passport;         /**< The passport account.          */
	char *friendly_name;    /**< The friendly name.             */
    
	char *uid;              /*< User ID                         */
	GSList *endpoints;      /*< Endpoint-specific data          */
    
	const char *status;     /**< The state of the user.         */
	char *statusline;       /**< The state of the user.         */
    
	gboolean idle;          /**< The idle state of the user.    */
};


#endif /* __NP_USER_H__ */
