/********************************************************************
* $ID: led.c         Wed 2012-10-17 11:07:06 +0800  dannywang       *
*                                                                   *
* Description:                                                      *
*                                                                   *
* Maintainer:  (danny.wang)  <dannywang@smartvideo.com>             *
*                                                                   *
* CopyRight (c) 2012 SMARTVIDEO                                      *
*   All rights reserved.                                            *
*                                                                   *
* This file is free software;                                       *
*   you are free to modify and/or redistribute it                   *
*   under the terms of the GNU General Public Licence (GPL).        *
*                                                                   *
* Last modified:                                                    *
*                                                                   *
* No warranty, no liability, use this at your own risk!             *
********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef CONFIG_LED_DEMO
static char ledd_cmdbuf[256];
static int  ledd_cmd_no = 0;
static int   ledd_fd = -1;
enum {LED_RED=0, LED_GREEN};

int ledd_open(void)
{
    if((ledd_fd = open("/tmp/ledd_rd_fifo", O_RDWR)) < 0)
	return -1;

    return 0;
}

// R_G: 0 red, 1:green
int led_command(int R_G, int power, int brightness, int delay_on, int delay_off)
{
    int len;
    static char RGstr[][8] = {"red", "greed"};

    if(ledd_fd < 0 && ledd_open() < 0) return -1;

    R_G = (R_G > 0) ? 1 : 0;
    len = sprintf(ledd_cmdbuf, "0 %s:ledd %d ledctrl type=%s power=%d brightness=%d delay_on=%d delay_off=%d\n",
	    "mcsgdemo", ledd_cmd_no++, RGstr[R_G], power, brightness, delay_on, delay_off);
    write(ledd_fd, ledd_cmdbuf, len);

    return 0;
}

int led_red_on(int mypower)
{
    return led_command(LED_RED, mypower, 1, -1, -1);
}

int led_red_off(int mypower)
{
    return led_command(LED_RED, mypower, 0, -1, -1);
}

int led_red_ctrl(int mypower, int brightness, int delay_on, int delay_off)
{
    return led_command(LED_RED, mypower, brightness, delay_on, delay_off);
}

int led_green_on(int mypower)
{
    return led_command(LED_GREEN, mypower, 1, -1, -1);
}

int led_green_off(int mypower)
{
    return led_command(LED_GREEN, mypower, 0, -1, -1);
}

int led_green_ctrl(int mypower, int brightness, int delay_on, int delay_off)
{
    return led_command(LED_GREEN, mypower, brightness, delay_on, delay_off);
}



//#else
enum {LED_No_RED=0, LED_No_GREEN, LED_No_LAST};

static int led_write(const char *devname, const char *buf, int len)
{
    int fd;

    if((fd = open(devname, O_RDWR)) < 0) {
	fprintf(stderr, "Error open:%s\n", devname);
	return -1;
    }

    write(fd, buf, len);
    close(fd);

    return 0;
}

static int led_read(const char *devname)
{
    int fd, power = 255;
    char buf[8];

    if((fd = open(devname, O_RDWR)) < 0) {
	fprintf(stderr, "Error open:%s\n", devname);
	return -1;
    }

    memset(buf, 0, sizeof(buf));
    if(read(fd, buf, 8) > 0) power = atoi(buf);
    close(fd);

    return power;
}

static int led_ctrl(int number, int mypower, int brightness, int delay_on, int delay_off)
{
    int len, power;
    char devname[64], buf[64];
    char *dirled = "/sys/class/leds/Red";
    if(LED_No_RED != number)
	dirled = "/sys/class/leds/Green";

    sprintf(devname, "%s/max_brightness", dirled);
    power = led_read(devname);
    if(mypower > power) return -1;
    else if(mypower < power) {
	len = sprintf(buf, "%d", mypower);
	led_write(devname, buf, len);
    }

    sprintf(devname, "%s/brightness", dirled);
    len = sprintf(buf, "%d", brightness);
    led_write(devname, buf, len);

    if(delay_on > 0 && delay_off > 0) {
	len = sprintf(buf, "%d", delay_on);
	sprintf(devname, "%s/delay_on", dirled);
	led_write(devname, buf, len);

	len = sprintf(buf, "%d", delay_off);
	sprintf(devname, "%s/delay_off", dirled);
	led_write(devname, buf, len);

	len = sprintf(buf, "%d", 1);
	sprintf(devname, "%s/blink_enable", dirled);
	led_write(devname, buf, len);
    }
    else {
	len = sprintf(buf, "%d", 0);
	sprintf(devname, "%s/blink_enable", dirled);
	led_write(devname, buf, len);
    }

    return 0;
}

int led_red_ctrl(int mypower, int brightness, int delay_on, int delay_off)
{
    return led_ctrl(LED_No_RED, mypower, brightness, delay_on, delay_off);
}

int led_red_on(int mypower)
{
    return led_ctrl(LED_No_RED, mypower, 1, -1, -1);
}

int led_red_off(int mypower)
{
    return led_ctrl(LED_No_RED, mypower, 0, -1, -1);
}

int led_green_ctrl(int mypower, int brightness, int delay_on, int delay_off)
{
    return led_ctrl(LED_No_GREEN, mypower, brightness, delay_on, delay_off);
}

int led_green_on(int mypower)
{
    return led_ctrl(LED_No_GREEN, mypower, 1, -1, -1);
}

int led_green_off(int mypower)
{
    return led_ctrl(LED_No_GREEN, mypower, 0, -1, -1);
}

// type:CHECK_T_XXX
int led_show_file(int type)
{
    switch(type) {
    case CHECK_T_READ:
    case CHECK_T_WRITE:
	led_red_ctrl(LED_POWER_NORMAL, 1, FWRITE_BLINK_ON, FWRITE_BLINK_OFF);
	led_green_off(LED_POWER_NORMAL);
	break;
    default:
	led_red_off(LED_POWER_NORMAL);
	led_green_on(LED_POWER_NORMAL);
	break;
    }

    return 0;
}
#endif

/********************** End Of File: led.c **********************/
