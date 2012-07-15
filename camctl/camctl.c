/* Simple bonecam control interface.
 *
 * Copyright 2012 Alexandru Csete
 *
 * May be (ab)used under GNU GPL v3
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* defaults */
#define DEFAULT_TTY       "/dev/ttyO1"
#define DEFAULT_CLIENT    "mixer"
#define DEFAULT_PORT      "4000"
#define DEFAULT_WIDTH     1024
#define DEFAULT_HEIGHT    576
#define DEFAULT_FPS       24

/* servo numbers */
#define AZI 0
#define ELE 1


typedef struct _v4l2ctl {
    char   cli;   /* camctl cli command */
    char  *cmd;   /* v4l2-ctl command */
    int    min;   /* minimum value */
    int    max;   /* max valiue */
    int    step;  /* step size */
    int    def;   /* default value */
    char  *cmd_auto; /* command to enable/disable auto mode */
    int    cmd_auto_off; /* value that disable auto mode */
    int    cmd_auto_on;  /* value that enables auto mode */
} v4l2ctl_t;

/* C920 command table */
static v4l2ctl_t ctltab[] = {
    { 'b', "brightness", 0, 255, 1, 128, NULL, 0, 0},
    { 'c', "contrast",   0, 255, 1, 128, NULL, 0, 0},
    { 's', "saturation", 0, 255, 1, 128, NULL, 0, 0},
    { 'h', "sharpness",  0, 255, 1, 128, NULL, 0, 0},
    { 'o', "backlight_compensation", 0, 1, 1, 0, NULL, 0, 0},
    { 'l', "power_line_frequency", 0, 2, 1, 1, NULL, 0, 0},
    { 'g', "gain", 0, 255, 1, 0, NULL, 0, 0 },
    { 'x', "exposure_auto_priority", 0, 1, 1, 1, NULL, 0, 0},
    { 'z', "zoom_absolute", 100, 500, 1, 100, NULL, 0, 0},
    { 'w', "white_balance_temperature", 2000, 6500, 1, 4000, "white_balance_temperature_auto", 0, 1},
    { 'f', "focus_absolute", 0, 250, 5, 0, "focus_auto", 0, 1},
    { 'e', "exposure_absolute", 0, 2047, 1, 250, "exposure_auto", 1, 3},
    { 0, NULL, 0, 0, 0, 0, NULL, 0, 0} /* used to find end of array */
};


#define VIDEO_SERVER_CMD "/home/root/bin/video-server"

static void init_servos();
static void init_camera(unsigned int w, unsigned int h, unsigned int fps);
static void start_camera(const char *client, const char *port);
static void stop_camera();
static void get_camera_ctl(const char *ctl);
static void set_camera_ctl(const char *ctl, int val);
static void set_speed(int servo, int speed);
static void set_angle(int servo, int angle);
static void write_tty(const char *tty, const char *data, int len);
static int  chr2idx(char c);
static void show_help();


