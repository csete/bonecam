/*
 * Copyright 2014 Alexandru Csete
 *
 * All rights reserved.
 * 
 * This Software is released under the "Simplified BSD License", see
 * license.txt for details
 */
#ifndef VIDEO_SERVER_H
#define VIDEO_SERVER_H

#include <gst/gst.h>
#include "config.h"

typedef struct
{
    config_t   *conf;
    GstElement *pipeline;
    GstElement *source;
    GstElement *vqueue;
    GstElement *parser;
    GstElement *payloader;
    GstElement *udpsink;
    GstCaps    *h264caps;
} video_server_t;


video_server_t * video_server_create(config_t *conf);
void video_server_delete(video_server_t * server);
void video_server_set_bitrate(video_server_t * server, unsigned int value);
void video_server_set_iframe_period(video_server_t * server, unsigned int value);
void video_server_set_framerate(video_server_t * server, unsigned int value);
void video_server_reset_frame_size(video_server_t * server);

#endif
