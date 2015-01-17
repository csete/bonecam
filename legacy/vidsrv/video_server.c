/*
 * Copyright 2014 Alexandru Csete
 *
 * All rights reserved.
 * 
 * This Software is released under the "Simplified BSD License", see
 * license.txt for details
 */
#include <stdio.h>
#include <stdlib.h>
#include <gst/gst.h>
#include "video_server.h"


/* Creates the uvch264src element */
static void create_input_source(video_server_t * server, const config_t * conf)
{
    server->source = gst_element_factory_make("uvch264src", "video-source");

    if (!server->source)
        return;

    if (conf->device != NULL)
        g_object_set(G_OBJECT(server->source), "device", conf->device, NULL);

    /* bitrate and I-frame interval */
    g_object_set(G_OBJECT(server->source),
                 "initial-bitrate", 1000 * conf->bitrate,
                 "average-bitrate", 1000 * conf->bitrate,
                 "iframe-period", conf->iframe_period, NULL);

    /* misc settings */
    g_object_set(G_OBJECT(server->source), "auto-start", TRUE, NULL);

}

/* Creates the UDP sink */
static void create_udp_sink(video_server_t * server, const config_t * conf)
{
    server->udpsink = gst_element_factory_make("udpsink", "video-udpsink");
    if (conf->udp_host != NULL)
        g_object_set(G_OBJECT(server->udpsink),
                     "host", conf->udp_host, "port", conf->udp_port, NULL);
    else
        g_object_set(G_OBJECT(server->udpsink),
                     "host", "localhost", "port", conf->udp_port, NULL);
}

static void create_h264_caps(video_server_t * server, const config_t * conf)
{
    server->h264caps = gst_caps_new_simple("video/x-h264",
                                           "width", G_TYPE_INT, conf->width,
                                           "height", G_TYPE_INT, conf->height,
                                           "framerate", GST_TYPE_FRACTION,
                                           conf->framerate, 1, NULL);
}

video_server_t *video_server_create(config_t * conf)
{
    video_server_t *server = (video_server_t *) malloc(sizeof(video_server_t));


    /* Create gstreamer elements */
    server->pipeline = gst_pipeline_new("bonecam-video");

    create_input_source(server, conf);
    create_h264_caps(server, conf);

    server->vqueue = gst_element_factory_make("queue", "video-queue");
    server->parser = gst_element_factory_make("h264parse", "video-parser");
    server->payloader = gst_element_factory_make("rtph264pay", "video-rtppay");

    create_udp_sink(server, conf);


    if (!server->pipeline || !server->source || !server->parser ||
        !server->vqueue || !server->payloader || !server->udpsink)
    {
        fprintf(stderr, "%s: At least one element could not be created.\n",
                __func__);
    }

    /* Add elements to pipeline */
    gst_bin_add_many(GST_BIN(server->pipeline),
                     server->source, server->vqueue, server->parser,
                     server->payloader, server->udpsink, NULL);

    /* Link elements */
    gst_element_link_pads(server->source, "vidsrc", server->vqueue, "sink");

    if (!gst_element_link_filtered(server->vqueue,
                                   server->parser, server->h264caps))
    {
        fprintf(stderr, "%s: Failed to link elements\n", __func__);
    }

    gst_caps_unref(server->h264caps);

    if (!gst_element_link_many(server->parser, server->payloader,
                               server->udpsink, NULL))
        fprintf(stderr, "%s: Failed to link elements\n", __func__);

    /* keep reference to config */
    server->conf = conf;

    return server;
}


void video_server_delete(video_server_t * server)
{
    /* FIXME: Does this unref child elements? */
    gst_object_unref(GST_OBJECT(server->pipeline));

    g_free(server);
}

void video_server_set_bitrate(video_server_t * server, unsigned int value)
{
    if (value >= 50 && value <= 15000)
    {
        fprintf(stderr, "New video bitrate: %d kbps\n", value);
        server->conf->bitrate = value;
        g_object_set(G_OBJECT(server->source),
                     "initial-bitrate", 1000 * value,
                     "average-bitrate", 1000 * value, NULL);
    }
}

void video_server_set_iframe_period(video_server_t * server,
                                    unsigned int value)
{
    if (value >= 1000 && value <= 60000)
    {
        server->conf->iframe_period = value;
        fprintf(stderr, "Video server playing -> ready\n");
        gst_element_set_state(server->pipeline, GST_STATE_READY);
        fprintf(stderr, "New video I-frame period: %d ms\n", value);
        g_object_set(G_OBJECT(server->source),
                     "initial-bitrate", 1000 * server->conf->bitrate,
                     "average-bitrate", 1000 * server->conf->bitrate,
                     "iframe-period", value, NULL);
        fprintf(stderr, "Video server ready -> playing\n");
        gst_element_set_state(server->pipeline, GST_STATE_PLAYING);
    }
}

void video_server_set_framerate(video_server_t * server, unsigned int value)
{
    server->conf->framerate = get_closest_framerate(value);

    fprintf(stderr, "Video server playing -> ready\n");
    gst_element_set_state(server->pipeline, GST_STATE_READY);
    fprintf(stderr, "New video framerate: %d fps\n", server->conf->framerate);
    gst_caps_set_simple(GST_CAPS(server->h264caps),
                        "framerate", GST_TYPE_FRACTION,
                        server->conf->framerate, 1, NULL);
    fprintf(stderr, "Video server ready -> playing\n");
    gst_element_set_state(server->pipeline, GST_STATE_PLAYING);

}

/* reset video frame size to what is stored in server->conf */
void video_server_reset_frame_size(video_server_t * server)
{
    fprintf(stderr, "Video server playing -> ready\n");
    gst_element_set_state(server->pipeline, GST_STATE_READY);
    fprintf(stderr, "New video frame size: %dx%d\n",
            server->conf->width, server->conf->height);
    gst_caps_set_simple(GST_CAPS(server->h264caps),
                        "width", G_TYPE_INT, server->conf->width,
                        "height", G_TYPE_INT, server->conf->height, NULL);
    fprintf(stderr, "Video server ready -> playing\n");
    gst_element_set_state(server->pipeline, GST_STATE_PLAYING);
}
