//
//  nphttputil.c
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-24.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#include "nphttputil.h"


static PurpleUtilFetchUrlData *
np_util_post_url(const gchar *url, const gchar *postdata, PurpleUtilFetchUrlCallback callback, gpointer data)
{
	PurpleUtilFetchUrlData *urldata;
	gchar *host, *path, *request;
	int port;
	        
	purple_url_parse(url, &host, &port, &path, NULL, NULL);
	request = g_strdup_printf("POST /%s HTTP/1.1\r\n"
                              "Connection: close\r\n"
                              "Host: %s\r\n"
                              "Accept: */*\r\n"
                              "Content-Type: application/x-www-form-urlencoded\r\n"
                              "Content-Length: %" G_GSIZE_FORMAT "\r\n\r\n%s",
                              path, host, strlen(postdata), postdata);
	
	urldata = purple_util_fetch_url_request(url, TRUE, NULL, TRUE, request, FALSE, callback, data);
	
	g_free(host);
	g_free(path);
	g_free(request);
	
	return urldata;
}


PurpleUtilFetchUrlData *
http_login0(NPSession *session,PurpleUtilFetchUrlCallback callback)
{
    PurpleUtilFetchUrlData *url_data;
    PurpleAccount *account;
    
    account = session->account;
    
    const char *username = account->username;
    const char *pwd_md5 = g_compute_checksum_for_string(G_CHECKSUM_MD5,account->password,strlen(account->password));
    const char *http_server = purple_account_get_string(session->account, "http_method_server", NP_HTTPCONN_SERVER);
    const char *_ua = purple_account_get_string(session->account, "np_user_agent", NP_USER_AGENT);

    char *login_url = g_strdup_printf("%s/%s",http_server,NP_LOGIN_0_PATH);
    purple_debug_info("np", "login_url: %s",login_url);
    
    const char *login_from = "niupai";
    const char *login_device_id = purple_account_get_string(session->account, "device_id", "FAKE_DEVICE_ID_FROM_PURPLE");
    char *plain_login_key = g_strdup_printf("%s%s",login_device_id,"nPiGa:#)");
    char *login_key = g_compute_checksum_for_string(G_CHECKSUM_MD5,plain_login_key,strlen(plain_login_key));
    purple_debug_info("np", "login_device_id: %s",login_device_id);
    
    
    gchar *content = g_strdup_printf("account=%s&password=%s&login_from=%s&login_device_id=%s&login_key=%s&_ua=%s",
                                     username,
                                     pwd_md5,
                                     login_from,
                                     login_device_id,
                                     login_key,
                                     _ua,
                                     NULL
                                     );
    url_data = np_util_post_url(login_url, content, callback, session);
    
    g_free(plain_login_key);
    g_free(login_key);
	g_free(login_url);
    g_free(content);
    
    return url_data;
    
}

PurpleUtilFetchUrlData *
http_main_start(NPSession *session,PurpleUtilFetchUrlCallback callback)
{
    PurpleUtilFetchUrlData *url_data;
    PurpleAccount *account;
    
    account = session->account;
    
    const char *http_server = purple_account_get_string(session->account, "http_method_server", NP_HTTPCONN_SERVER);
    const char *_ua = purple_account_get_string(session->account, "np_user_agent", NP_USER_AGENT);
    
    char *start_url = g_strdup_printf("%s/%s",http_server,NP_MAIN_START_PATH);
    purple_debug_info("np", "main/start: %s",start_url);
        
    gchar *content = g_strdup_printf("_ua=%s",_ua,NULL);
    
    url_data = np_util_post_url(start_url, content, callback, session);
    
    g_free(content);
    
    return url_data;
    
}
