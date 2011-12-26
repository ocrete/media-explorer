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

#include <glib.h>

#include <mex/mex-model.h>
#include <mex/mex-generic-model.h>
#include <mex/mex-model-manager.h>
#include <mex/mex-utils.h>
#include <glib/gi18n.h>

enum {
  PROP_TITLE = 1,
  PROP_ICON_NAME,
  PROP_LENGTH,
  PROP_PLACEHOLDER_TEXT,
  PROP_DISPLAY_ITEM_COUNT,
  PROP_SORT_FUNC,
  PROP_SORT_DATA,
  PROP_ALWAYS_VISIBLE,
  PROP_CATEGORY,
  PROP_PRIORITY,
  PROP_SORT_FUNCTIONS,
  PROP_ALT_MODEL,
  PROP_ALT_MODEL_STRING,
  PROP_ALT_MODEL_ACTIVE
};

struct _MexGenericModelPrivate {
  GController *controller;
  GArray      *items;

  MexModelSortFunc sort_func;
  gpointer         sort_data;

  gchar *title;
  gchar *icon_name;
  gchar *placeholder_text;

  gchar     *category;
  GPtrArray *sort_functions;

  gint priority;

  MexModel *alt_model;
  gchar    *alt_model_string;
  guint     alt_model_active : 1;

  guint  display_item_count : 1;
  guint  always_visible     : 1;
};

#define GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj),           \
                                                       MEX_TYPE_GENERIC_MODEL, \
                                                       MexGenericModelPrivate))

static void mex_model_iface_init (MexModelIface *iface);

G_DEFINE_TYPE_WITH_CODE (MexGenericModel, mex_generic_model, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (MEX_TYPE_MODEL,
                                                mex_model_iface_init));

static MexContent *
get_content_internal (MexGenericModel *model,
                      guint            idx)
{
  MexGenericModelPrivate *priv = model->priv;

  if (idx >= priv->items->len) {
    return NULL;
  } else {
    return g_array_index (priv->items, MexContent *, idx);
  }
}

/*
 * MexModel implementation
 */

static gint
binary_search (GArray           *array,
               MexContent       *content,
               MexModelSortFunc  sort,
               gpointer          user_data)
{
  gint first, last, mid;

  first = 0;
  last = array->len == 0 ? 0 : array->len - 1;

  while (first <= last)
    {
      MexContent *e;
      gint cmp;

      mid = (first + last) / 2;
      e = g_array_index (array, MexContent *, mid);
      cmp = sort (e, content, user_data);
      if (cmp < 0)
        first = mid + 1;
      else if (cmp > 0)
        last = mid - 1;
      else
        return mid;
    }

  /* not found, encode first in a negative value to be used as the index
   * to insert into a sorted array */
  return - (first + 1);
}

static gint
mex_generic_model_insert_sorted (MexGenericModel *self,
                                 MexContent      *content)
{
  MexGenericModelPrivate *priv = self->priv;
  gint position;

  position = binary_search (priv->items, content,
                            priv->sort_func, priv->sort_data);
  if (position < 0)
    position = -position - 1;

  g_array_insert_val (priv->items, position, content);

  return position;
}

static void
mex_generic_model_add_content (MexModel   *model,
                               MexContent *content)
{
  MexGenericModel *gm = (MexGenericModel *) model;
  MexGenericModelPrivate *priv = gm->priv;
  GControllerReference *ref;
  gint pos;

  g_object_ref_sink (content);
  if (priv->sort_func && priv->items->len)
    {
      pos = mex_generic_model_insert_sorted (gm, content);
    }
  else
    {
      pos = priv->items->len;
      g_array_append_val (priv->items, content);
    }

  ref = g_controller_create_reference (priv->controller, G_CONTROLLER_ADD,
                                       G_TYPE_UINT, 1, pos);
  g_controller_emit_changed (priv->controller, ref);
  g_object_unref (ref);

  g_object_notify (G_OBJECT (model), "length");
}

static int
array_find (GArray   *array,
            gpointer  data)
{
  int i;

  for (i = 0; i < array->len; i++) {
    gpointer child = g_array_index (array, gpointer, i);
    if (child == data) {
      return i;
    }
  }

  return -1;
}

