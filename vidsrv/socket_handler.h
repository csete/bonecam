/*
 * Copyright 2014 Alexandru Csete
 *
 * All rights reserved.
 * 
 * This Software is released under the "Simplified BSD License", see
 * license.txt for details
 */
#ifndef SOCKET_HANDLER_H
#define SOCKET_HANDLER_H

#include <gio/gio.h>

gboolean socket_callback(GSocketService *service, GSocketConnection * connection,
                         GObject * source_object, gpointer user_data);

#endif
