#include <platform.h>
#include <kernel/types.h>

#include "curses.h"

#include <kernel/connection.h>
#include <kernel/curse.h>
#include <kernel/config.h>
#include <kernel/faction.h>
#include <kernel/message.h>
#include <kernel/region.h>
#include <kernel/save.h>
#include <kernel/terrain.h>
#include <kernel/unit.h>
#include <kernel/version.h>

#include "spells/regioncurse.h"
#include "spells/unitcurse.h"
#include "spells/shipcurse.h"
#include "spells/buildingcurse.h"

#include <util/attrib.h>
#include <util/language.h>
#include <util/rand.h>
#include <util/rng.h>
#include <util/resolve.h>
#include <util/storage.h>

#include <assert.h>

typedef struct wallcurse {
  curse *buddy;
  connection *wall;
} wallcurse;

void cw_init(attrib * a)
{
  curse *c;
  curse_init(a);
  c = (curse *) a->data.v;
  c->data.v = calloc(sizeof(wallcurse), 1);
}

void cw_write(const attrib * a, const void *target, storage * store)
{
  connection *b = ((wallcurse *) ((curse *) a->data.v)->data.v)->wall;
  curse_write(a, target, store);
  store->w_int(store, b->id);
}

typedef struct bresolve {
  unsigned int id;
  curse *self;
} bresolve;

static int resolve_buddy(variant data, void *addr);

static int cw_read(attrib * a, void *target, storage * store)
{
  bresolve *br = calloc(sizeof(bresolve), 1);
  curse *c = (curse *) a->data.v;
  wallcurse *wc = (wallcurse *) c->data.v;
  variant var;

  curse_read(a, store, target);
  br->self = c;
  br->id = store->r_int(store);

  var.i = br->id;
  ur_add(var, &wc->wall, resolve_borderid);

  var.v = br;
  ur_add(var, &wc->buddy, resolve_buddy);
  return AT_READ_OK;
}

attrib_type at_cursewall = {
  "cursewall",
  cw_init,
  curse_done,
  curse_age,
  cw_write,
  cw_read,
  ATF_CURSE
};

static int resolve_buddy(variant data, void *addr)
{
  curse *result = NULL;
  bresolve *br = (bresolve *) data.v;
  connection *b;

  assert(br->id > 0);
  b = find_border(br->id);

  if (b && b->from && b->to) {
    attrib *a = a_find(b->from->attribs, &at_cursewall);
    while (a && a->data.v != br->self) {
      curse *c = (curse *) a->data.v;
      wallcurse *wc = (wallcurse *) c->data.v;
      if (wc->wall->id == br->id)
        break;
      a = a->next;
    }
    if (!a || a->type != &at_cursewall) {
      a = a_find(b->to->attribs, &at_cursewall);
      while (a && a->type == &at_cursewall && a->data.v != br->self) {
        curse *c = (curse *) a->data.v;
        wallcurse *wc = (wallcurse *) c->data.v;
        if (wc->wall->id == br->id)
          break;
        a = a->next;
      }
    }
    if (a && a->type == &at_cursewall) {
      curse *c = (curse *) a->data.v;
      free(br);
      result = c;
    }
  } else {
    /* fail, object does not exist (but if you're still loading then 
      * you may want to try again later) */
    *(curse **) addr = NULL;
    return -1;
  }
  *(curse **) addr = result;
  return 0;
}

static void wall_init(connection * b)
{
  wall_data *fd = (wall_data *) calloc(sizeof(wall_data), 1);
  fd->countdown = -1;           /* infinite */
  b->data.v = fd;
}

static void wall_destroy(connection * b)
{
  free(b->data.v);
}

static void wall_read(connection * b, storage * store)
{
  static wall_data dummy;
  wall_data *fd = b->data.v ? (wall_data *) b->data.v : &dummy;
  variant mno;

  if (store->version < STORAGE_VERSION) {
    mno.i = store->r_int(store);
    fd->mage = findunit(mno.i);
    if (!fd->mage && b->data.v) {
      ur_add(mno, &fd->mage, resolve_unit);
    }
  } else {
    read_reference(&fd->mage, store, read_unit_reference, resolve_unit);
  }
  fd->force = store->r_int(store);
  if (store->version >= NOBORDERATTRIBS_VERSION) {
    fd->countdown = store->r_int(store);
  }
  fd->active = true;
}

static void wall_write(const connection * b, storage * store)
{
  wall_data *fd = (wall_data *) b->data.v;
  write_unit_reference(fd->mage, store);
  store->w_int(store, fd->force);
  store->w_int(store, fd->countdown);
}

static int wall_age(connection * b)
{
  wall_data *fd = (wall_data *) b->data.v;
  --fd->countdown;
  return (fd->countdown > 0) ? AT_AGE_KEEP : AT_AGE_REMOVE;
}

static region *wall_move(const connection * b, struct unit *u,
  struct region *from, struct region *to, boolean routing)
{
  wall_data *fd = (wall_data *) b->data.v;
  if (!routing && fd->active) {
    int hp = dice(3, fd->force) * u->number;
    hp = MIN(u->hp, hp);
    u->hp -= hp;
    if (u->hp) {
      ADDMSG(&u->faction->msgs, msg_message("firewall_damage",
          "region unit", from, u));
    } else
      ADDMSG(&u->faction->msgs, msg_message("firewall_death", "region unit",
          from, u));
    if (u->number > u->hp) {
      scale_number(u, u->hp);
      u->hp = u->number;
    }
  }
  return to;
}

static const char *b_namefirewall(const connection * b, const region * r,
  const faction * f, int gflags)
{
  const char *bname;
  unused(f);
  unused(r);
  unused(b);
  if (gflags & GF_ARTICLE)
    bname = "a_firewall";
  else
    bname = "firewall";

  if (gflags & GF_PURE)
    return bname;
  return LOC(f->locale, mkname("border", bname));
}

border_type bt_firewall = {
  "firewall", VAR_VOIDPTR,
  b_transparent,                /* transparent */
  wall_init,                    /* init */
  wall_destroy,                 /* destroy */
  wall_read,                    /* read */
  wall_write,                   /* write */
  b_blocknone,                  /* block */
  b_namefirewall,               /* name */
  b_rvisible,                   /* rvisible */
  b_finvisible,                 /* fvisible */
  b_uinvisible,                 /* uvisible */
  NULL,
  wall_move,
  wall_age
};

void convert_firewall_timeouts(connection * b, attrib * a)
{
  while (a) {
    if (b->type == &bt_firewall && a->type == &at_countdown) {
      wall_data *fd = (wall_data *) b->data.v;
      fd->countdown = a->data.i;
    }
  }
}

border_type bt_wisps = { /* only here for reading old data */
  "wisps", VAR_VOIDPTR,
  b_transparent,                /* transparent */
  0,                   /* init */
  wall_destroy,                 /* destroy */
  wall_read,                    /* read */
  0,                   /* write */
  b_blocknone,                  /* block */
  0,                   /* name */
  b_rvisible,                   /* rvisible */
  b_fvisible,                   /* fvisible */
  b_uvisible,                   /* uvisible */
  NULL,                         /* visible */
  0
};

void register_curses(void)
{
  border_convert_cb = &convert_firewall_timeouts;
  at_register(&at_cursewall);

  register_bordertype(&bt_firewall);
  register_bordertype(&bt_wisps);
  register_bordertype(&bt_chaosgate);

  register_unitcurse();
  register_regioncurse();
  register_shipcurse();
  register_buildingcurse();
}