static void
mex_generic_model_remove_content (MexModel   *model,
                                  MexContent *content)
{
  MexGenericModel *gm = (MexGenericModel *) model;
  MexGenericModelPrivate *priv = gm->priv;
  GControllerReference *ref;
  int idx;

  idx = array_find (priv->items, content);
  if (idx == -1) {
    return;
  }

  ref = g_controller_create_reference (priv->controller, G_CONTROLLER_REMOVE,
                                       G_TYPE_UINT, 1, idx);

  g_controller_emit_changed (priv->controller, ref);
  g_object_unref (ref);

  g_array_remove_index (priv->items, idx);
  g_object_unref (content);

  g_object_notify (G_OBJECT (model), "length");
}

static void
mex_generic_model_clear (MexModel *model)
{
  MexGenericModel *gm = (MexGenericModel *) model;
  MexGenericModelPrivate *priv = gm->priv;
  GControllerReference *ref;
  gint i;

  ref = g_controller_create_reference (priv->controller, G_CONTROLLER_CLEAR,
                                       G_TYPE_NONE, 0);
  g_controller_emit_changed (priv->controller, ref);
  g_object_unref (ref);

  for (i = 0; i < priv->items->len; i++)
    {
      MexContent *child = g_array_index (priv->items, MexContent *, i);
      g_object_unref (child);
    }
  g_array_set_size (priv->items, 0);
}

static GController *
mex_generic_model_get_controller (MexModel *model)
{
  MexGenericModel *gm = (MexGenericModel *) model;

  return gm->priv->controller;
}

static MexContent *
mex_generic_model_get_content (MexModel *model,
                               guint     index_)
{
  MexGenericModel *gm = (MexGenericModel *) model;

  return get_content_internal (gm, index_);
}

typedef struct
{
  MexModelSortFunc sort_func;
  gpointer         userdata;
} MexGenericModelSortData;

static gint
mex_generic_model_sort_func (gconstpointer a,
                             gconstpointer b,
                             gpointer      userdata)
{
  MexGenericModelSortData *data = userdata;
  MexContent **ca = (MexContent **)a;
  MexContent **cb = (MexContent **)b;

  return data->sort_func (*ca, *cb, data->userdata);
}

static void
mex_generic_model_set_sort_func (MexModel         *model,
                                 MexModelSortFunc  sort_func,
                                 gpointer          userdata)
{
  MexGenericModel *gm = (MexGenericModel *) model;
  MexGenericModelPrivate *priv = gm->priv;

  MexGenericModelSortData data;
  GControllerReference *ref;

  if ((priv->sort_func == sort_func) &&
      (priv->sort_data == userdata))
    return;

  priv->sort_func = sort_func;
  priv->sort_data = userdata;

  /* Sort the existing array. Items inserted later will be insert-sorted
   * with a binary search.
   */
  if (sort_func)
    {
      data.sort_func = sort_func;
      data.userdata = userdata;
      g_array_sort_with_data (priv->items, mex_generic_model_sort_func, &data);
    }

  /* Emit the controller 'replace' signal so all views representing this model
   * are refreshed.
   */
  ref = g_controller_create_reference (priv->controller,
                                       G_CONTROLLER_REPLACE,
                                       G_TYPE_NONE, 0);
  g_controller_emit_changed (priv->controller, ref);
  g_object_unref (ref);
}

static gboolean
mex_generic_model_is_sorted (MexModel *model)
{
  MexGenericModel *gm = (MexGenericModel *) model;

  return gm->priv->sort_func != NULL;
}

static guint
mex_generic_model_get_length (MexModel *model)
{
  MexGenericModel *gm = (MexGenericModel *) model;

  MexGenericModelPrivate *priv = gm->priv;

  return priv->items->len;
}

static gint
mex_generic_model_index (MexModel   *model,
                         MexContent *content)
{
  return array_find (MEX_GENERIC_MODEL (model)->priv->items, content);
}

static MexModel *
mex_generic_model_get_model (MexModel *model)
{
  return model;
}

static void
mex_model_iface_init (MexModelIface *iface)
{
  iface->get_controller = mex_generic_model_get_controller;
  iface->get_content = mex_generic_model_get_content;
  iface->add_content = mex_generic_model_add_content;
  iface->remove_content = mex_generic_model_remove_content;
  iface->clear = mex_generic_model_clear;
  iface->set_sort_func = mex_generic_model_set_sort_func;
  iface->is_sorted = mex_generic_model_is_sorted;
  iface->get_length = mex_generic_model_get_length;
  iface->index = mex_generic_model_index;
  iface->get_model = mex_generic_model_get_model;
}

