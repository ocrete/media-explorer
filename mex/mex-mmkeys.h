/*
 * Mex - a media explorer
 *
 * Copyright © 2010, 2011 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses>
 */


#ifndef __MEX_MMKEYS_H__
#define __MEX_MMKEYS_H__

#include <glib-object.h>
#include <mex/mex-volume-control.h>

G_BEGIN_DECLS

#define MEX_TYPE_MMKEYS mex_mmkeys_get_type()

#define MEX_MMKEYS(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MEX_TYPE_MMKEYS, MexMMkeys))

#define MEX_MMKEYS_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), MEX_TYPE_MMKEYS, MexMMkeysClass))

#define MEX_IS_MMKEYS(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MEX_TYPE_MMKEYS))

#define MEX_IS_MMKEYS_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), MEX_TYPE_MMKEYS))

#define MEX_MMKEYS_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), MEX_TYPE_MMKEYS, MexMMkeysClass))

typedef struct _MexMMkeys MexMMkeys;
typedef struct _MexMMkeysClass MexMMkeysClass;
typedef struct _MexMMkeysPrivate MexMMkeysPrivate;

struct _MexMMkeys
{
  GObject parent;

  MexMMkeysPrivate *priv;
};

struct _MexMMkeysClass
{
  GObjectClass parent_class;
};

GType           mex_mmkeys_get_type         (void) G_GNUC_CONST;

MexMMkeys *     mex_mmkeys_get_default      (void);

void            mex_mmkeys_grab_keys        (MexMMkeys *self);
void            mex_mmkeys_ungrab_keys      (MexMMkeys *self);

void            mex_mmkeys_set_stage        (MexMMkeys    *self,
                                             ClutterActor *stage);

G_END_DECLS

#endif /* __MEX_MMKEYS_H__ */
