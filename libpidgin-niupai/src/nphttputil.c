//
//  nphttputil.c
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-24.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#include "nphttputil.h"

static PurpleUtilFetchUrlData *
np_util_post_url(const gchar *url, const gchar *postdata,gboolean include_headers, PurpleUtilFetchUrlCallback callback, gpointer data)
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
	
	urldata = purple_util_fetch_url_request(url, TRUE, NULL, TRUE, request, include_headers, callback, data);
	
	g_free(host);
	g_free(path);
	g_free(request);
	
	return urldata;
}


static PurpleUtilFetchUrlData *
np_util_post_url2(const gchar *url, const gchar *postdata,gchar *cookies,gboolean include_headers, PurpleUtilFetchUrlCallback callback, gpointer data)
{
	PurpleUtilFetchUrlData *urldata;
	gchar *host, *path, *request;
	int port;
    
    gchar *formatted_cookie;
    if (cookies && strlen(cookies)) {
        formatted_cookie = g_strdup_printf("Cookie: %s",cookies);
    } else {
        formatted_cookie = g_strdup_printf("");
    }
    
	purple_url_parse(url, &host, &port, &path, NULL, NULL);
	request = g_strdup_printf("POST /%s HTTP/1.1\r\n"
                              "Connection: close\r\n"
                              "Host: %s\r\n"
                              "Accept: */*\r\n"
                              "%s"
                              "Content-Type: application/x-www-form-urlencoded\r\n"
                              "Content-Length: %" G_GSIZE_FORMAT "\r\n\r\n%s",
                              path, host,formatted_cookie ,strlen(postdata), postdata);
	
	urldata = purple_util_fetch_url_request(url, TRUE, NULL, TRUE, request, include_headers, callback, data);
	
	g_free(formatted_cookie);
    g_free(host);
	g_free(path);
	g_free(request);
	
	return urldata;
}


PurpleUtilFetchUrlData *
np_http_login0(NPNotification *notification,PurpleUtilFetchUrlCallback callback)
{
    PurpleUtilFetchUrlData *url_data;
    PurpleAccount *account;
    NPSession *session;
    session = notification->session;
    
    account = session->account;
    
    const char *username = account->username;
    const char *pwd_md5 = g_compute_checksum_for_string(G_CHECKSUM_MD5,account->password,strlen(account->password));
    const char *http_server = notification->http_server ;//purple_account_get_string(session->account, "http_method_server", NP_HTTPCONN_SERVER);
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
    url_data = np_util_post_url(login_url, content,TRUE, callback, notification);
    
    g_free(plain_login_key);
    g_free(login_key);
	g_free(login_url);
    g_free(content);
    
    return url_data;
    
}

PurpleUtilFetchUrlData *
http_main_start(NPNotification *notitication,PurpleUtilFetchUrlCallback callback)
{
    PurpleUtilFetchUrlData *url_data;
    PurpleAccount *account;
    NPSession *session = notitication->session;
    
    account = session->account;
    
    char *http_server = notitication->http_server ;//purple_account_get_string(session->account, "http_method_server", NP_HTTPCONN_SERVER);
    const char *_ua = purple_account_get_string(session->account, "np_user_agent", NP_USER_AGENT);
    
    char *start_url = g_strdup_printf("%s/%s",http_server,NP_MAIN_START_PATH);
    purple_debug_info("np", "main/start: %s",start_url);
        
    gchar *content = g_strdup_printf("_ua=%s",_ua,NULL);
    
    url_data = np_util_post_url(start_url, content, FALSE , callback, notitication);
    
    g_free(content);
    
    return url_data;
    
}

PurpleUtilFetchUrlData *
http_get_friend_list(NPNotification *notification,PurpleUtilFetchUrlCallback callback)
{
    PurpleUtilFetchUrlData *url_data;
    PurpleAccount *account;
    gchar *cookies;
    
    NPSession *session;
    session = notification->session;

    account = session->account;
    cookies = np_session_get_encoded_cookie(session);

    char *http_server = notification->http_server;//purple_account_get_string(session->account, "http_method_server", NP_HTTPCONN_SERVER);
    const char *_ua = purple_account_get_string(session->account, "np_user_agent", NP_USER_AGENT);
    
    char *friend_list_url = g_strdup_printf("%s/%s",http_server,NP_FRIEND_LIST_PATH);

    purple_debug_info("np", "friendListURL: %s",friend_list_url);
    
    gchar *content = g_strdup_printf("_ua=%s",_ua,NULL);
    
    url_data = np_util_post_url2(friend_list_url, content,cookies, FALSE , callback, notification);
    
    g_free(content);

    return url_data;
}

PurpleUtilFetchUrlData *np_http_send_im(NPSession *session, const gchar *who, const gchar *message,PurpleMessageFlags flags)
{
    ///type=0&location=31.300000%2C121.490000&fid=22909360&related=0&content=%E5%96%94%20%E5%8E%9A%E9%81%93&c_mtime=1349798362558015
    NPNotification *notification;
    char *http_server;
    
    notification = session->notification;
    
    http_server = notification->http_server;
    const char *_ua = purple_account_get_string(session->account, "np_user_agent", NP_USER_AGENT);
    
    char *send_im_url = g_strdup_printf("%s/%s",http_server,NP_FRIEND_LIST_PATH);

}



