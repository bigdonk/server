/* vi: set ts=2:
 *
 * $Id: illusion.c,v 1.1 2001/01/25 09:37:57 enno Exp $
 * Eressea PB(E)M host Copyright (C) 1998-2000
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
#include "illusion.h"

/* kernel includes */
#include <unit.h>
#include <faction.h>
#include <message.h>

/* libc includes */
#include <stdlib.h>

#define ILLUSIONMAX                 6

void
age_illusion(unit *u)
{
	if (u->age == ILLUSIONMAX) {
		add_message(&u->faction->msgs, new_message(u->faction,
			"warnillusiondissolve%u:unit", u));
	} else if (u->age > ILLUSIONMAX) {
		set_number(u, 0);
		add_message(&u->faction->msgs, new_message(u->faction,
			"illusiondissolve%u:unit", u));
	}
}
