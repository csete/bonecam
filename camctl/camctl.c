/* Simple bonecam control interface. */
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_TTY       "/dev/ttyO1"
#define DEFAULT_CLIENT    "romit"
#define DEFAULT_PORT      "4000"

/* servo numbers */
#define AZI 0
#define ELE 1


static void init_servos();
static void set_speed(int servo, int speed);
static void set_angle(int servo, int angle);
static void write_tty(const char *tty, const char *data, int len);
static void show_help();


int main(int argc, char *argv[])
{

    if (argc == 1)
    {
        show_help();
        return 1;
    }

    init_servos();

    return 0;
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

    //fputs(data, fp);
    fwrite(data, 1, len, fp);

    fclose(fp);
}


/* Show help text. */
static void show_help()
{
    const char *help_msg = {
        "\n"
        "Usage: cam command <param1> <param2> ...\n"
        "\n"
        "  cam init\n"
        "  cam start\n"
        "  cam stop\n"
        "\n"
        "  cam p  <int> <int>    Set azi and ele angles\n"
        "  cam pa <int>          Set azi angle ±90 deg\n"
        "  cam pe <int>          Set ele angle\n"
        "\n"
        "  cam s  <int> <int>    Set azi and ele rotation speeds\n"
        "  cam sa <int>          Set azi rotation speed\n"
        "  cam se <int>          Set ele rotation speed\n"
    };

    printf("%s\n", help_msg);
}