int main(int argc, char *argv[])
{
    int rc  =  0;
    int idx = -1;


    if (argc == 1)
    {
        show_help();
        return 1;
    }

    /* first check whether we have init/start/stop */
    if (!strcmp(argv[1], "init"))
    {
        int w = DEFAULT_WIDTH;
        int h = DEFAULT_HEIGHT;
        int fps = DEFAULT_FPS;

        if (argc > 2)
            w = atoi(argv[2]);

        if (argc > 3)
            h = atoi(argv[3]);

        if (argc > 4)
            fps = atoi(argv[4]);

        init_camera(w, h, fps);
        init_servos();

        goto done;
    }
    else if (!strcmp(argv[1], "start"))
    {
        switch (argc)
        {
        case 2:
            /* Use default IP and port */
            start_camera(DEFAULT_CLIENT, DEFAULT_PORT);
            break;
        case 3:
            /* IP given; use default port */
            start_camera(argv[2], DEFAULT_PORT);
            break;
        case 4:
            /* both IP and port givem */
            start_camera(argv[2], argv[3]);
            break;
        default:
            show_help();
            break;
        }
        goto done;
    }
    else if (!strcmp(argv[1], "stop"))
    {
        stop_camera();
        goto done;
    }
    else if (!strcmp(argv[1], "status"))
    {
        printf("cam status not implemented\n");
        goto done;
    }

    /* now check for p, s, m ... commands */
    char *s = argv[1];

    switch (*s)
    {
    case 'p':
        switch (*++s)
        {
        case 0:
            if (argc < 4)
            {
                show_help();
            }
            else
            {
                set_angle(AZI, atoi(argv[2]));
                set_angle(ELE, atoi(argv[3]));
            }
            break;
        case 'a':
            if (argc < 3)
                show_help();
            else
                set_angle(AZI, atoi(argv[2]));
            break;
        case 'e':
            if (argc < 3)
                show_help();
            else
                set_angle(ELE, atoi(argv[2]));
            break;
        default:
            show_help();
            goto done;
        }
        break;

    case 'r':
        switch (*++s)
        {
        case 0:
            if (argc < 4)
            {
                show_help();
            }
            else
            {
                set_speed(AZI, atoi(argv[2]));
                set_speed(ELE, atoi(argv[3]));
            }
            break;
        case 'a':
            if (argc < 3)
                show_help();
            else
                set_speed(AZI, atoi(argv[2]));
            break;
        case 'e':
            if (argc < 3)
                show_help();
            else
                set_speed(ELE, atoi(argv[2]));
            break;
        default:
            show_help();
            goto done;
        }
        break;

    case 'm':
        switch (*++s)
        {
        case 0:
            if (argc < 5)
            {
                show_help();
            }
            else
            {
                set_speed(AZI, atoi(argv[4]));
                set_speed(ELE, atoi(argv[4]));
                set_angle(AZI, atoi(argv[2]));
                set_angle(ELE, atoi(argv[3]));
            }
            break;
        case 'a':
            if (argc < 4)
            {
                show_help();
            }
            else
            {
                set_speed(AZI, atoi(argv[3]));
                set_angle(AZI, atoi(argv[2]));
            }
            break;
        case 'e':
            if (argc < 4)
            {
                show_help();
            }
            else
            {
                set_speed(ELE, atoi(argv[3]));
                set_angle(ELE, atoi(argv[2]));
            }
            break;
        default:
            show_help();
            goto done;
        }
        break;

    case 'z':
        switch (*++s)
        {
        case 0:
            if (argc < 3)
            {
                /* read current value */
                get_camera_ctl("zoom_absolute");
            }
            else
            {
                /* set new value */
                int val = atoi(argv[2]);
                if ((val >= 100) && (val <= 500))
                    set_camera_ctl("zoom_absolute", val);
            }
            break;
        case 'r':
            /* reset zoom */
            set_camera_ctl("zoom_absolute", 100);
            set_camera_ctl("tilt_absolute", 0);
            set_camera_ctl("pan_absolute", 0);
            break;
        case 'p':
            if (argc < 3)
            {
                /* read current value */
                get_camera_ctl("pan_absolute");
            }
            else
            {
                /* set new value */
                int val = atoi(argv[2]);
                if ((val >= -10) && (val <= +10))
                    set_camera_ctl("pan_absolute", 3600*val);
            }
            break;
        case 't':
            if (argc < 3)
            {
                /* read current value */
                get_camera_ctl("tilt_absolute");
            }
            else
            {
                /* set new value */
                int val = atoi(argv[2]);
                if ((val >= -10) && (val <= +10))
                    set_camera_ctl("tilt_absolute", 3600*val);
            }
            break;
        default:
            show_help();
            goto done;
        }
        break;

    /* camera controls listed in ctltab */
    case 'b':
    case 'c':
    case 's':
    case 'h':
    case 'o':
    case 'l':
    case 'g':
    case 'x':
    case 'w':
    case 'f':
    case 'e':
        idx = chr2idx(*s);
        if (idx == -1)
        {
            printf("Internal error (v4l2ctl: %c  idx:-1)\n", *s);
            goto done;
        }

        switch (*++s)
        {
        case 'r':
            /* if ctl has 'auto' setting set to manual */
            if (ctltab[idx].cmd_auto != NULL)
                set_camera_ctl(ctltab[idx].cmd_auto, ctltab[idx].cmd_auto_off);

            /* reset control to its default value */
            set_camera_ctl(ctltab[idx].cmd, ctltab[idx].def);
            break;
        case 0:
            if (argc < 3)
            {
                /* read current value */
                get_camera_ctl(ctltab[idx].cmd);
                if (ctltab[idx].cmd_auto != NULL)
                    get_camera_ctl(ctltab[idx].cmd_auto);
            }
            else
            {
                /* set new value */
                int val = atoi(argv[2]);
                if (val == -1)
                {
                    /* use auto setting if we have that */
                    if (ctltab[idx].cmd_auto != NULL)
                    {
                        set_camera_ctl(ctltab[idx].cmd_auto, ctltab[idx].cmd_auto_on);
                    }
                    else
                    {
                        printf("Command %s does not have 'auto' setting.\n",
                               ctltab[idx].cmd);
                    }
                }
                else if ((val >= ctltab[idx].min) && (val <= ctltab[idx].max))
                {
                    /* if cmd has 'auto' setting reset it to manual */
                    if (ctltab[idx].cmd_auto != NULL)
                        set_camera_ctl(ctltab[idx].cmd_auto, ctltab[idx].cmd_auto_off);

                    set_camera_ctl(ctltab[idx].cmd, val);
                }
            }
            break;
        }
        break;

    default:
        show_help();
        break;
    }

done:
    return rc;
}

