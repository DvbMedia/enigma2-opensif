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
#define VFDDISPLAYWRITEONOFF  0xc0425a05 //light on off
#define VFDSETTIME2           0xc0425afd

static pthread_t thread_start_loop = 0;
void * start_loop (void *arg);

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

void evfd::init()
{
	pthread_create (&thread_start_loop, NULL, &start_loop, NULL);
	return;
}

evfd::~evfd()
{
	//close (file_vfd);
}

void * start_loop (void *arg)
{
	evfd vfd;
	//vfd.vfd_clear_icons();
	vfd.vfd_write_string("Opensif Spark");
	//run 2 times through all icons 
	for (int vloop = 0; vloop < 128; vloop++)
	{
		if (vloop%14 == 0 )
			vfd.vfd_set_brightness(1);
		else if (vloop%14 == 1 )
			vfd.vfd_set_brightness(2);
		else if (vloop%14 == 2 )
			vfd.vfd_set_brightness(3);
		else if (vloop%14 == 3 )
			vfd.vfd_set_brightness(4);
		else if (vloop%14 == 4 )
			vfd.vfd_set_brightness(5);
		else if (vloop%14 == 5 )
			vfd.vfd_set_brightness(6);
		else if (vloop%14 == 6 )
			vfd.vfd_set_brightness(7);
		else if (vloop%14 == 7 )
			vfd.vfd_set_brightness(6);
		else if (vloop%14 == 8 )
			vfd.vfd_set_brightness(5);
		else if (vloop%14 == 9 )
			vfd.vfd_set_brightness(4);
		else if (vloop%14 == 10 )
			vfd.vfd_set_brightness(3);
		else if (vloop%14 == 11 )
			vfd.vfd_set_brightness(2);
		else if (vloop%14 == 12 )
			vfd.vfd_set_brightness(1);
		else if (vloop%14 == 13 )
			vfd.vfd_set_brightness(0);
		usleep(75000);
	}
	vfd.vfd_set_brightness(7);
	return NULL;
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

void evfd::vfd_set_light(bool onoff)
{
	struct vfd_ioctl_data data;
	memset(&data, 0, sizeof(struct vfd_ioctl_data));
	if (onoff)
		data.start = 0x01;
	else
		data.start = 0x00;
	data.length = 0;
	file_vfd = open (VFD_DEVICE, O_WRONLY);
	ioctl(file_vfd, VFDDISPLAYWRITEONOFF, &data);
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