static void
mex_generic_model_finalize (GObject *object)
{
  MexGenericModel *self = (MexGenericModel *) object;
  MexGenericModelPrivate *priv = self->priv;

  g_free (priv->title);
  priv->title = NULL;

  g_free (priv->icon_name);
  priv->icon_name = NULL;

  g_free (priv->placeholder_text);
  priv->placeholder_text = NULL;

  G_OBJECT_CLASS (mex_generic_model_parent_class)->finalize (object);
}

static void
mex_generic_model_dispose (GObject *object)
{
  MexGenericModel *self = (MexGenericModel *) object;
  MexGenericModelPrivate *priv =self->priv;

  if (priv->controller) {
    mex_generic_model_clear (MEX_MODEL (self));
    g_object_unref (priv->controller);
    priv->controller = NULL;
  }

  G_OBJECT_CLASS (mex_generic_model_parent_class)->dispose (object);
}

static void
mex_generic_model_set_property (GObject      *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  MexGenericModel *self = (MexGenericModel *) object;
  MexGenericModelPrivate *priv = self->priv;

  switch (prop_id) {
  case PROP_TITLE:
    g_free (priv->title);
    priv->title = g_value_dup_string (value);
    break;

  case PROP_PLACEHOLDER_TEXT:
    g_free (priv->placeholder_text);
    priv->placeholder_text = g_value_dup_string (value);
    break;

  case PROP_ICON_NAME:
    g_free (priv->icon_name);
    priv->icon_name = g_value_dup_string (value);
    break;

  case PROP_DISPLAY_ITEM_COUNT:
    priv->display_item_count = g_value_get_boolean (value);
    break;

  case PROP_ALWAYS_VISIBLE:
    priv->always_visible = g_value_get_boolean (value);
    break;

  case PROP_CATEGORY:
    g_free (priv->category);
    priv->category = g_value_dup_string (value);
    g_object_notify (object, "category");
    break;

  case PROP_PRIORITY:
    priv->priority = g_value_get_int (value);
    g_object_notify (object, "priority");
    break;

  case PROP_ALT_MODEL:
    if (priv->alt_model)
      g_object_unref (priv->alt_model);
    priv->alt_model = g_value_dup_object (value);
    g_object_notify (object, "alt-model");
    break;

  case PROP_ALT_MODEL_STRING:
    g_free (priv->alt_model_string);
    priv->alt_model_string = g_value_dup_string (value);
    g_object_notify (object, "alt-model-string");
    break;

  case PROP_ALT_MODEL_ACTIVE:
    priv->alt_model_active = g_value_get_boolean (value);
    g_object_notify (object, "alt-model-active");
    break;

  default:
    break;
  }
}

static void
mex_generic_model_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  MexGenericModel *self = (MexGenericModel *) object;
  MexGenericModelPrivate *priv = self->priv;

  switch (prop_id) {
  case PROP_TITLE:
    g_value_set_string (value, priv->title);
    break;

  case PROP_PLACEHOLDER_TEXT:
    g_value_set_string (value, priv->placeholder_text);
    break;

  case PROP_ICON_NAME:
    g_value_set_string (value, priv->icon_name);
    break;

  case PROP_LENGTH:
    g_value_set_int (value, priv->items->len);
    break;

  case PROP_DISPLAY_ITEM_COUNT:
    g_value_set_boolean (value, priv->display_item_count);
    break;

  case PROP_SORT_FUNC:
    g_value_set_pointer (value, priv->sort_func);
    break;

  case PROP_SORT_DATA:
    g_value_set_pointer (value, priv->sort_data);
    break;

  case PROP_ALWAYS_VISIBLE:
    g_value_set_boolean (value, priv->always_visible);
    break;

  case PROP_CATEGORY:
    g_value_set_string (value, priv->category);
    break;

  case PROP_PRIORITY:
    g_value_set_int (value, priv->priority);
    break;

  case PROP_ALT_MODEL:
    g_value_set_object (value, priv->alt_model);
    break;

  case PROP_ALT_MODEL_STRING:
    g_value_set_string (value, priv->alt_model_string);
    break;

  case PROP_ALT_MODEL_ACTIVE:
    g_value_set_boolean (value, priv->alt_model_active);
    break;

  case PROP_SORT_FUNCTIONS:
    g_value_set_boxed (value, priv->sort_functions);
    break;

  default:
    break;
  }
}

