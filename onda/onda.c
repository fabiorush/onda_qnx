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
#include <sys/iofunc.h>
#include <sys/dispatch.h>

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
uintptr_t gpio5, sys, gpt9;
int t = 0;

struct sigevent *gpio_isr_handler(void *args, int i)
{
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

	return NULL;
}

struct sigevent *timer_isr_handler(void *args, int i)
{
	if (t)
		out32(gpio5 + OMAP2420_GPIO_CLEARDATAOUT, (1 << 11));
	else
		out32(gpio5 + OMAP2420_GPIO_SETDATAOUT, (1 << 11));

	t = (~t) & 1;
	out32(gpt9 + OMAP3530_GPT_TISR, 2);

	return NULL;
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

	name_attach_t *name;

	printf("Welcome Onda\n");

	if (ThreadCtl(_NTO_TCTL_IO, 0) < 0) {
		perror(NULL);
		return -1;
	}

	name = name_attach(NULL, "onda", NAME_FLAG_ATTACH_GLOBAL);
	if (name == NULL) {
		perror("Error0\n");
		return -1;
	}

	/* attach GPIO interrupt */
	id = InterruptAttach (33, gpio_isr_handler, NULL, 0, _NTO_INTR_FLAGS_PROCESS);

	/* attach timer interrupt */
	id2 = InterruptAttach (45, timer_isr_handler, NULL, 0,  _NTO_INTR_FLAGS_PROCESS);

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
	out32(gpt9 + OMAP3530_GPT_TCLR, 0);

	/* enabling the interrupt */
	out32(gpt9 + OMAP3530_GPT_TIER, 2); //comentar se PWM

	/* configuring PWM */
	out32(gpt9 + OMAP3530_GPT_TCLR, (1<<12) | (1<<10) | (1<<7)); //-- PWM

	out32(gpio5 + OMAP2420_GPIO_SETDATAOUT, (1 << 11));

	printf("Esperando requisições\n");

	while (1) {
		rcvid = MsgReceive(name->chid, &msg, sizeof(msg), NULL);

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
			MsgReply(rcvid, EOK, &pincount, sizeof(pincount));
			break;

		case 0x65:
			MsgReply(rcvid, EOK, &interval, sizeof(interval));
			break;

		case 0x66:
			interval = msg.data;
			MsgReply(rcvid, EOK, &interval, sizeof(interval));
			break;

		default:
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