/* Initialise servo interface and controller */
static void init_servos()
{
    int rc;

    /* setup TTY */
    rc = system("echo 20 > /sys/kernel/debug/omap_mux/uart1_rxd");
    rc = system("echo 0 > /sys/kernel/debug/omap_mux/uart1_txd");

    if (rc)
        printf("Error (%d) setting up UART.\n", rc);

    /* set servos to speed 5 */
    set_speed(AZI, 5);
    set_speed(ELE, 5);

    /* set angles */
    set_angle(AZI, 90);
    set_angle(ELE, 40);
}

/* Set servo angle */
static void set_angle(int servo, int angle)
{
    char cmd[7];
    int data1=0, data2=0;

    /* The angle goes from 0 to 180:
         0x00 0x00  -  0 degrees
         0x01 0x00  -  90 degrees
         0x01 0x7F  -  180 degrees
    */
    if (angle >= 90)
    {
        data1 = 1;
        angle -= 90;
    }
    data2 = (angle * 127) / 90;

    sprintf(cmd, "%c%c%c%c%c%c", 0x80, 0x01, 0x03, servo, data1, data2);
    write_tty(DEFAULT_TTY, cmd, 6);
}

/* Set servo speed 0..15 */
static void set_speed(int servo, int speed)
{
    char cmd[6];
    sprintf(cmd, "%c%c%c%c%c", 0x80, 0x01, 0x01, servo, speed);
    write_tty(DEFAULT_TTY, cmd, 5);
}

/* Write data to character device. */
static void write_tty(const char *tty, const char *data, int len)
{
    /* note: data must be 0-terminated */
    FILE *fp = fopen(tty, "w");

    if (!fp)
    {
        printf("Error opening %s\n", tty);
        return;
    }
    fwrite(data, 1, len, fp);
    fclose(fp);
}

/* Initialise camera to given frame size and frame rate */
static void init_camera(unsigned int w, unsigned int h, unsigned int fps)
{
    int rc;
    char cmd[100];

    printf("Initialising camera to %ux%u @ %u fps\n", w, h, fps);

    /* frame size and format */
    sprintf(cmd,"v4l2-ctl --set-fmt-video=width=%u,height=%u,pixelformat=1",
            w, h);
    rc = system(cmd);
    printf("  Set frame size: %s\n", rc ? "Not ok" : "Ok");

    /* frame rate */
    sprintf(cmd, "v4l2-ctl --set-parm=%u", fps);
    rc = system(cmd);
    printf("  Set frame rate: %s\n", rc ? "Not ok" : "Ok");

    set_camera_ctl("power_line_frequency", 1);
    set_camera_ctl("zoom_absolute", 100);
}

/* satart camera server process */
static void start_camera(const char *client, const char *port)
{
    char cmd[250];
    int rc;
    
    /* create command that will detach from current PTS */
    sprintf(cmd, "%s %s %s > /dev/null 2>&1 < /dev/null &",
            VIDEO_SERVER_CMD, client, port);

    rc = system(cmd);
    printf("Start video streaming to %s:%s : %s\n", client, port,
           rc ? "Not ok" : "Ok");

}

