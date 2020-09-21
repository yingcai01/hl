#include "jlinkui.h"
#include "graphics.h"
#include <dirent.h>
#include <poll.h>

#define LEFTRODX 120
#define RODY 300
#define RIGHTRODX 1000

gr_surface big_surface;
gr_surface sma_surface;
static int big_width;
static int big_height;
static int half_big_width;
static int half_big_height;
static int small_width;
static int small_height;
static int half_small_width;
static int half_small_height;


static int org_lef_x;
static int org_lef_y;
static int dst_lef_x;
static int dst_lef_y;
static int org_rig_x;
static int org_rig_y;
static int dst_rig_x;
static int dst_rig_y;

static int left_clear_X1;
static int left_clear_Y1;
static int left_clear_X2;
static int left_clear_Y2;

static int right_clear_X1;
static int right_clear_Y1;
static int right_clear_X2;
static int right_clear_Y2;

static pthread_mutex_t flipMutex;
static pthread_mutex_t drawMutex;
static pthread_mutex_t draw1Mutex;
static pthread_mutex_t draw2Mutex;
static pthread_cond_t cond;
static pthread_cond_t cond1;
static pthread_cond_t cond2;
static int isneeddraw1,isneeddraw2;
static int oldx,oldy,oldz,oldrz;
static int x_,y_;
static int x,y,z,rz;
extern void ui_clear_key_queue();
extern int res_create_surface(const char* fname, gr_surface* pSurface);;
static void* StartRodEvenet(void*param);
static void* startKeyEvent(void*param);
static void* StartRodEvenet(void*param);
void* drawImageRod1(void* param);
void* drawImageRod2(void* param);
static void drawImageRod(int x,int y, int z, int rz);
static int gamekeyrun;
static int dirkeynum = 4;
typedef void (*Callbak)();
static int flag = 0;
static int resultFlag=0;
struct GameMap{
	Callbak call;
	int keycode;
	int isdraw;
};


void drawKeyUpLeft1(){
	jlinkdrawRectangle3(220,30,60,60,3*jpi/4);
	flag |= 0x1;
}

void drawKeyUpLeft2(){
	gr_fill( 240, 33, 320, 87);
	flag |= 0x2;
}

void drawKeyUpRight1(){
	gr_fill( 960, 33, 1040, 87);
	flag |= 0x4;
}

void drawKeyUpRight2(){
	jlinkdrawRectangle3(1060,30,60,60,jpi/4);
	flag |= 0x8;
}
/*
void drawKeyKeyLeft1(){
	//jlinkdrawRectangle(230,160,80,60,jpi/3,0);
	jlinkdrawRectangle(180,460,80,60,jpi/3,0);
	flag |= 0x10;
}

void drawKeyKeyLeft2(){
	//jlinkdrawRectangle(340,160,80,60,jpi/3,0);
	jlinkdrawRectangle(180,560,80,60,jpi/3,0);
	flag |= 0x20;
}
*/
void drawKeyKeyVolumeup(){
	//jlinkdrawRectangle(230,160,80,60,jpi/3,0);
	jlinkdrawRectangle(180,510,80,60,jpi/3,0);
	flag |= 0x10;
}

void drawKeyKeyVolumedown(){
	//jlinkdrawRectangle(340,160,80,60,jpi/3,0);
	jlinkdrawRectangle(180,600,80,60,jpi/3,0);
	flag |= 0x20;
}
void drawKeyKeyG(){
	jlinkdrawRectangle(1000,560,80,60,jpi/3,0);
	flag |= 0x40;
}
/*
void drawKeyKeyLeft3(){
	jlinkdrawRectangle(450,160,80,60,jpi/3,0);
	flag |= 0x40;
}

void drawKeyKeyCenter(){
	jlinkdrawRectangle(560,160,160,60,jpi/3,1);
	flag |= 0x80;
}
*/
void drawKeyKeyPower(){
	jlinkdrawRectangle(1000,640,80,60,jpi/3,1);
	flag |= 0x80;
}

