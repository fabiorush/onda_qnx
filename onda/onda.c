#include <stdlib.h>
#include <stdio.h>
#include <sys/neutrino.h>
#include <errno.h>
#include <stdint.h>
#include <sys/mman.h>
#include <hw/inout.h>
#include <time.h>
#include <sys/netmgr.h>
#include <unistd.h>
#include <sys/siginfo.h>
#include <arm/omap3530.h>

//#include <Pt.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>

#define MY_PULSE_CODE   _PULSE_CODE_MINAVAIL

#define	OMAP2420_GPIO_REVISION		0x00
#define	OMAP2420_GPIO_SYSCONFIG		0x10
#define	OMAP2420_GPIO_SYSSTATUS		0x14
#define	OMAP2420_GPIO_IRQSTATUS1	0x18
#define	OMAP2420_GPIO_IRQENABLE1	0x1C
#define	OMAP2420_GPIO_WAKEUPENABLE	0x20
#define	OMAP2420_GPIO_IRQSTATUS2	0x28
#define	OMAP2420_GPIO_IRQENABLE2	0x2C
#define	OMAP2420_GPIO_CTRL			0x30
#define	OMAP2420_GPIO_OE			0x34
#define	OMAP2420_GPIO_DATAIN		0x38
#define	OMAP2420_GPIO_DATAOUT		0x3C
#define	OMAP2420_GPIO_LEVELDETECT0	0x40
#define	OMAP2420_GPIO_LEVELDETECT1	0x44
#define	OMAP2420_GPIO_RISINGDETECT	0x48
#define	OMAP2420_GPIO_FALLINGDETECT	0x4C
#define	OMAP2420_GPIO_DEBOUNCENABLE	0x50
#define	OMAP2420_GPIO_DEBOUNCINGTIME	0x54
#define	OMAP2420_GPIO_CLEARIRQENABLE1	0x60
#define	OMAP2420_GPIO_SETIRQENABLE1	0x64
#define	OMAP2420_GPIO_CLEARIRQENABLE2	0x70
#define	OMAP2420_GPIO_SETIRQENABLE2	0x74
#define	OMAP2420_GPIO_CLEARWKUENA	0x80
#define	OMAP2420_GPIO_SETWKUENA		0x84
#define	OMAP2420_GPIO_CLEARDATAOUT	0x90
#define	OMAP2420_GPIO_SETDATAOUT	0x94

//struct sigevent *intTeste (void *, int);
//struct sigevent event;

int pincount = 1039;
int interval = 10;
//int pwm_enable = 0;


uintptr_t gpio5, sys, /*gpt3,*/ gpt9;

int t = 0;

/* Handle a message from a client: */
//static PtConnectionMsgFunc_t msghandler;

/*static void const *msghandler(PtConnectionServer_t *connection, void *data,
		unsigned long type, void const *msgptr, unsigned msglen,
		unsigned *reply_len) {
	//struct MyMsg const *msg = (struct MyMsg const*) msgptr;
	static int reply = 0;

	switch (type) {
	case 0x55:
		puts("É um button_count read\n");
		PtConnectionReply(connection, sizeof(pincount), &pincount);
		break;

	case 0x56:
		puts("É um onda_interval read\n");
		PtConnectionReply(connection, sizeof(interval), &interval);
		break;

	case 0x66:
		puts("É um onda_interval write\n");
		if (msglen > 0)
			interval = *((int *)msgptr);
		PtConnectionReply(connection, sizeof(interval), &interval);
		break;

	default:
		puts("Não sei o q é\n");
	}

	*reply_len = sizeof(reply);
	reply = type;
	return &reply;
}*/

/* Set up a new connection: */
/*static PtConnectorCallbackFunc_t connector_callback;

static void connector_callback(PtConnector_t *connector,
		PtConnectionServer_t *connection, void *data) {
	static const PtConnectionMsgHandler_t handlers = { 0, msghandler };
	if (PtConnectionAddMsgHandlers(connection, &handlers, 1) != 0) {
		fputs("Unable to set up connection handler\n", stderr);
		PtConnectionServerDestroy(connection);
	}
}*/

