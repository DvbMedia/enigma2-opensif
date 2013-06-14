#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/stat.h>
#include <pthread.h>

#include <lib/base/eerror.h>
#include <lib/driver/vfd.h>

#define VFDLENGTH 16
#define VFD_DEVICE "/dev/vfd"
#define VFDICONDISPLAYONOFF   0xc0425a0a
#define VFDDISPLAYCHARS       0xc0425a00
#define VFDBRIGHTNESS         0xc0425a03
#define VFDSETLED             0xc0425afe
#define VFDDISPLAYWRITEONOFF  0xc0425a05 //light on off
#define VFDSETTIME2           0xc0425afd

struct set_mode_s {
	int compat; /* 0 = compatibility mode to vfd driver; 1 = nuvoton mode */
};

struct set_brightness_s {
	int level;
};

struct set_icon_s {
	int icon_nr;
	int on;
};

struct set_led_s {
	int led_nr;
	int on;
};

/* time must be given as follows:
 * time[0] & time[1] = mjd ???
 * time[2] = hour
 * time[3] = min
 * time[4] = sec
 */
struct set_standby_s {
	char time[5];
};

struct set_time_s {
	char time[5];
};

struct aotom_ioctl_data {
	union
	{
		struct set_icon_s icon;
		struct set_led_s led;
		struct set_brightness_s brightness;
		struct set_mode_s mode;
		struct set_standby_s standby;
		struct set_time_s time;
	} u;
};

struct vfd_ioctl_data
{
	unsigned char start;
	unsigned char data[64];
	unsigned char length;
};

evfd* evfd::instance = NULL;

evfd* evfd::getInstance()
{
	if (instance == NULL)
		instance = new evfd;
	return instance;
}

evfd::evfd()
{
	file_vfd = 0;
}

evfd::~evfd()
{
	//close (file_vfd);
}

void evfd::init()
{
}

void evfd::vfd_write_string(char * str)
{
	int i;
	i = strlen ( str );
	if ( i > 63 ) i = 63;
	struct vfd_ioctl_data data;
	memset ( data.data, ' ', 63 );
	memcpy ( data.data, str, i );
	data.start = 0;
	data.length = i;
	file_vfd = open (VFD_DEVICE, O_WRONLY);
	ioctl ( file_vfd, VFDDISPLAYCHARS, &data );
	close (file_vfd);
	return;
}

void evfd::vfd_clear_string()
{
	vfd_write_string("                ");
	return;
}

void evfd::vfd_set_icon(tvfd_icon id, bool onoff)
{
	struct vfd_ioctl_data data;
	memset(&data, 0, sizeof(struct vfd_ioctl_data));
	data.start = 0x00;
	data.data[0] = id;
	data.data[4] = onoff;
	data.length = 5;
	file_vfd = open (VFD_DEVICE, O_WRONLY);
	ioctl(file_vfd, VFDICONDISPLAYONOFF, &data);
	close (file_vfd);
	return;
}

void evfd::vfd_clear_icons()
{
	for (int id = 0x10; id < 0x20; id++)
	{
		vfd_set_icon((tvfd_icon)id, false);
	}
	return;
}

void evfd::vfd_set_brightness(unsigned char setting)
{
	struct vfd_ioctl_data data;
	memset(&data, 0, sizeof(struct vfd_ioctl_data));
	data.start = setting & 0x07;
	data.length = 0;
	file_vfd = open (VFD_DEVICE, O_WRONLY);
	ioctl ( file_vfd, VFDBRIGHTNESS, &data );
	close (file_vfd);
	return;
}

void evfd::vfd_set_led(long onoff)
{
	int status;
	struct aotom_ioctl_data vData;
	vData.u.led.led_nr = 0;
	if (onoff)
		vData.u.led.on = 1;
	else
		vData.u.led.on = 0;

	memset(&vData, 0, sizeof(struct aotom_ioctl_data));
	
	eDebug("[evfd] Set led: which = %d, onoff = %d",vData.u.led.led_nr,vData.u.led.on);

 	file_vfd = open (VFD_DEVICE, O_WRONLY);
	status=ioctl(file_vfd, VFDSETLED, &vData);
	
	eDebug("[evfd] Set led: status = %d",status);

	close (file_vfd);
	return;
}

void evfd::vfd_set_clock(time_t t)
{
	struct tm *tmp;
	tmp = localtime(&t);
	t += tmp->tm_gmtoff;
	file_vfd = open (VFD_DEVICE, O_WRONLY);
	ioctl(file_vfd, VFDSETTIME2, &t);
	close (file_vfd);
	return;
}

