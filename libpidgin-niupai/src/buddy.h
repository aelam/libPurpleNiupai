

#include <buddy_list.h>


typedef _NiupaiBuddy NiupaiBuddy

struct _NiupaiBuddy {
    NiupaiBuddyList *buddy_list,

    char *id,
    chat *account_name,
    chat *store_name,
    chat *friendly_name,
    chat *idle,
};