struct sigevent *gpio_isr_handler(void *args, int i)
{
	//(void)args;
	if (in32(gpio5 + OMAP2420_GPIO_DATAIN) & (1 << 10)) {
		out32(gpt9 + OMAP3530_GPT_TCLR, (1<<12) | (1<<10) | (1<<7));
		out32(gpt9 + OMAP3530_GPT_TISR, 2);

		/* set the pin 139 */
		out32(gpio5 + OMAP2420_GPIO_SETDATAOUT, (1 << 11));
	} else {
		/* clear the pin 139*/
		out32(gpio5 + OMAP2420_GPIO_CLEARDATAOUT, (1 << 11));

		/* setting the initial timer counter value
		 * cada tick é 80ns */
		unsigned int t = 0xffffffff - ((interval*1000)/77);

		out32(gpt9 + OMAP3530_GPT_TLDR, t);
		out32(gpt9 + OMAP3530_GPT_TCRR, t);

		/* starting timer with PWM */
		out32(gpt9 + OMAP3530_GPT_TCLR, 3 | (1<<12) | (1<<10)); //-- PWM

		t = 0;
		pincount++;
	}
	out32(gpio5 + OMAP2420_GPIO_IRQSTATUS1, 1 << 10);

	return NULL;//&event;
}

struct sigevent *timer_isr_handler(void *args, int i)
{
	//(void)data;
	if (t)
		out32(gpio5 + OMAP2420_GPIO_CLEARDATAOUT, (1 << 11));
	else
		out32(gpio5 + OMAP2420_GPIO_SETDATAOUT, (1 << 11));

	t = (~t) & 1;
	//out32(gpt3 + OMAP3530_GPT_TISR, 2);
	out32(gpt9 + OMAP3530_GPT_TISR, 2);

	return NULL;//&event;
}

/* We specify the header as being at least a pulse */
typedef struct _pulse msg_header_t;

typedef struct _my_data {
    msg_header_t hdr;
    int data;
} my_data_t;


