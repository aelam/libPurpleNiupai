//
//  nphttputil.c
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-24.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#include "nphttputil.h"
#include "internal.h"
#include "session.h"
#include "npconfig.h"

static char *
np_use_agent()
{
    gchar *ua = "i|5.0|niupai|4|niupai_ios_apple||320";
    
    return ua;
}

void http_login0(NPSession *session,PurpleUtilFetchUrlCallback callback)
{
    PurpleAccount *account;
    account = session->account;
    
	const char *username = account->username;
	const char *password = account->password;
    const char *http_server = purple_account_get_string(session->account, "http_method_server", NP_HTTPCONN_SERVER);
    const char *ua = np_use_agent();
    
    const char *login_device_id = NULL;
    const char *login_from = "niupai";
    const char *login_key = "niupai";

//	gchar *encsender = g_strdup(purple_url_encode(sender));
//	gchar *encmsg = g_strdup(purple_url_encode(message));
//	gchar *senddata = g_strdup_printf("label=Pidgin&title=%s&msg=%s", encsender, encmsg);
//	gchar *authstring = g_strdup_printf("%s:%s", username, apikey);
//	gchar *encauthstring = g_base64_encode(authstring, strlen(authstring));
    gchar *content = g_strdup("account=%s&password=%s");
    purple_debug_info("np", "http_server: %s",http_server);

	gchar *request = g_strdup_printf (
                                      "POST %s?_ua=%s HTTP/1.1\r\n"
                                      "Host: %s\r\n"
                                      "Content-Type: application/x-www-form-urlencoded\r\n"
                                      "Content-Length: %d\r\n\r\n%s",
                                      NP_LOGIN_0_PATH,
                                      ua,
                                      http_server
                                      );
    
//	g_free(encsender);
//	g_free(encmsg);
//	g_free(senddata);
//	g_free(authstring);
    
	purple_util_fetch_url_request(http_server, TRUE, NULL, TRUE, request, FALSE, callback, session);
	g_free(request);
}