void drawKeyKeyRight1(){
	jlinkdrawRectangle(750,160,80,60,2*jpi/3,0);
	flag |= 0x100;
}

void drawKeyKeyRight2(){
	jlinkdrawRectangle(860,160,80,60,2*jpi/3,0);
	flag |= 0x200;
}

void drawKeyKeyRight3(){
	jlinkdrawRectangle(970,160,80,60,2*jpi/3,0);
	flag |= 0x400;
}

/*
void drawKeyKeyCenter1(){
	gr_fill( 420, 360, 500, 420);
	flag |= 0x800;
}

void drawKeyKeyCenter2(){
	jlinkdrawRectangle2(840,360,70,70,jpi*3/8);
	flag |= 0x1000;
}
*/

void drawKeyKeyST(){
	//gr_fill( 1000, 470, 500, 420);
	gr_fill( 1000, 470, 1060, 530);
	flag |= 0x800;
}

void drawKeyKeySE(){
	jlinkdrawRectangle2(1000,380,70,70,jpi*3/8);
	flag |= 0x1000;
}

void drawDirctKey(int key){
	if (key == 0){
		//gr_fill( 200, 560, 260, 620);//left
		//gr_fill( 260, 560, 320, 620);//center
		//gr_fill( 320, 560, 380, 620);//right
		//gr_fill( 260, 500, 320, 560);//up
		//gr_fill( 260, 620, 320, 680);//down
		
		gr_fill( 160, 380, 220, 440);//left
		gr_fill( 220, 380, 280, 440);//center
		gr_fill( 280, 380, 340, 440);//right
		gr_fill( 220, 320, 280, 380);//up
		gr_fill( 220, 440, 280, 500);//down

		flag |= 0x2000;
		flag |= 0x4000;
		flag |= 0x8000;
		flag |= 0x10000;
		dirkeynum=0;
	}else if(key==105){
		//gr_fill( 200, 560, 260, 620);//left
		gr_fill( 160, 380, 220, 440);//left
		flag |= 0x2000;
		dirkeynum++;
	}else if (key==106){
		//gr_fill( 320, 560, 380, 620);//right
		gr_fill( 280, 380, 340, 440);//right
		dirkeynum++;
		flag |= 0x4000;
	}else if (key==103){
		//gr_fill( 260, 500, 320, 560);//up
		gr_fill( 220, 320, 280, 380);//up
		dirkeynum++;
		flag |= 0x8000;
	}else if (key==108){
		//gr_fill( 260, 620, 320, 680);//down
		gr_fill( 220, 440, 280, 500);//down

		dirkeynum++;
		flag |= 0x10000;
	}

	if (dirkeynum==4){
		//gr_fill( 260, 560, 320, 620);//center
		gr_fill( 220, 380, 280, 440);//center
	}
}

//X
void drawXkey(){
	//jlinkdrawRealCircle(910,600,40);
	jlinkdrawRealCircle(950,230,40);
	flag |= 0x20000;
}

//Y
void drawYkey(){
	//jlinkdrawRealCircle(990,530,40);
jlinkdrawRealCircle(1030,160,40);
	flag |= 0x40000;
}

//A
void drawAkey(){
	//jlinkdrawRealCircle(990,670,40);
jlinkdrawRealCircle(1030,300,40);
	flag |= 0x80000;
}

//B
void drawBkey(){
	//jlinkdrawRealCircle(1070,600,40);
jlinkdrawRealCircle(1110,230,40);
	flag |= 0x100000;
}



struct GameMap gameMap[] = {
	{drawKeyUpLeft1,310},//
	{drawKeyUpLeft2,312},//
	{drawKeyUpRight1,313},//
	{drawKeyUpRight2,311},//

