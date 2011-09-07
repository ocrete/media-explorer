/*
 * Mex - a media explorer
 *
 * Copyright © 2011 Intel Corporation.
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

#ifndef _MEX_BG_IMAGE_PLUGIN
#define _MEX_BG_IMAGE_PLUGIN

#include <glib-object.h>
#include <gmodule.h>
#include <mex/mex.h>

G_BEGIN_DECLS

#define MEX_TYPE_BG_IMAGE_PLUGIN mex_bg_image_plugin_get_type()

#define MEX_BG_IMAGE_PLUGIN(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MEX_TYPE_BG_IMAGE_PLUGIN, MexBgImagePlugin))

#define MEX_BG_IMAGE_PLUGIN_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), MEX_TYPE_BG_IMAGE_PLUGIN, MexBgImagePluginClass))

#define MEX_IS_BG_IMAGE_PLUGIN(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MEX_TYPE_BG_IMAGE_PLUGIN))

#define MEX_IS_BG_IMAGE_PLUGIN_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), MEX_TYPE_BG_IMAGE_PLUGIN))

#define MEX_BG_IMAGE_PLUGIN_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), MEX_TYPE_BG_IMAGE_PLUGIN, MexBgImagePluginClass))

typedef struct _MexBgImagePluginPrivate MexBgImagePluginPrivate;

typedef struct {
  GObject parent;

  MexBgImagePluginPrivate *priv;
} MexBgImagePlugin;

typedef struct {
  GObjectClass parent_class;
} MexBgImagePluginClass;

GType mex_bg_image_plugin_get_type (void);

MexBgImagePlugin* mex_bg_image_plugin_new (void);

G_MODULE_EXPORT const GType mex_get_plugin_type (void);

G_END_DECLS

#endif