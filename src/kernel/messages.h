/*
Copyright (c) 1998-2015, Enno Rehling <enno@eressea.de>
Katja Zedel <katze@felidae.kn-bremen.de
Christian Schlittchen <corwin@amber.kn-bremen.de>

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
**/

#ifndef H_KRNL_MESSAGE
#define H_KRNL_MESSAGE
#ifdef __cplusplus
extern "C" {
#endif

#include <kernel/types.h>
#include <util/message.h>

    struct faction;
    struct msglevel;

    typedef struct mlist {
        struct mlist *next;
        struct message *msg;
    } mlist;

    typedef struct message_list {
        struct mlist *begin, **end;
    } message_list;

    extern void free_messagelist(message_list * msgs);

    typedef struct msglevel {
        /* used to set specialized msg-levels */
        struct msglevel *next;
        const struct message_type *type;
        int level;
    } msglevel;

    extern struct message *msg_message(const char *name, const char *sig, ...);
    extern struct message *msg_feedback(const struct unit *, struct order *cmd,
        const char *name, const char *sig, ...);
    extern struct message *add_message(struct message_list **pm,
    struct message *m);
    void addmessage(struct region *r, struct faction *f, const char *s,
        msg_t mtype, int level);

#define ADDMSG(msgs, mcreate) { message * m = mcreate; if (m) { assert(m->refcount>=1); add_message(msgs, m); msg_release(m); } }

    struct message * cmistake(const struct unit *u, struct order *ord, int mno, int mtype);
    struct message * msg_error(const struct unit * u, struct order *ord, int mno);
#ifdef __cplusplus
}
#endif
#endif