	//{drawKeyKeyLeft1,115},
	//{drawKeyKeyLeft2,114},
	{drawKeyKeyVolumeup,115},
	{drawKeyKeyVolumedown,114},
	{drawKeyKeyG,88},
	//{drawKeyKeyLeft3,88},
	//{drawKeyKeyCenter,116},
	{drawKeyKeyPower,116},
	//{drawKeyKeyRight1,580},
	//{drawKeyKeyRight2,102},
	//{drawKeyKeyRight3,158},
	{drawXkey,307},
	{drawYkey,308},
	{drawAkey,304},
	{drawBkey,305},
	//{drawKeyKeyCenter1,314},
	//{drawKeyKeyCenter2,315}
	{drawKeyKeyST,314},
	{drawKeyKeySE,315},
};


static pthread_t key_handle;
static pthread_t image_handle;
static pthread_t drawleft_handle;
static pthread_t drawright_handle;
void jlink_start_gamekey(){
	int i;
	oldx = -256,oldy=-256,oldz=-256,oldrz=-256;
	int ret = res_create_surface("/vendor/res/images/big.png",&big_surface);
	if (ret != 0){
		LOGD("MYTEST drawImageRod error0 ret=%d,errno=%d",ret,errno);
		return;
	}
	big_width = gr_get_width(big_surface);
	big_height = gr_get_height(big_surface);
	LOGE("MYTEST big_width=%d,big_height=%d",big_width,big_height);
	ret = res_create_surface("/vendor/res/images/small.png",&sma_surface);
	if (ret != 0){
		LOGD("MYTEST drawImageRod error1");
		return;
	}
	small_width = gr_get_width(sma_surface);
	small_height = gr_get_height(sma_surface);

	left_clear_X1 = LEFTRODX-small_width/2;
	left_clear_Y1 = RODY - small_height/2;
	left_clear_X2 = LEFTRODX+small_width/2+big_width;
	left_clear_Y2 = RODY + small_height/2 + big_height;

	right_clear_X1 = RIGHTRODX-small_width/2;
	right_clear_Y1 = left_clear_Y1;
	right_clear_X2 = RIGHTRODX+small_width/2+big_width;
	right_clear_Y2 = left_clear_Y2;
	org_lef_x = LEFTRODX+(big_width-small_width)/2;
	org_lef_y = RODY+(big_height-small_height)/2;
	org_rig_x = RIGHTRODX+(big_width-small_width)/2;
	org_rig_y = org_lef_y;

	half_big_width = big_width/2;
	half_big_height = big_height/2;

    //remove rod
	//drawImageRod(0,0,0,0);

	
	gamekeyrun = 1;
	set_gr_color(0xcccccc55);
    resultFlag=0;
	int keymapnum = sizeof(gameMap)/sizeof(gameMap[0]);
	for (i=0;i<keymapnum;i++){
		(gameMap[i].call)();
	}
	drawDirctKey(0);
	#ifdef JLINK_ITEM_DEBUG
	gr_flip();
	#endif
	resultFlag=flag;
	flag = 0;
	isneeddraw1 = 0;
	isneeddraw2 = 0;
	pthread_mutex_init(&flipMutex,NULL);
	pthread_mutex_init(&drawMutex,NULL);
	pthread_mutex_init(&draw1Mutex,NULL);
	pthread_mutex_init(&draw2Mutex,NULL);
	pthread_cond_init(&cond,NULL);
	pthread_cond_init(&cond1,NULL);
	pthread_cond_init(&cond2,NULL);
	pthread_create(&key_handle,NULL,startKeyEvent,NULL);
	//remove Rod Test
	//pthread_create(&image_handle,NULL,StartRodEvenet,NULL);
	//pthread_create(&drawleft_handle,NULL,drawImageRod1,NULL);
	//pthread_create(&drawright_handle,NULL,drawImageRod2,NULL);
	pthread_join(key_handle,NULL);
	//pthread_join(image_handle,NULL);
	//pthread_join(drawleft_handle,NULL);
	//pthread_join(drawright_handle,NULL);
	pthread_cond_destroy(&cond);
	pthread_cond_destroy(&cond1);
	pthread_cond_destroy(&cond2);
	
}



