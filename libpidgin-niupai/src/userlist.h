//
//  use_list.h
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-19.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#ifndef __NP_USERLIST_H__
#define __NP_USERLIST_H__

typedef struct _NPUserList NPUserList;

#include "user.h"
#include "group.h"

struct _NPUserList {
    NPSession *session;
    GList *users;
    GList *groups;
    
    GQueue *buddy_icon_requests;
	int buddy_icon_window;
	guint buddy_icon_request_timer;

};



NPUserList *np_userlist_new(NPSession *session);
void np_userlist_destroy(NPUserList *userlist);

void np_userlist_add_user(NPUserList *userlist, NPUser *user);
void np_userlist_remove_user(NPUserList *userlist, NPUser *user);



#endif  /* __NP_USERLIST_H__ */
