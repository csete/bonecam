/*
 * Copyright 2014 Alexandru Csete
 * 
 * All rights reserved.
 * 
 * This Software is released under the "Simplified BSD License", see
 * license.txt for details
 */
#include <glib.h>
#include <stdio.h>
#include "socket_handler.h"
#include "video_server.h"


/* command socket callback */
gboolean socket_callback(GSocketService * service, GSocketConnection * conn,
                         GObject * source_object, gpointer user_data)
{
    video_server_t * server = (video_server_t *)user_data;
    gchar            message[128];
    guint64          value;

    GInputStream * istream = g_io_stream_get_input_stream(G_IO_STREAM(conn));

    g_input_stream_read(istream, message, 128, NULL, NULL);

    /* Supported commands:
     *
     *   "b 5000"     set bitrate to 5 Mbps
     *   "i 3000"     set I-frame interval in msec
     *   "f 30"       set framerate in frames/sec
     *   "s 640x360"  set frame size
     */
    gchar **cmd_str = g_strsplit(message, " ", 0);
    if (g_strv_length(cmd_str) != 2)
    {
        fprintf(stderr, "Incorrect command syntax: %s", message);
        return FALSE;
    }

    switch (cmd_str[0][0])
    {
    case 'b':
        value = g_ascii_strtoull(cmd_str[1], NULL, 0);
        video_server_set_bitrate(server, (unsigned int) value);
        break;
    case 'i':
        value = g_ascii_strtoull(cmd_str[1], NULL, 0);
        video_server_set_iframe_period(server, (unsigned int) value);
        break;
    case 'f':
        value = g_ascii_strtoull(cmd_str[1], NULL, 0);
        video_server_set_framerate(server, (unsigned int) value);
        break;
    case 's':
        get_frame_size(cmd_str[1], server->conf);
        video_server_reset_frame_size(server);
        break;
    }

    g_strfreev(cmd_str);

    return FALSE;
}
