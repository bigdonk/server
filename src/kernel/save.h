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

#ifndef H_KRNL_SAVE
#define H_KRNL_SAVE

#include <stream.h>
#ifdef __cplusplus
extern "C" {
#endif

    struct attrib;
    struct item;
    struct storage;
    struct spell;
    struct spellbook;
    struct unit;

    typedef struct gamedata {
        struct storage *store;
        stream strm;
        int version;
        int encoding;
    } gamedata;

#define MAX_INPUT_SIZE	DISPLAYSIZE*2
    /* Nach MAX_INPUT_SIZE brechen wir das Einlesen der Zeile ab und nehmen an,
     * dass hier ein Fehler (fehlende ") vorliegt */

    int readorders(const char *filename);
    int creategame(void);
    int readgame(const char *filename, int backup);
    int writegame(const char *filename);

    /* Versionsänderungen: */
    extern int data_version;
    extern int enc_gamedata;

    extern int current_turn(void);

    extern void read_items(struct storage *store, struct item **it);
    extern void write_items(struct storage *store, struct item *it);

    extern void read_spellbook(struct spellbook **bookp, struct storage *store, int(*get_level)(const struct spell * sp, void *), void * cbdata);
    extern void write_spellbook(const struct spellbook *book, struct storage *store);

    extern void write_unit(struct gamedata *data, const struct unit *u);
    extern struct unit *read_unit(struct gamedata *data);

    extern int a_readint(struct attrib *a, void *owner, struct storage *store);
    extern void a_writeint(const struct attrib *a, const void *owner,
    struct storage *store);
    extern int a_readshorts(struct attrib *a, void *owner, struct storage *store);
    extern void a_writeshorts(const struct attrib *a, const void *owner,
    struct storage *store);
    extern int a_readchars(struct attrib *a, void *owner, struct storage *store);
    extern void a_writechars(const struct attrib *a, const void *owner,
    struct storage *store);
    extern int a_readvoid(struct attrib *a, void *owner, struct storage *store);
    extern void a_writevoid(const struct attrib *a, const void *owner,
    struct storage *store);
    extern int a_readstring(struct attrib *a, void *owner, struct storage *store);
    extern void a_writestring(const struct attrib *a, const void *owner,
    struct storage *store);
    extern void a_finalizestring(struct attrib *a);

    extern void create_backup(char *file);

#ifdef __cplusplus
}
#endif
#endif
