//
//  session.h
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-19.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#ifndef __NP_SESSION_H__
#define __NP_SESSION_H__

typedef struct _NPSession NPSession;

/**
 * Types of errors.
 */
typedef enum
{
	NP_ERROR_SERVCONN,
	NP_ERROR_UNSUPPORTED_PROTOCOL,
	NP_ERROR_HTTP_MALFORMED,
	NP_ERROR_AUTH,
	NP_ERROR_BAD_BLIST,
	NP_ERROR_SIGN_OTHER,
	NP_ERROR_SERV_DOWN,
	NP_ERROR_SERV_UNAVAILABLE
} NPErrorType;


/**
 * Login steps.
 */
typedef enum
{
	NP_LOGIN_STEP_START,
	NP_LOGIN_STEP_HANDSHAKE,
	NP_LOGIN_STEP_TRANSFER,
	NP_LOGIN_STEP_HANDSHAKE2,
	NP_LOGIN_STEP_AUTH_START,
	NP_LOGIN_STEP_GET_COOKIE,
	NP_LOGIN_STEP_AUTH_END,
	NP_LOGIN_STEP_SYN,
	NP_LOGIN_STEP_END
} NPLoginStep;

#define NP_LOGIN_STEPS NP_LOGIN_STEP_END

//#include "niupai.h"
#include "account.h"
#include "user.h"
#include "userlist.h"
#include "notification.h"


struct _NPSession {
    PurpleAccount   *account;
    NPUser          *user;
    guint           protocol_ver;

    NPLoginStep     login_step;
    
	gboolean        connected:1;
	gboolean        logged_in:1; /**< A temporal flag to ignore local buddy list adds. */
	gboolean        destroying:1; /**< A flag that states if the session is being destroyed. */
	gboolean        http_method:1;

    guint           login_timeout; /**< Timeout to force status change if ADL/FQY fail. */

    int             servconns_count;
    NPNotification  *notification;
    NPUserList      *userlist;
};

/**
 * Creates a NP session.
 *
 * @param account The account.
 *
 * @return The new NP session.
 */
NPSession *np_session_new(PurpleAccount *account);

/**
 * Destroys a NP session.
 *
 * @param session The NP session to destroy.
 */
void np_session_destroy(NPSession *session);

/**
 * Connects to and initiates an NP session.
 *
 * @param session     The NP session.
 * @param host        The dispatch server host.
 * @param port        The dispatch server port.
 * @param http_method Whether to use or not http_method.
 *
 * @return @c TRUE on success, @c FALSE on failure.
 */
gboolean np_session_connect(NPSession *session,
							 const char *host, int port,
							 gboolean http_method);

/**
 * Disconnects from an NP session.
 *
 * @param session The NP session.
 */
void np_session_disconnect(NPSession *session);


/**
 * Sets the current step in the login process.
 *
 * @param session The NP session.
 * @param step The current step.
 */
void np_session_set_login_step(NPSession *session, NPLoginStep step);


/**
 * Finish the login proccess.
 *
 * @param session The NP session.
 */
void np_session_finish_login(NPSession *session);


#endif /* __NP_NIUPAI_SESSION_H__ */