void* startKeyEvent(void*param){
	int key;
	int keymapnum = sizeof(gameMap)/sizeof(gameMap[0]);
	int i;
	struct timespec ntime;
	ntime.tv_sec= time(NULL)+20;
	ntime.tv_nsec=0;
	ui_clear_key_queue();
	while(gamekeyrun){
		key=jlink_wait_key(&ntime);
		if (key!=110){
			LOGD("MYTEST keycode=%d",key);
			switch (key){
				case 103:
				case 105:
				case 106:
				case 108:
					pthread_mutex_lock(&flipMutex);
					set_gr_color(0x11cc1177);
					drawDirctKey(key);
					#ifdef JLINK_ITEM_DEBUG
					gr_flip();
					#endif
					pthread_mutex_unlock(&flipMutex);
					break;
				default:
					for (i=0;i<keymapnum;i++){
						if (key == gameMap[i].keycode){
							pthread_mutex_lock(&flipMutex);
							set_gr_color(0x11cc1177);
							(gameMap[i].call)();
							#ifdef JLINK_ITEM_DEBUG
							gr_flip();
							#endif
							pthread_mutex_unlock(&flipMutex);
							break;
						}
					}
			}
			if (flag == resultFlag){
				gamekeyrun = 0;
				pthread_cond_signal(&cond);
			}
		}

	}
	return NULL;
}

void* drawImageRod1(void* param){
	while(gamekeyrun){
		pthread_cond_wait(&cond1,&draw1Mutex);
        pthread_mutex_lock(&flipMutex);
		set_gr_color(BACKCOLOR);
		gr_fill(left_clear_X1,left_clear_Y1,left_clear_X2,left_clear_Y2);
		gr_blit((GRSurface*)big_surface, 0, 0, big_width, big_height, LEFTRODX, RODY);
		gr_blit((GRSurface*)sma_surface, 0, 0, small_width, small_height, x_, y_);
		#ifdef JLINK_ITEM_DEBUG
		gr_flip();
		#endif
		pthread_mutex_unlock(&flipMutex);
		pthread_cond_signal(&cond);
	}
	return NULL;
}

void* drawImageRod2(void* parma){
	while(gamekeyrun){
        pthread_cond_wait(&cond2,&draw2Mutex);
        pthread_mutex_lock(&flipMutex);
		set_gr_color(BACKCOLOR);
		gr_fill(right_clear_X1,right_clear_Y1,right_clear_X2,right_clear_Y2);
		gr_blit((GRSurface*)big_surface, 0, 0, big_width, big_height, RIGHTRODX, RODY);
		gr_blit((GRSurface*)sma_surface, 0, 0, small_width, small_height, x_, y_);
		#ifdef JLINK_ITEM_DEBUG
		gr_flip();
		#endif
		pthread_mutex_unlock(&flipMutex);
		pthread_cond_signal(&cond);
	}
	return NULL;
}

