/* Simple bonecam control interface. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


/* defaults */
#define DEFAULT_TTY       "/dev/ttyO1"
#define DEFAULT_CLIENT    "romit"
#define DEFAULT_PORT      "4000"
#define DEFAULT_WIDTH     640
#define DEFAULT_HEIGHT    480
#define DEFAULT_FPS       24

/* servo numbers */
#define AZI 0
#define ELE 1

#define SERVER_STR "/home/root/bonecam.git/scripts/server"


static void init_servos();
static void init_camera(unsigned int w, unsigned int h, unsigned int fps);
static void start_camera(const char *client, const char *port);
static void set_speed(int servo, int speed);
static void set_angle(int servo, int angle);
static void write_tty(const char *tty, const char *data, int len);
static void show_help();


int main(int argc, char *argv[])
{
    int rc=0;

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
        printf("cam start not implemented\n");
        goto done;
    }
    else if (!strcmp(argv[1], "stop"))
    {
        /* This should kill everything */
        rc = system("pkill capture");
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

    case 's':
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

    sprintf(cmd,"v4l2-ctl --set-fmt-video=width=%u,height=%u,pixelformat=1",
            w, h);
    rc = system(cmd);
    printf("  Set frame size: %s\n", rc ? "Ok" : "Not ok");

    sprintf(cmd, "--set-parm=%u", fps);
    rc = system(cmd);
    printf("  Set frame rate: %s\n", rc ? "Ok" : "Not ok");
}

/* satart camera by forking the server process */
static void start_camera(const char *client, const char *port)
{
    pid_t pid = vfork();

    if (pid < 0)
    {
        /* failed */
        printf("Failed to fork camera server\n");
    }
    else if (pid == 0)
    {
        /* child */
        execl(SERVER_STR, "camctl", client, port, NULL);
    }
    else
    {
        /* parent */
        printf("Camera server started. Streaming to %s:%s\n",
                client, port);
    }
}


/* Show help text. */
static void show_help()
{
    const char *help_msg = {
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
        "  cam s  <spa> <spe>       Set azi and ele rotation speeds\n"
        "  cam sa <sp>              Set azi rotation speed\n"
        "  cam se <sp>              Set ele rotation speed\n"
        "\n"
        "  cam m  <azi> <ele> <sp>  Move to pos. (azi,ele) at speed sp\n"
        "  cam ma <azi> <sp>        Move to azi at speed sp\n"
        "  cam me <ele> <sp>        Move to ele at speed sp\n"
    };

    printf("%s\n", help_msg);
}


