/*
 * Copyright © 2009 Codethink Limited
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of version 3 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * See the included COPYING file for more information.
 */

#include "dconfstorage.h"

#include <string.h>
#include <dconf.h>

struct OPAQUE_TYPE__DConfStorage
{
  GSettingsStorage parent_instance;
  gchar *blocked_event;
  gchar *base;

  GList *item_list;
  GList item_tail;
};

struct OPAQUE_TYPE__DConfStorageClass
{
  GSettingsStorageClass parent_class;
};

G_DEFINE_TYPE (DConfStorage, dconf_storage, G_TYPE_SETTINGS_STORAGE)

typedef struct
{
  gchar *prefix;
  gint prefix_len;
  GTree *values;
} DConfStorageItem;

static void
dconf_storage_merge_complete (DConfAsyncResult *result,
                              gpointer          user_data)
{
  DConfStorage *storage = user_data;
  DConfStorageItem *item;
  GError *error = NULL;
  GList *head;

  head = g_list_first (storage->item_list);
  item = head->data;
  storage->item_list = g_list_delete_link (storage->item_list, head);

  if (storage->blocked_event)
    {
      g_free (storage->blocked_event);
      storage->blocked_event = NULL;
    }

  if (!dconf_merge_finish (result, &storage->blocked_event, &error))
    {
      g_warning ("%s", error->message);
      g_error_free (error);

      g_settings_storage_changed_tree (G_SETTINGS_STORAGE (storage),
                                       item->prefix, item->values, NULL);
    }

  g_tree_destroy (item->values);
  g_free (item->prefix);
  g_slice_free (DConfStorageItem, item);
}

static void
dconf_storage_write (GSettingsStorage     *g_storage,
                     const gchar          *prefix,
                     GTree                *values,
                     gpointer              origin_tag)
{
  DConfStorage *storage = DCONF_STORAGE (g_storage);
  DConfStorageItem *item;
  gchar *path;

  item = g_slice_new (DConfStorageItem);
  g_assert (prefix != NULL);
  item->prefix = g_strdup (prefix);
  item->values = g_tree_ref (values);
  item->prefix_len = strlen (item->prefix);

  path = g_strconcat (storage->base, prefix, NULL);
  dconf_merge_tree_async (path, values,
                          dconf_storage_merge_complete,
                          storage);
  g_free (path);

  storage->item_list = g_list_insert_before (storage->item_list,
                                             &storage->item_tail,
                                             item);

  g_settings_storage_changed_tree (G_SETTINGS_STORAGE (storage),
                                   prefix, values, origin_tag);
}

static GVariant *
dconf_storage_read (GSettingsStorage   *g_storage,
                    const gchar        *key,
                    const GVariantType *expected_type)
{
  DConfStorage *storage = DCONF_STORAGE (g_storage);
  GVariant *result;
  GList *node;

  for (node = storage->item_list; node != &storage->item_tail; node = node->next)
    {
      DConfStorageItem *item = node->data;

      if (g_str_has_prefix (key, item->prefix))
        if ((result = g_tree_lookup (item->values,
                                     key + item->prefix_len)))
          {
            g_variant_ref (result);
            break;
          }
    }

  if (node == &storage->item_tail)
    {
      gchar *path;

      path = g_strjoin (NULL, storage->base, key, NULL);
      result = dconf_get (path);
    }

  return result;
}

static void
dconf_storage_notify (const gchar         *prefix,
                      const gchar * const *items,
                      gint                 n_items,
                      const gchar         *event_id,
                      gpointer             user_data)
{
  DConfStorage *storage = DCONF_STORAGE (user_data);

  if (g_strcmp0 (storage->blocked_event, event_id) == 0)
    {
      g_free (storage->blocked_event);
      storage->blocked_event = NULL;
      return;
    }

  if (!g_str_has_prefix (prefix, storage->base))
    return;

g_print ("changed %s\n", prefix);
  g_settings_storage_changed (G_SETTINGS_STORAGE (storage),
                              prefix + strlen (storage->base),
                              items, n_items, NULL);
}

static void
dconf_storage_subscribe (GSettingsStorage *g_storage,
                         const gchar      *name)
{
  DConfStorage *storage = DCONF_STORAGE (g_storage);
  gchar *full;

  full = g_strdup_printf ("%s%s", storage->base, name);
  dconf_watch (full, dconf_storage_notify, g_storage);
  g_free (full);
}

static void
dconf_storage_unsubscribe (GSettingsStorage *g_storage,
                           const gchar      *name)
{
  DConfStorage *storage = DCONF_STORAGE (g_storage);
  gchar *full;

  full = g_strdup_printf ("%s%s", storage->base, name);
  dconf_unwatch (full, dconf_storage_notify, g_storage);
  g_free (full);
}

static gboolean
dconf_storage_sensitive (GSettingsStorage *g_storage,
                         const gchar      *name)
{
  return TRUE;
}

static void
dconf_storage_init (DConfStorage *storage)
{
  storage->item_list = &storage->item_tail;
}

static void
dconf_storage_constructed (GObject *object)
{
  DConfStorage *storage = DCONF_STORAGE (object);

  if (storage->base == NULL)
    storage->base = g_strdup ("/user/");
}

static void
dconf_storage_finalize (GObject *object)
{
  DConfStorage *storage = DCONF_STORAGE (object);

  g_free (storage->base);
}

static void
dconf_storage_class_init (DConfStorageClass *class)
{
  GSettingsStorageClass *storage_class = G_SETTINGS_STORAGE_CLASS (class);

  GObjectClass *object_class = G_OBJECT_CLASS (class);
  object_class->constructed = dconf_storage_constructed;
  object_class->finalize = dconf_storage_finalize;

  storage_class->read = dconf_storage_read;
  storage_class->write = dconf_storage_write;
  storage_class->subscribe = dconf_storage_subscribe;
  storage_class->unsubscribe = dconf_storage_unsubscribe;
  storage_class->sensitive = dconf_storage_sensitive;
}