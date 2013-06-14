#ifndef VFD_H_
#define VFD_H_

typedef enum { VFD_USB = 0x10, VFD_STANDBY, VFD_SAT, VFD_REC, VFD_TIMESHIFT, VFD_TIMER, VFD_HD, VFD_LOCK, VFD_DD, VFD_MUTE, VFD_TUNER1, VFD_TUNER2, VFD_MP3, VFD_REPEAT, VFD_PLAY, VFD_PAUSE, VFD_TER, VFD_FILE, VFD_480i, VFD_480p, VFD_576i, VFD_576p, VFD_720p, VFD_1080i, VFD_1080p } tvfd_icon;

class evfd
{
protected:
	static evfd *instance;
	int file_vfd;
#ifdef SWIG
	evfd();
	~evfd();
#endif
public:
#ifndef SWIG
	evfd();
	~evfd();
#endif
	void init();
	static evfd* getInstance();

	void vfd_set_icon(tvfd_icon id, bool onoff);
	void vfd_clear_icons();

	void vfd_write_string(char * string);
	void vfd_clear_string();
	
	void vfd_set_brightness(unsigned char setting);
	void vfd_set_led(bool onoff);

	void vfd_set_clock(time_t t);

};

#endif
