/* vi: set ts=2:
 *
 *	$Id: unitmessage.h,v 1.1 2001/01/25 09:37:57 enno Exp $
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

#ifndef UNITMESSAGE_H
#define UNITMESSAGE_H

/* all types we use are defined here to reduce dependencies */
struct trigger_type;
struct trigger;
struct unit;

extern struct trigger_type tt_unitmessage;
extern struct trigger * trigger_unitmessage(struct unit * target, const char * string, int type, int level);

#endif
