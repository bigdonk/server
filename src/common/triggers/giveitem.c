/* vi: set ts=2:
 *
 *	$Id: giveitem.c,v 1.1 2001/01/25 09:37:57 enno Exp $
 *	Eressea PB(E)M host Copyright (C) 1998-2000
 *      Christian Schlittchen (corwin@amber.kn-bremen.de)
 *      Katja Zedel (katze@felidae.kn-bremen.de)
 *      Henning Peters (faroul@beyond.kn-bremen.de)
 *      Enno Rehling (enno@eressea-pbem.de)
 *      Ingo Wilken (Ingo.Wilken@informatik.uni-oldenburg.de)
 *
 * This program may not be used, modified or distributed without
 * prior permission by the authors of Eressea.
 */

#include <config.h>
#include <eressea.h>
#include "giveitem.h"

/* kernel includes */
#include <item.h>
#include <unit.h>

/* util includes */
#include <event.h>
#include <resolve.h>
#include <base36.h>

/* ansi includes */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/***
 ** give an item to someone
 **/

typedef struct giveitem_data {
	struct unit * u;
	const struct item_type * itype;
	int number;
} giveitem_data;

static void
giveitem_init(trigger * t)
{
	t->data.v = calloc(sizeof(giveitem_data), 1);
}

static void
giveitem_free(trigger * t)
{
	free(t->data.v);
}

static int
giveitem_handle(trigger * t, void * data)
{
	/* call an event handler on giveitem.
	 * data.v -> ( variant event, int timer )
	 */
	giveitem_data * td = (giveitem_data*)t->data.v;
	if (td->u!=NULL) {
		i_change(&td->u->items, td->itype, td->number);
	} else
		fprintf(stderr, "\aERROR: could not perform giveitem::handle()\n");
	unused(data);
	return 0;
}

static void
giveitem_write(const trigger * t, FILE * F)
{
	giveitem_data * td = (giveitem_data*)t->data.v;
	fprintf(F, "%s ", itoa36(td->u->no));
	fprintf(F, "%d %s ", td->number, td->itype->rtype->_name[0]);
}

static int
giveitem_read(trigger * t, FILE * F)
{
	giveitem_data * td = (giveitem_data*)t->data.v;
	char zText[128];
	int i;

	fscanf(F, "%s", zText);
	i = atoi36(zText);
	td->u = findunit(i);
	if (td->u==NULL) ur_add((void*)i, (void**)&td->u, resolve_unit);

	fscanf(F, "%d %s", &td->number, zText);
	td->itype = it_find(zText);
	assert(td->itype);

	return 1;
}

trigger_type tt_giveitem = {
	"giveitem",
	giveitem_init,
	giveitem_free,
	giveitem_handle,
	giveitem_write,
	giveitem_read
};

trigger *
trigger_giveitem(unit * u, const item_type * itype, int number)
{
	trigger * t = t_new(&tt_giveitem);
	giveitem_data * td = (giveitem_data*)t->data.v;
	td->number = number;
	td->u = u;
	td->itype = itype;
	return t;
}
