/*
 * Copyright 2014 Alexandru Csete
 *
 * All rights reserved.
 * 
 * This Software is released under the "Simplified BSD License", see
 * license.txt for details
 */
#include <getopt.h>
#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include "config.h"


static void show_help()
{
    static const char *help_message = \
        "\n"
        "Video server options:\n\n"
        "  -d <dev>   Camera device. Default is /dev/video0.\n"
        "  -s <size>  Image size in WxH format e.g. 1280x720 (default).\n"
        "  -f <num>   Frames per second. Range 5 - 30. Default is 24.\n"
        "  -b <num>   Bitrate in kilobits / sec. Range 50 - 15000. Default is 3000.\n"
        "  -i <num>   I-frame period in ms. Range 1000 - 60000. Default is 10000.\n"
        "  -a <addr>  Hostname or IP address of the mixer. Default is localhost.\n"
        "  -p <num>   UDP port on the mixer number to stream to. Default is 4000.\n"
        "  -c <num>   TCP command port number. Default is 4242.\n"
        "  -h         This help message.\n"
        "\n"
        "Use v4l2-ctl to set brightness, contrast, focus, etc.\n"
        "For example:\n"
        "  v4l2-ctl --set-ctrl=focus_auto=0\n"
        "  v4l2-ctl --set-ctrl=focus_absolute=0\n"
        "\n"
        "Use 'v4l2-ctl -l' for a list of supported settings\n";

    fprintf(stderr, "%s", help_message);
}

/* Check is requested frame size is supported by C920.
 * We only support down to 640x360
 */
static int size_is_ok(guint64 width, guint64 height)
{
    int sizeok = 0;

    switch (width)
    {
    case 1920:
        if (height == 1080)
            sizeok = 1;
        break;

    case 1600:
        if (height == 896)
            sizeok = 1;
        break;

    case 1280:
        if (height == 720)
            sizeok = 1;
        break;

    case 1024:
        if (height == 576)
            sizeok = 1;
        break;

    case 960:
        if (height == 720)
            sizeok = 1;
        break;

    case 864:
        if (height == 480)
            sizeok = 1;
        break;

    case 800:
        if (height == 600 || height == 448)
            sizeok = 1;
        break;

    case 640:
        if (height == 360)
            sizeok = 1;
        break;

    default:
        sizeok = 0;
        break;
    }

    return sizeok;
}

/* Extract frame size from string in the format 1280x720 */
void get_frame_size(const gchar *optarg, config_t* conf)
{
    gchar **size_vec = g_strsplit(optarg, "x", 0);

    guint64 w = g_ascii_strtoull(size_vec[0], NULL, 0);
    guint64 h = g_ascii_strtoull(size_vec[1], NULL, 0);

    if (size_is_ok(w, h))
    {
        conf->width = (unsigned int) w;
        conf->height = (unsigned int) h;
    }

    g_strfreev(size_vec);
}

unsigned int get_closest_framerate(int val)
{
    unsigned int fps = 24;

    if (val < 8)
        fps = 5;
    else if (val < 13)
        fps = 10;
    else if (val < 18)
        fps = 15;
    else if (val < 22)
        fps = 20;
    else if (val < 27)
        fps = 24;
    else if (val < 33)
        fps = 30;

    return fps;
}

config_t* config_create(int argc, char *argv[])
{
    config_t* conf = malloc(sizeof(config_t));

    /* add sensible defaults */
    conf->device = g_strdup("/dev/video0");
    conf->width = 1280;
    conf->height = 720;
    conf->framerate = 24;
    conf->bitrate = 3000;
    conf->iframe_period = 10000;
    conf->udp_host = g_strdup("localhost");
    conf->udp_port = 4000;
    conf->cmd_port = 4242;

    /* parse command line options */
    int opt,val;
    while ((opt = getopt(argc, argv, "d:s:f:b:i:a:p:h")) != -1)
    {
        switch (opt)
        {
        case 'd':
            g_free(conf->device);
            conf->device = g_strdup(optarg);
            break;
        case 's':
            get_frame_size(optarg, conf);
            break;
        case 'f':
            val = atoi(optarg);
            conf->framerate = get_closest_framerate(val);
            break;
        case 'b':
            val = atoi(optarg);
            if (val >= 50 && val <= 15000)
                conf->bitrate = val;
            else
                goto help;
            break;
        case 'i':
            val = atoi(optarg);
            if (val >= 1000 && val <= 60000)
                conf->iframe_period = val;
            else
                goto help;
            break;
        case 'a':
            g_free(conf->udp_host);
            conf->udp_host = g_strdup(optarg);
            break;
        case 'p':
            val = atoi(optarg);
            if (val >= 0 && val <= 65535)
                conf->udp_port = val;
            else
                goto help;
            break;
        case 'c':
            val = atoi(optarg);
            if (val >= 0 && val <= 65535)
                conf->cmd_port = val;
            else
                goto help;
        case 'h':
            goto help;
            break;
        default:
            break;
        }
    }

    return conf;

help:
    show_help();
    config_destroy(conf);
    exit(EXIT_FAILURE);
}


void config_destroy(config_t* conf)
{
    if (conf->device != NULL)
        g_free(conf->device);

    if (conf->udp_host != NULL)
        g_free(conf->udp_host);

    free(conf);
    conf = NULL;
}
