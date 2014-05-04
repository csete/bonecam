/*
 * Copyright 2014 Alexandru Csete
 *
 * All rights reserved.
 * 
 * This Software is released under the "Simplified BSD License", see
 * license.txt for details
 */
#ifndef CONFIG_H
#define CONFIG_H

#include <glib.h>

typedef struct
{
    /* camera settings */
    gchar *       device;
    unsigned int  width;         /* pixels */
    unsigned int  height;        /* pixels */
    unsigned int  framerate;     /* frames / second */
    unsigned int  bitrate;       /* kilobits / second */
    unsigned int  iframe_period; /* keyframe period in milliseconds. */

    /* network settings */
    gchar *       udp_host;
    unsigned int  udp_port;
    unsigned int  cmd_port;
} config_t;


config_t* config_create(int argc, char *argv[]);
void      config_destroy(config_t* conf);

void get_frame_size(const gchar *optarg, config_t* conf);
unsigned int get_closest_framerate(int val);

#endif
