//
//  nphttputil.c
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-24.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#include "nphttputil.h"
#include "cipher.h"

static char *
np_user_agent()
{
    //    NSString *loginKey = [[NSString stringWithFormat:@"%@%@",deviceID,@"nPiGa:#)"] md5];

    gchar *ua = "i|5.0|niupai|4|niupai_ios_apple||320";
//    gchar *ua = "i%7C5.1%7Cniupai%7C4%7Cniupai_ios_apple%7C%7C320";
    return ua;
}

guchar *
md5(guchar *origin)
{
    PurpleCipher *cipher;
    PurpleCipherContext *context;
    guchar digest[16];
    guchar buf[33];

    cipher = purple_ciphers_find_cipher("md5");
    context = purple_cipher_context_new(cipher, NULL);
    purple_cipher_context_append(context, (const guchar *)origin, strlen((char *)origin));

    purple_cipher_context_digest(context, sizeof(digest), digest, NULL);
    purple_cipher_context_destroy(context);
    
    for (int i = 0; i < 16; i++)
        g_snprintf((gchar *)buf + (i*2), 3, "%02x", digest[i]);

    buf[32] = '\0';
    purple_debug_info("np", "============== > %s",buf);

    return buf;
}


//#define fb_util_get_url(url, callback, data) purple_util_fetch_url_request(NULL, url, TRUE, NULL, FALSE, NULL, FALSE, -1, callback, data)

static PurpleUtilFetchUrlData *
np_util_post_url(const gchar *url, const gchar *postdata, PurpleUtilFetchUrlCallback callback, gpointer data)
{
	PurpleUtilFetchUrlData *urldata;
	gchar *host, *path, *request;
	int port;
	
    guchar *pwd_md5 = md5("4541973");
    purple_debug_info("np", "============== >pwd_md5: %s",pwd_md5);

    
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

//PurpleUtilFetchUrlData *
//http_login0(NPSession *session,PurpleUtilFetchUrlCallback callback)
//{
//    PurpleUtilFetchUrlData *url_data;
//    PurpleAccount *account;
//    account = session->account;
//    
//    purp
//    //	const char *username = "wanglun02@gmail.com";//account->username;
//    //	const char *password = "f7ab800d2e1d5055d695c75d6a5c187f";//account->password; // Need md5 password
//    const char *http_server = "http://ztest2.mapi.niupai.com";//purple_account_get_string(session->account, "http_method_server", NP_HTTPCONN_SERVER);
//    
//    const char *ua = np_user_agent();
//    
//    char *login_url = g_strdup_printf("%s/%s?_au=%s",http_server,NP_LOGIN_0_PATH,ua);
////    char *login_url = g_strdup_printf("%s/%s",http_server,NP_LOGIN_0_PATH);
//    //    char *login_url = g_strdup_printf("%s/%s",http_server,NP_LOGIN_0_PATH);
//    purple_debug_info("np", "login_url: %s",login_url);
//    
//    //
//    //    const char *login_device_id = "6a46144be35ed93714589ebd50dc4a6f";
//    //    const char *login_from = "niupai";
//    //    const char *login_key = "dff1dab3bad3440dc13f1ed867bfb6fa";
//    
//    //    gchar *content = g_strdup_printf("account=%s&password=%s&login_from=%s&login_device_id=%s&login_key=%s",
//    //                              username,
//    //                              password,
//    //                              login_from,
//    //                              login_device_id,
//    //                              login_key,
//    //                              NULL
//    //                              );
//    gchar *content = "account=wanglun4541973@sina.com&login_device_id=47a17558eaf9d9b82073aa1195fabf9c&login_from=niupai&login_key=9be8740f5a89c981936769929a2e41cb&password=f7ab800d2e1d5055d695c75d6a5c187f";
//    
//    url_data = np_util_post_url(login_url, content, callback, session);
//    
//    //    g_free(content);
//	g_free(login_url);
//    
//    return url_data;
//    
//}

PurpleUtilFetchUrlData *
http_login0(NPSession *session,PurpleUtilFetchUrlCallback callback)
{
    PurpleUtilFetchUrlData *url_data;
    PurpleAccount *account;
    account = session->account;
    
//	const char *username = "wanglun02@gmail.com";//account->username;
//	const char *password = "f7ab800d2e1d5055d695c75d6a5c187f";//account->password; // Need md5 password
    const char *http_server = "http://ztest2.mapi.niupai.com";//purple_account_get_string(session->account, "http_method_server", NP_HTTPCONN_SERVER);
    
//    const char *ua = np_user_agent();

//    char *login_url = g_strdup_printf("%s/%s?_au=%s",http_server,NP_LOGIN_0_PATH,ua);
    char *login_url = g_strdup_printf("%s/%s",http_server,NP_LOGIN_0_PATH);
//    char *login_url = g_strdup_printf("%s/%s",http_server,NP_LOGIN_0_PATH);
  purple_debug_info("np", "login_url: %s",login_url);

//    
//    const char *login_device_id = "6a46144be35ed93714589ebd50dc4a6f";
//    const char *login_from = "niupai";
//    const char *login_key = "dff1dab3bad3440dc13f1ed867bfb6fa";

//    gchar *content = g_strdup_printf("account=%s&password=%s&login_from=%s&login_device_id=%s&login_key=%s",
//                              username,
//                              password,
//                              login_from,
//                              login_device_id,
//                              login_key,
//                              NULL
//                              );
    gchar *content = "account=wanglun4541973@sina.com&login_device_id=47a17558eaf9d9b82073aa1195fabf9c&login_from=niupai&login_key=9be8740f5a89c981936769929a2e41cb&password=f7ab800d2e1d5055d695c75d6a5c187f&_ua=i|5.0|niupai|4|niupai_ios_apple||320";
    
    url_data = np_util_post_url(login_url, content, callback, session);
    
//    g_free(content);
	g_free(login_url);

    return url_data;
    
}


