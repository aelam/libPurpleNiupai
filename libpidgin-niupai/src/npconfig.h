//
//  npconfig.h
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-18.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#ifndef __NP_NPCONFIG_H__
#define __NP_NPCONFIG_H__

#ifdef DEBUG
#include "debug.h"
#endif

/* Niupai Purple Version */
#define LIBNP_VERSION          "0.1"

/* Niupai Adium Version */
#define LIBNP_AD_VERSION       "0.1"

/* Niupai Purple Plugin ID */
#define NP_PLUGIN_ID           "prpl-ryanwang-np"
#define NP_PLUGIN_NAME         "Niupai"
#define NP_PLUGIN_VERSION      "0.1"

#define NP_PLUGIN_SUMMARY       "niupai Protocol Plugin"
#define NP_PLUGIN_DESCRIPTION   "niupai Protocol Plugin"
#define NP_PLUGIN_AUTHOR        "Ryan Wang <wanglun02@gmail.com>"
#define NP_PLUGIN_HOMEPAGE      "http://www.niupai.com"

#define NP_MAJOR_VERSION        30
#define NP_MINOR_VERSION        1


#define NP_ADIUM_PLUGIN_ID     "libpurple-ryanwang-np"


#define NP_BUF_LEN 8192

/* IM Server*/
#define NP_IM_SERVER            "ims.niupai.com"
#define NP_IM_PORT              9000

#define NP_HTTPCONN_SERVER      "http://mapi.niupai.com"

#define NP_LOGIN_0_PATH         "nologin/login"
#define NP_MAIN_START_PATH      "main/start"
#define NP_FRIEND_LIST_PATH     "friend/get"

#define NP_USER_AGENT           "i|5.0|niupai|4|niupai_ios_apple||320"

#endif /* __NP_NPCONFIG_H__ */
