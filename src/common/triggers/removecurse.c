/* vi: set ts=2:
 *
 *	$Id: removecurse.c,v 1.1 2001/01/25 09:37:57 enno Exp $
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
#include "removecurse.h"

/* kernel includes */
#include <curse.h>
#include <unit.h>

/* util includes */
#include <event.h>
#include <resolve.h>
#include <base36.h>

/* ansi includes */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#include <stdio.h>

typedef struct removecurse_data {
	curse * curse;
	unit * target;
} removecurse_data;

static void
removecurse_init(trigger * t)
{
	t->data.v = calloc(sizeof(removecurse_data), 1);
}

static void
removecurse_free(trigger * t)
{
	free(t->data.v);
}

static int
removecurse_handle(trigger * t, void * data)
{
	/* call an event handler on removecurse.
	 * data.v -> ( variant event, int timer )
	 */
	removecurse_data * td = (removecurse_data*)t->data.v;
	if (td->curse!=NULL && td->target!=NULL) {
		attrib * a = a_select(td->target->attribs, td->curse, cmp_curse);
		if (a) {
			a_remove(&td->target->attribs, a);
		}
		else fprintf(stderr, "\aERROR: could not perform removecurse::handle()\n");
	} else {
		fprintf(stderr, "\aERROR: could not perform removecurse::handle()\n");
	}
	unused(data);
	return 0;
}

static void
removecurse_write(const trigger * t, FILE * F)
{
	removecurse_data * td = (removecurse_data*)t->data.v;
	fprintf(F, "%s ", itoa36(td->target->no));
	fprintf(F, "%d ", td->curse->no);
}

static int
removecurse_read(trigger * t, FILE * F)
{
	removecurse_data * td = (removecurse_data*)t->data.v;
	char zText[128];
	int i;

	fscanf(F, "%s", zText);
	i = atoi36(zText);
	td->target = findunit(i);
	if (td->target==NULL) ur_add((void*)i, (void**)&td->target, resolve_unit);

	fscanf(F, "%d", &i);
	td->curse = cfindhash(i);
	if (td->curse==NULL) ur_add((void*)i, (void**)&td->curse, resolve_curse);

	return 1;
}

trigger_type tt_removecurse = {
	"removecurse",
	removecurse_init,
	removecurse_free,
	removecurse_handle,
	removecurse_write,
	removecurse_read
};

trigger *
trigger_removecurse(curse * c, unit * target)
{
	trigger * t = t_new(&tt_removecurse);
	removecurse_data * td = (removecurse_data*)t->data.v;
	td->curse = c;
	td->target = target;
	return t;
}