static void
mex_generic_model_class_init (MexGenericModelClass *klass)
{
  GObjectClass *o_class = (GObjectClass *) klass;
  GParamSpec *pspec;

  o_class->dispose = mex_generic_model_dispose;
  o_class->finalize = mex_generic_model_finalize;
  o_class->set_property = mex_generic_model_set_property;
  o_class->get_property = mex_generic_model_get_property;

  pspec = g_param_spec_string ("placeholder-text", "placeholder-text",
                               "Text to use when the model is empty", "",
                               G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE);
  g_object_class_install_property (o_class, PROP_PLACEHOLDER_TEXT, pspec);

  pspec = g_param_spec_boolean ("display-item-count",
                                "Display item count",
                                "Whether to display the number of items",
                                TRUE,
                                G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE);
  g_object_class_install_property (o_class, PROP_DISPLAY_ITEM_COUNT, pspec);

  pspec = g_param_spec_boolean ("always-visible",
                                "Always Visible",
                                "Whether to always display this model",
                                FALSE,
                                G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE);
  g_object_class_install_property (o_class, PROP_ALWAYS_VISIBLE, pspec);

  /* MexModel properties */
  g_object_class_override_property (o_class, PROP_TITLE, "title");
  g_object_class_override_property (o_class, PROP_SORT_FUNC, "sort-function");
  g_object_class_override_property (o_class, PROP_SORT_DATA, "sort-data");
  g_object_class_override_property (o_class, PROP_ICON_NAME, "icon-name");
  g_object_class_override_property (o_class, PROP_LENGTH, "length");

  g_object_class_override_property (o_class, PROP_CATEGORY, "category");
  g_object_class_override_property (o_class, PROP_PRIORITY, "priority");
  g_object_class_override_property (o_class, PROP_SORT_FUNCTIONS,
                                    "sort-functions");
  g_object_class_override_property (o_class, PROP_ALT_MODEL, "alt-model");
  g_object_class_override_property (o_class, PROP_ALT_MODEL_STRING,
                                    "alt-model-string");
  g_object_class_override_property (o_class, PROP_ALT_MODEL_ACTIVE,
                                    "alt-model-active");


  g_type_class_add_private (klass, sizeof (MexGenericModelPrivate));
}

static void
mex_generic_model_init (MexGenericModel *self)
{
  MexGenericModelPrivate *priv = GET_PRIVATE (self);

  self->priv = priv;

  priv->items = g_array_new (FALSE, FALSE, sizeof (MexContent *));
  priv->controller = g_array_controller_new (priv->items);
  g_array_unref (priv->items);

  priv->placeholder_text = g_strdup ("");

  priv->display_item_count = TRUE;

  /* add default sort functions */
  priv->sort_functions = g_ptr_array_sized_new (5);

  g_ptr_array_add (priv->sort_functions,
                   mex_model_sort_func_info_new ("smart",
                                                 _("Unseen"),
                                                 mex_model_sort_smart_cb,
                                                 GINT_TO_POINTER (FALSE)));

  g_ptr_array_add (priv->sort_functions,
                   mex_model_sort_func_info_new ("atoz",
                                                 _("A to Z"),
                                                 mex_model_sort_alpha_cb,
                                                 GINT_TO_POINTER (FALSE)));


  g_ptr_array_add (priv->sort_functions,
                   mex_model_sort_func_info_new ("ztoa",
                                                 _("Z to A"),
                                                 mex_model_sort_alpha_cb,
                                                 GINT_TO_POINTER (TRUE)));

  g_ptr_array_add (priv->sort_functions,
                   mex_model_sort_func_info_new ("newest",
                                                 _("Newest"),
                                                 mex_model_sort_time_cb,
                                                 GINT_TO_POINTER (TRUE)));

  g_ptr_array_add (priv->sort_functions,
                   mex_model_sort_func_info_new ("oldest",
                                                 _("Oldest"),
                                                 mex_model_sort_time_cb,
                                                 GINT_TO_POINTER (FALSE)));
}

/**
 * mex_generic_model_new:
 * @title: String containing the title.
 * @icon_name: String containing the icon name.
 *
 * Creates an empty #MexGenericModel.
 *
 * Return value: (transfer full): A #MexGenericModel
 */
MexModel *
mex_generic_model_new (const gchar *title,
                       const gchar *icon_name)
{
  return g_object_new (MEX_TYPE_GENERIC_MODEL,
                       "title", title,
                       "icon-name", icon_name,
                       NULL);
}

const gchar *
mex_generic_model_get_title (MexGenericModel *model)
{
  g_return_val_if_fail (MEX_IS_GENERIC_MODEL (model), NULL);

  return model->priv->title;
}

const gchar *
mex_generic_model_get_icon_name (MexGenericModel *model)
{
  g_return_val_if_fail (MEX_IS_GENERIC_MODEL (model), NULL);

  return model->priv->icon_name;
}