/* stop camera */
static void stop_camera()
{
    /* This should kill everything */
    int rc = system("pkill capture");
    printf("Stop camera: %s\n", rc ? "Not ok" : "Ok");
}

/* Set camera parameter (see v4l2-ctl -l) */
static void set_camera_ctl(const char *ctl, int val)
{
    int rc;
    char cmd[100];

    sprintf(cmd, "v4l2-ctl --set-ctrl=%s=%d", ctl, val);
    rc = system(cmd);
    printf("  Set camera ctrl (%s=%d): %s\n",
           ctl, val,rc ? "Not ok" : "Ok");
}

/* Get camera parameter (see v4l2-ctl -l) */
static void get_camera_ctl(const char *ctl)
{
    int rc;
    char cmd[100];

    sprintf(cmd, "v4l2-ctl --get-ctrl=%s", ctl);
    rc = system(cmd);
    if (rc)
        printf("Get camera ctrl (%s): Not ok\n", ctl);
}

/* Given a character c (cli command) find its corresponding
   index in ctltab */
static int chr2idx(char c)
{
    int idx = -1;
    int i = 0;

    while (ctltab[i].cli != 0)
    {
        if (ctltab[i].cli == c)
        {
            idx = i;
            break;
        }
        i++;
    }

    return idx;
}

/* Show help text. */
static void show_help()
{
    const char *help_msg = {
        "Bonecam controller built on " __DATE__ "  " __TIME__ "\n"
        "\n"
        "Usage: cam command <param1> <param2> ...\n"
        "\n"
        "  cam init  [w] [h] [fps]  Initialize camera\n"
        "  cam start [ip] [port]    Start streaming\n"
        "  cam stop                 Stop streaming\n"
        "  cam status               Show camera status\n"
        "\n"
        "  cam p  <azi> <ele>       Set azi and ele angles\n"
        "  cam pa <azi>             Set azi angle\n"
        "  cam pe <ele>             Set ele angle\n"
        "\n"
        "  cam r  <spa> <spe>       Set azi and ele rates\n"
        "  cam ra <sp>              Set azi rotation rate\n"
        "  cam re <sp>              Set ele rotation rate\n"
        "\n"
        "  cam m  <azi> <ele> <sp>  Move to pos. (azi,ele) at speed sp\n"
        "  cam ma <azi> <sp>        Move to azi at speed sp\n"
        "  cam me <ele> <sp>        Move to ele at speed sp\n"
        "\n"
        "Get/set image options. Use 'r' to reset (e.g. br, cr, ...):\n"
        "  cam b [val]              Brightness 0..255 (128)\n"
        "  cam c [val]              Contrast   0..255 (128)\n"
        "  cam s [val]              Saturation 0..255 (128)\n"
        "  cam h [val]              Sharpness  0..255 (128)\n"
        "  cam o [0|1]              Backlight compensation {0,1} (0)\n"
        "  cam l [val]              Power line freq. 0..2 (1 ~ 50Hz)\n"
        "  cam g [val]              Gain       0..255 (0)\n"
        "  cam x [0|1]              Exposure auto priority {0,1} (1)\n"
        "\n"
        "Following commands can be set to 'auto' using val=-1:\n"
        "  cam w [val]              White balance 2000..6500 (4000)\n"
        "  cam f [val]              Focus 0..250 (0)\n"
        "  cam e [val]              Exposure 3..2047 (250)\n"
        "\n"
        "Digital zoom, pan and tilt:\n"
        "  cam z  [val]             Zoom 100 ... 500 (100)\n"
        "  cam zp [val]             Pan  -10 ... +10  (0)\n"
        "  cam zt [val]             Tilt -10 ... +10  (0)\n"
        "  cam zr                   Reset digital zoom, pan and tilt\n"
        "\n"
        "Misc video options:\n"
        "  cam v                    Video options\n"
        "  cam vs <width> <height>  Set video frame size\n"
        "  cam vr <fps>             Set video frame rate for fps.\n"
    };

    printf("%s\n", help_msg);
}