void drawImageRod(int x,int y, int z, int rz){
	if (x != oldx && y != oldy){
		oldx = x;
		oldy = y;
		//set_gr_color(BACKCOLOR);
		gr_fill(left_clear_X1,left_clear_Y1,left_clear_X2,left_clear_Y2);
		dst_lef_x = (int)(org_lef_x + (float)x/255*half_big_width);
		dst_lef_y = (int)(org_lef_y + (float)y/255*half_big_height);
		gr_blit((GRSurface*)big_surface, 0, 0, big_width, big_height, LEFTRODX, RODY);
		gr_blit((GRSurface*)sma_surface, 0, 0, small_width, small_height, dst_lef_x, dst_lef_y);
	}
	
	if (z != oldz && rz != oldrz){
		oldz = z;
		oldrz = rz;
		//set_gr_color(BACKCOLOR);
		gr_fill(right_clear_X1,right_clear_Y1,right_clear_X2,right_clear_Y2);
		dst_rig_x = (int)(org_rig_x + (float)z/255*half_big_width);
		dst_rig_y = (int)(org_rig_y + (float)rz/255*half_big_height);
		gr_blit((GRSurface*)big_surface, 0, 0, big_width, big_height, RIGHTRODX, RODY);
		gr_blit((GRSurface*)sma_surface, 0, 0, small_width, small_height, dst_rig_x, dst_rig_y);
	}
	LOGD("MYTEST startDraw");
	#ifdef JLINK_ITEM_DEBUG
	gr_flip();
	#endif
	
}

static struct pollfd pfd;
static void* StartRodEvenet(void*param){
	DIR *dir;
	struct dirent *de;
	char name[100];
	dir = opendir("/dev/input");
	int fd;
	if (dir != NULL)
	{
		while(de = readdir(dir)){
			if(strncmp(de->d_name,"event",5)) continue;
			fd = openat(dirfd(dir), de->d_name, O_RDONLY);
            if(fd < 0) continue;

            ioctl(fd, EVIOCGNAME(sizeof(name) - 1), name);
            LOGD("MYTEST0,StartRodEvenet name=%s",name);
            if (!strcmp(name, "jlink_joy playgame")) {
            	pfd.fd = fd;
            	pfd.events = POLLIN;
            	break;
            }else{
            	close(fd);
            	fd = -1;
            }
		}
		closedir(dir);
	}

	if (fd == -1){
		LOGE("MYTEST StartRodEvenet open event error");
		return NULL;
	}
	int ret;
	struct input_event ev;
	x=0,y=0,z=0,rz=0;
	do{
		ret = poll(&pfd,1,0);
		if (ret >0){
			if(pfd.revents & POLLIN){
				ret = read(pfd.fd, &ev, sizeof(ev));
				if(ret == sizeof(ev)) {
					//LOGD("MYTEST ev.type=%d,ev.code=%d,ev->value=%d",ev.type,ev.code,ev.value);
					
					if (ev.type == EV_ABS){
						if (ev.code == ABS_X){
							x = ev.value;
						}else if (ev.code == ABS_Y){
							y = ev.value;
						}else if (ev.code == ABS_Z){
							z = ev.value;
						}else if (ev.code == ABS_RZ){
							rz = ev.value;
						}
				}else if (ev.type==EV_SYN){
						if (x!=oldx || y != oldy){
							pthread_mutex_lock(&draw1Mutex);
							oldx = x;
							oldy = y;
							x_ = (int)(org_lef_x + (float)x/255*half_big_width);
							y_ = (int)(org_lef_y + (float)y/255*half_big_height);
							pthread_cond_signal(&cond1);
        					pthread_mutex_unlock(&draw1Mutex);
        					pthread_cond_wait(&cond,&drawMutex);
						}else if(z!=oldz || rz != oldrz ){
							pthread_mutex_lock(&draw2Mutex);
							oldz = z;
							oldrz = rz;
							x_ = (int)(org_rig_x + (float)z/255*half_big_width);
							y_ = (int)(org_rig_y + (float)rz/255*half_big_height);
							pthread_cond_signal(&cond2);
	        				pthread_mutex_unlock(&draw2Mutex);
	        				pthread_cond_wait(&cond,&drawMutex);
	        				
						}
						
						//drawImageRod(x,y,z,rz);
						//LOGD("MYTEST x:%d,y:%d oldx:%d,oldy:%d z:%d,rz:%d",
						//	x,y,oldx,oldy,z,rz);
					}	
				}
			}
		}
	}while(gamekeyrun);
	pthread_cond_signal(&cond1);
	pthread_cond_signal(&cond2);
	close(fd);
	return NULL;
}
