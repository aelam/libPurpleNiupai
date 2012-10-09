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


#include "session.h"
#include "userlist.h"

struct _NPUser {
    NPUserList *userlist;
    
    
	guint8 refcount;        /**< The reference count of this object */
    
	char *friendly_name;    /**< The friendly name.             */
    
	
	const char *status;     /**< The state of the user.         */
	char *statusline;       /**< The state of the user.         */
    
	gboolean idle;          /**< The idle state of the user.    */
    
	char *uid;              /*< User ID                         */
    guint age;
	gchar *gender;
	gchar *location;
	gchar *face_url;

    gchar *nickname;
    gchar *remark_name;
    
    
};


/**
 * Creates a new user structure.
 *
 * @param session      The NP session.
 * @param passport     The initial passport.
 * @param stored_name  The initial stored name.
 *
 * @return A new user structure.  It will have a reference count of 1.
 */
NPUser *np_user_new(NPUserList *userlist, const char *passport,
                    const char *friendly_name);


/**
 * destory a new user structure
 * @param user NPUser
 */
static void np_user_destroy(NPUser *user);

/**
 * Increment the reference count.
 *
 * @param user 	The user.
 *
 * @return 		user.
 */
NPUser *np_user_ref(NPUser *user);

/**
 * Decrement the reference count.  When the count reaches 0 the object is
 * automatically freed.
 *
 * @param user 	The user
 */
void np_user_unref(NPUser *user);

/**
 * Updates the user.
 *
 * Communicates with the core to update the ui, etc.
 *
 * @param user The user to update.
 */
void np_user_update(NPUser *user);

/**
 *  Sets the new statusline of user.
 *
 *  @param user The user.
 *  @param state The statusline string.
 */
void np_user_set_statusline(NPUser *user, const char *statusline);

/**
 * Sets the new state of user.
 *
 * @param user The user.
 * @param state The state string.
 */
void np_user_set_state(NPUser *user, const char *state);

/**
 * Sets the passport account for a user.
 *
 * @param user     The user.
 * @param passport The passport account.
 */
void np_user_set_passport(NPUser *user, const char *passport);

/**
 * Sets the friendly name for a user.
 *
 * @param user The user.
 * @param name The friendly name.
 *
 * @returns TRUE is name actually changed, FALSE otherwise.
 */
gboolean np_user_set_friendly_name(NPUser *user, const char *name);

/**
 * Sets the buddy icon for a local user.
 *
 * @param user     The user.
 * @param img      The buddy icon image
 */
void np_user_set_buddy_icon(NPUser *user, PurpleStoredImage *img);

void np_user_set_uid(NPUser *user, const char *uid);

/**
 * Returns the passport account for a user.
 *
 * @param user The user.
 *
 * @return The passport account.
 */
const char *np_user_get_passport(const NPUser *user);

/**
 * Returns the friendly name for a user.
 *
 * @param user The user.
 *
 * @return The friendly name.
 */
const char *np_user_get_friendly_name(const NPUser *user);

/**
 * Check if the user is part of the group.
 *
 * @param user 		The user we are asking group membership.
 * @param group_id 	The group where the user may be in.
 *
 * @return TRUE if user is part of the group. Otherwise, FALSE.
 */
gboolean np_user_is_in_group(NPUser *user, const char * group_id);

/**
 * Check if user is on list.
 *
 * @param user 		The user we are asking list membership.
 * @param list_id 	The list where the user may be in.
 *
 * @return TRUE if the user is on the list, else FALSE.
 */
//gboolean np_user_is_in_list(NPUser *user, NPListId list_id);

/**
 * check to see if user is online
 */
gboolean np_user_is_online(PurpleAccount *account, const char *name);


#endif /* __NP_USER_H__ */
