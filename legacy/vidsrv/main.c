/*
 * Copyright 2014 Alexandru Csete
 *
 * All rights reserved.
 * 
 * This Software is released under the "Simplified BSD License", see
 * license.txt for details
 */
#include <gio/gio.h>
#include <glib-unix.h>
#include <gst/gst.h>
#include <stdio.h>

#include "config.h"
#include "socket_handler.h"
#include "video_server.h"


/* callback to manage GST messages on D-BUS */
static gboolean bus_cb(GstBus * bus, GstMessage * msg, gpointer data)
{
    GError         *error;
    gchar          *debug;
    GMainLoop      *loop = (GMainLoop *) data;

    switch (GST_MESSAGE_TYPE(msg))
    {
    case GST_MESSAGE_EOS:
        g_print("End of stream\n");
        g_main_loop_quit(loop);
        break;

    case GST_MESSAGE_ERROR:
        gst_message_parse_error(msg, &error, &debug);
        g_free(debug);

        g_printerr("Error: %s\n", error->message);
        g_error_free(error);

        g_main_loop_quit(loop);
        break;

    default:
        break;
    }

    return TRUE;
}


/* unix signal handler */
static gboolean signal_handler(gpointer data)
{
    fprintf(stderr, "Received termination signal\n");
    g_main_loop_quit((GMainLoop *) data);

    return FALSE;
}


int main(int argc, char *argv[])
{
    GMainLoop      *main_loop;
    GSocketService *socket;
    GError         *error = NULL;

    GstBus         *bus;
    guint           bus_watch_id;

    config_t       *conf;
    video_server_t *server;

    guint           major = glib_major_version;
    guint           minor = glib_minor_version;
    guint           micro = glib_micro_version;
    guint           nano = 0;

    fprintf(stderr, "Glib version %d.%d.%d\n", major, minor, micro);

    /* initialise gsatreamer */
    gst_init(&argc, &argv);
    gst_version(&major, &minor, &micro, &nano);
    fprintf(stderr, "GStreamer version %d.%d.%d (%d)\n",
            major, minor, micro, nano);

    conf = config_create(argc, argv);
    server = video_server_create(conf);
    main_loop = g_main_loop_new(NULL, FALSE);
    g_unix_signal_add(SIGHUP, signal_handler, main_loop);
    g_unix_signal_add(SIGTERM, signal_handler, main_loop);
    g_unix_signal_add(SIGINT, signal_handler, main_loop);

    /* Message handler */
    bus = gst_pipeline_get_bus(GST_PIPELINE(server->pipeline));
    bus_watch_id = gst_bus_add_watch(bus, bus_cb, main_loop);
    gst_object_unref(bus);

    /* command socket */
    socket = g_socket_service_new();
    g_socket_listener_add_inet_port((GSocketListener *) socket,
                                    conf->cmd_port, NULL, &error);
    if (error != NULL)
        fprintf(stderr, "%s\n", error->message);

    g_signal_connect(socket, "incoming", G_CALLBACK(socket_callback), server);
    g_socket_service_start(socket);

    /* Set the pipeline to "playing" state */
    fprintf(stderr, "Video server playing\n");
    gst_element_set_state(server->pipeline, GST_STATE_PLAYING);

    /* Iterate */
    fprintf(stderr, "Running...\n");
    g_main_loop_run(main_loop);


    /* Stop */
    fprintf(stderr, "Stopping video server\n");
    gst_element_set_state(server->pipeline, GST_STATE_NULL);

    fprintf(stderr, "Deleting pipeline\n");

    video_server_delete(server);

    g_source_remove(bus_watch_id);
    g_main_loop_unref(main_loop);

    return 0;
}