int main(int argc, char *argv[]) {
	uint32_t l;
	int id, id2;
	my_data_t msg;
	int rcvid;

	//ds_t ds_descriptor;
	//char ovenID[7], oven_temp[MAXLEN], flag = 0;
	//char buf[5];
	name_attach_t *name;

//	static const char name[] = "onda";

	printf("Welcome ttto the QNX Momentics IDE\n");

	if (ThreadCtl(_NTO_TCTL_IO, 0) < 0) {
		perror(NULL);
		return -1;
	}

	/*ds_descriptor = ds_register();
	if (ds_descriptor == -1) {
		perror("ds_register");
		exit(1);
	}

	//strcpy(ovenID, "oven1");

	if (ds_create(ds_descriptor, "button_count", 0, 0) == -1) {
		perror("ds_create");
		exit(1);
	}

	if (ds_create(ds_descriptor, "onda_interval", 0, 0) == -1) {
		perror("ds_create");
		exit(1);
	}

	sprintf(buf, "%d", pincount);
	ds_set(ds_descriptor, "button_count", buf, 5);

	sprintf(buf, "%d", interval);
	ds_set(ds_descriptor, "onda_interval", buf, 5);*/

	printf("Passou0\n");

	name = name_attach(NULL, "onda", NAME_FLAG_ATTACH_GLOBAL);
	if (name == NULL) {
		perror("Error0\n");
		return -1;
	}

	printf("Passou1\n");

	/* attach GPIO interrupt */
	id = InterruptAttach (33, gpio_isr_handler, NULL, 0, _NTO_INTR_FLAGS_PROCESS);

	/* attach timer interrupt */
	id2 = InterruptAttach (45, timer_isr_handler, NULL, 0,  _NTO_INTR_FLAGS_PROCESS);

	printf("Passou2\n");

	gpio5 = mmap_device_io(OMAP3530_GPIO_SIZE, OMAP3530_GPIO5_BASE);
	if (gpio5 == MAP_DEVICE_FAILED) {
		perror(NULL);
		return -1;
	}

	//gpt3 = mmap_device_io(OMAP3530_GPT_SIZE, OMAP3530_GPT3_BASE);
	gpt9 = mmap_device_io(OMAP3530_GPT_SIZE, OMAP3530_GPT9_BASE);
	if (gpt9 == MAP_DEVICE_FAILED) {
		perror(NULL);
		return -1;
	}

	sys = mmap_device_io(OMAP3530_SYSCTL_SIZE, OMAP3530_SYSCTL_BASE);
	if (sys == MAP_DEVICE_FAILED) {
		perror(NULL);
		return -1;
	}

	printf("Passou1\n");

	/* selecting mode 4 function - GPIO 139
	 * selecting pullup and mode 4 function - GPIO 138 */
#define SYS_CONF	((4 << 16) | ((1 << 8) | (1<<3) | 4))
#define SYS_MASK	~(0x10F010F)
	l = (in32(sys + 0x168) &  SYS_MASK) | SYS_CONF;
	//l = (in32(sys + 0x168) & ~(7<<16) ) | (4 << 16);
	//out32(sys + 0x168, ((1<<3 | 4) << 16) | (1<<3) | 4);
	out32(sys + 0x168, l);

	/* setting mode 2 - PWM */
	l = (in32(sys + 0x174) & ~7 ) | 2;
	out32(sys + 0x174, l);

	/* setting the PIN 138 to input
	 * setting the PIN 139 to output */
	l = (in32(gpio5 + OMAP2420_GPIO_OE) & ~(1 << 11)) | 1 << 10;
	out32(gpio5 + OMAP2420_GPIO_OE, l);

	/* enabling interrupt on both levels on GPIO 139 */
	out32(gpio5 + OMAP2420_GPIO_RISINGDETECT, l << 10);
	out32(gpio5 + OMAP2420_GPIO_FALLINGDETECT, l << 10);
	out32(gpio5 + OMAP2420_GPIO_SETIRQENABLE1, l << 10);

	/* make sure timer has stop */
	//out32(gpt3 + OMAP3530_GPT_TCLR, 0);
	out32(gpt9 + OMAP3530_GPT_TCLR, 0);

	/* enabling the interrupt */
	out32(gpt9 + OMAP3530_GPT_TIER, 2); //comentar se PWM

	/* configuring PWM */
	out32(gpt9 + OMAP3530_GPT_TCLR, (1<<12) | (1<<10) | (1<<7)); //-- PWM

	out32(gpio5 + OMAP2420_GPIO_SETDATAOUT, (1 << 11));

	//return EXIT_SUCCESS;
	/* precisa definir a função de irq */
	printf("Esperando interrupção\n");
	/*while (1) {
		ds_get(ds_descriptor, "onda_interval", buf, 5);
		sscanf(buf, "%d", &interval);
		sleep(1);
	}*/
	while (1) {
		rcvid = MsgReceive(name->chid, &msg, sizeof(msg), NULL);

		printf("MSGReceived\n");
		if (rcvid == -1) {/* Error condition, exit */
			break;
		}
		/* name_open() sends a connect message, must EOK this */
		if (msg.hdr.type == _IO_CONNECT) {
			MsgReply(rcvid, EOK, NULL, 0);
			continue;
		}

		/* Some other QNX IO message was received; reject it */
		if (msg.hdr.type > _IO_BASE && msg.hdr.type <= _IO_MAX) {
			MsgError(rcvid, ENOSYS);
			continue;
		}
		switch (msg.hdr.subtype) {
		case 0x55:
			puts("É um button_count read\n");
			MsgReply(rcvid, EOK, &pincount, sizeof(pincount));
			break;

		case 0x65:
			puts("É um onda_interval read\n");
			MsgReply(rcvid, EOK, &interval, sizeof(interval));
			break;

		case 0x66:
			puts("É um onda_interval write\n");
			interval = msg.data;
			MsgReply(rcvid, EOK, &interval, sizeof(interval));
			break;

		default:
			puts("Não sei o q é\n");
			MsgReply(rcvid, EOK, NULL, 0);
		}
	}
	out32(gpt9 + OMAP3530_GPT_TCLR, 0);
	out32(gpt9 + OMAP3530_GPT_TIER, 0);
	InterruptDetach (id);
	InterruptDetach (id2);
	printf("Fim\n");

	return EXIT_SUCCESS;
}

