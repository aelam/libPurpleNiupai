/**
 * @file switchboard.h NP switchboard functions
 *
 * purple
 *
 * Purple is the legal property of its developers, whose names are too numerous
 * to list here.  Please refer to the COPYRIGHT file distributed with this
 * source distribution.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
 */
#ifndef NP_SWITCHBOARD_H
#define NP_SWITCHBOARD_H

typedef struct _NPSwitchBoard NPSwitchBoard;

/**
 * A switchboard error.
 */
typedef enum
{
	NP_SB_ERROR_NONE, /**< No error. */
	NP_SB_ERROR_CAL, /**< The user could not join (answer the call). */
	NP_SB_ERROR_OFFLINE, /**< The account is offline. */
	NP_SB_ERROR_USER_OFFLINE, /**< The user to call is offline. */
	NP_SB_ERROR_CONNECTION, /**< There was a connection error. */
	NP_SB_ERROR_TOO_FAST, /**< We are sending too fast */
	NP_SB_ERROR_AUTHFAILED, /**< Authentication failed joining the switchboard session */
	NP_SB_ERROR_UNKNOWN /**< An unknown error occurred. */
} NPSBErrorType;

/**
 * A switchboard flag.
 */
typedef enum
{
	NP_SB_FLAG_IM = 0x01, /**< This switchboard is being used for a conversation. */
	NP_SB_FLAG_FT = 0x02  /**< This switchboard is being used for file transfer. */
} NPSBFlag;

#include "cmdproc.h"
#include "msg.h"
#include "servconn.h"
#include "session.h"

/**
 * A switchboard.
 *
 * A place where a bunch of users send messages to the rest of the users.
 */
struct _NPSwitchBoard
{
	NPSession *session;   /**< Our parent session. */
	NPServConn *servconn; /**< The physical connection for this switchboard. */
	NPCmdProc *cmdproc;   /**< Convenience variable for servconn->cmdproc. */
	char *im_user;

	NPSBFlag flag;
	char *auth_key;
	char *session_id;

	PurpleConversation *conv; /**< The conversation that displays the
							  messages of this switchboard, or @c NULL if
							  this is a helper switchboard. */

	gboolean empty;			/**< A flag that states if the swithcboard has no
							  users in it. */
	gboolean invited;		/**< A flag that states if we were invited to the
							  switchboard. */
	gboolean ready;			/**< A flag that states if this switchboard is
							  ready to be used. */
	gboolean closed;		/**< A flag that states if the switchboard has
							  been closed by the user. */
	gboolean destroying;	/**< A flag that states if the switchboard is
							  alredy on the process of destruction. */

	int current_users;
	int total_users;
	GList *users;

	int chat_id;

	GQueue *msg_queue; /**< Queue of messages to send. */
	GList *ack_list; /**< List of messages waiting for an ack. */

	NPSBErrorType error; /**< The error that occurred in this switchboard
							(if applicable). */
	GList *slplinks; /**< The list of slplinks that are using this switchboard. */
	guint reconn_timeout_h;
};

/**
 * Initialize the variables for switchboard creation.
 */
void np_switchboard_init(void);

/**
 * Destroy the variables for switchboard creation.
 */
void np_switchboard_end(void);

/**
 * Creates a new switchboard.
 *
 * @param session The NP session.
 *
 * @return The new switchboard.
 */
NPSwitchBoard *np_switchboard_new(NPSession *session);

/**
 * Destroys a switchboard.
 *
 * @param swboard The switchboard to destroy.
 */
void np_switchboard_destroy(NPSwitchBoard *swboard);

/**
 * Sets the auth key the switchboard must use when connecting.
 *
 * @param swboard The switchboard.
 * @param key     The auth key.
 */
void np_switchboard_set_auth_key(NPSwitchBoard *swboard, const char *key);

/**
 * Returns the auth key the switchboard must use when connecting.
 *
 * @param swboard The switchboard.
 *
 * @return The auth key.
 */
const char *np_switchboard_get_auth_key(NPSwitchBoard *swboard);

/**
 * Sets the session ID the switchboard must use when connecting.
 *
 * @param swboard The switchboard.
 * @param id      The session ID.
 */
void np_switchboard_set_session_id(NPSwitchBoard *swboard, const char *id);

/**
 * Returns the session ID the switchboard must use when connecting.
 *
 * @param swboard The switchboard.
 *
 * @return The session ID.
 */
const char *np_switchboard_get_session_id(NPSwitchBoard *swboard);

/**
 * Returns the next chat ID for use by a switchboard.
 *
 * @return The chat ID.
 */
int np_switchboard_get_chat_id(void);

/**
 * Sets whether or not we were invited to this switchboard.
 *
 * @param swboard The switchboard.
 * @param invite  @c TRUE if invited, @c FALSE otherwise.
 */
void np_switchboard_set_invited(NPSwitchBoard *swboard, gboolean invited);

/**
 * Returns whether or not we were invited to this switchboard.
 *
 * @param swboard The switchboard.
 *
 * @return @c TRUE if invited, @c FALSE otherwise.
 */
gboolean np_switchboard_is_invited(NPSwitchBoard *swboard);

/**
 * Connects to a switchboard.
 *
 * @param swboard The switchboard.
 * @param host    The switchboard server host.
 * @param port    The switcbharod server port.
 *
 * @return @c TRUE if able to connect, or @c FALSE otherwise.
 */
gboolean np_switchboard_connect(NPSwitchBoard *swboard,
								 const char *host, int port);

/**
 * Disconnects from a switchboard.
 *
 * @param swboard The switchboard to disconnect from.
 */
void np_switchboard_disconnect(NPSwitchBoard *swboard);

/**
 * Closes the switchboard.
 *
 * Called when a conversation is closed.
 *
 * @param swboard The switchboard to close.
 */
void np_switchboard_close(NPSwitchBoard *swboard);

/**
 * Release a switchboard from a certain function.
 *
 * @param swboard The switchboard to release.
 * @param flag The flag that states the function.
 */
void np_switchboard_release(NPSwitchBoard *swboard, NPSBFlag flag);

/**
 * Returns whether or not we currently can send a message through this
 * switchboard.
 *
 * @param swboard The switchboard.
 *
 * @return @c TRUE if a message can be sent, @c FALSE otherwise.
 */
gboolean np_switchboard_can_send(NPSwitchBoard *swboard);

/**
 * Sends a message through this switchboard.
 *
 * @param swboard The switchboard.
 * @param msg The message.
 * @param queue A flag that states if we want this message to be queued (in
 * the case it cannot currently be sent).
 *
 * @return @c TRUE if a message can be sent, @c FALSE otherwise.
 */
void np_switchboard_send_msg(NPSwitchBoard *swboard, NPMessage *msg,
							  gboolean queue);

void
msg_error_helper(NPCmdProc *cmdproc, NPMessage *msg, NPMsgErrorType error);

gboolean np_switchboard_chat_leave(NPSwitchBoard *swboard);
gboolean np_switchboard_chat_invite(NPSwitchBoard *swboard, const char *who);

gboolean np_switchboard_request(NPSwitchBoard *swboard);
void np_switchboard_request_add_user(NPSwitchBoard *swboard, const char *user);

/**
 * Shows an ink message from this switchboard.
 *
 * @param swboard  The switchboard.
 * @param passport The user that sent the ink.
 * @param data     The ink data.
 */
void np_switchboard_show_ink(NPSwitchBoard *swboard, const char *passport,
                              const char *data);

#endif /* NP_SWITCHBOARD_H */
