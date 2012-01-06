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

#ifndef __MEX_THUMBNAILER_H__
#define __MEX_THUMBNAILER_H__

#include <glib.h>

#include <mex-content.h>

typedef void (*MexThumbnailCallback) (const char *uri, gpointer user_data);

void mex_thumbnailer_generate (const char *url,
                               const char *mime_type,
                               MexThumbnailCallback callback,
                               gpointer user_data);

gchar * mex_get_thumbnail_path_for_uri (const gchar *uri);

#endif /* __MEX_THUMBNAILER_H__ */
