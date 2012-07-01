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

#define MY_PULSE_CODE   _PULSE_CODE_MINAVAIL

struct sigevent *intTeste (void *, int);
struct sigevent event;
uintptr_t gpt3, gpio5, conf;
int t=0;

int main(int argc, char *argv[]) {
	//FILE *teste = fopen("/tmp/teste.txt","w");
	uint32_t l, i = 0;
	int id;

	printf("Welcome ttto the QNX Momentics IDE\n");
	//fclose(teste);*/
	if (ThreadCtl(_NTO_TCTL_IO, 0) < 0) {
		perror(NULL);
		return -1;
	}

	/* attach GPIO interrupt */
//	id = InterruptAttach (33, intTeste, NULL, 0, 0);

	/* attach timer interrupt */
	id = InterruptAttach (39, intTeste, NULL, 0,  _NTO_INTR_FLAGS_PROCESS);
	//GPIO_DATAOUT: 0x03C 0x4905603C
	//GPIO_CLEARDATAOUT : 0x090 0x49056090
	//GPIO_SETDATAOUT: 0x094 0x49056094

	gpio5 = mmap_device_io(OMAP3530_GPIO_SIZE, OMAP3530_GPIO5_BASE);
	if (gpio5 == MAP_DEVICE_FAILED) {
		perror(NULL);
		return -1;
	}

	//dataout = mmap_device_io(4, 0x49056034);
	//if (dataout == MAP_DEVICE_FAILED) {
	gpt3 = mmap_device_io(OMAP3530_GPT_SIZE, OMAP3530_GPT3_BASE);
	//gpt3 = mmap_device_io(OMAP3530_GPT_SIZE, OMAP3530_GPT9_BASE);
	if (gpt3 == MAP_DEVICE_FAILED) {
		perror(NULL);
		return -1;
	}

	conf = mmap_device_io(OMAP3530_SYSCTL_SIZE, OMAP3530_SYSCTL_BASE);
	if (conf == MAP_DEVICE_FAILED) {
		perror(NULL);
		return -1;
	}

	out32(gpio5 + OMAP2420_GPIO_SYSCONFIG, (1<<3));
	out32(gpio5 + OMAP2420_GPIO_CTRL, 0);

	sleep(2);

	printf("OMAP2420_GPIO_SYSCONFIG: %x\n", in32(gpio5 + OMAP2420_GPIO_SYSCONFIG));
	printf("OMAP2420_GPIO_CTRL: %x\n", in32(gpio5 + OMAP2420_GPIO_CTRL));

	/* setting the PIN 139 to output */
	l = in32(gpio5 + OMAP2420_GPIO_OE) & ~(1 << 11);
	out32(gpio5 + OMAP2420_GPIO_OE, l);

	/* setting the PIN 139 to output */
	out32(gpio5 + OMAP2420_GPIO_SETDATAOUT, (1 << 11));
	out32(gpio5 + OMAP2420_GPIO_CLEARDATAOUT, (1 << 11));
	out32(gpio5 + OMAP2420_GPIO_SETDATAOUT, (1 << 11));
	out32(gpio5 + OMAP2420_GPIO_CLEARDATAOUT, (1 << 11));
	out32(gpio5 + OMAP2420_GPIO_SETDATAOUT, (1 << 11));
	out32(gpio5 + OMAP2420_GPIO_CLEARDATAOUT, (1 << 11));
	out32(gpio5 + OMAP2420_GPIO_SETDATAOUT, (1 << 11));
	out32(gpio5 + OMAP2420_GPIO_CLEARDATAOUT, (1 << 11));
	out32(gpio5 + OMAP2420_GPIO_SETDATAOUT, (1 << 11));
	out32(gpio5 + OMAP2420_GPIO_CLEARDATAOUT, (1 << 11));
	out32(gpio5 + OMAP2420_GPIO_SETDATAOUT, (1 << 11));
	out32(gpio5 + OMAP2420_GPIO_CLEARDATAOUT, (1 << 11));

	return EXIT_SUCCESS;
	/* setting the PIN to PWM
	 *
	 * Mode 2:
	 *
	 */
//
//	l = (in32(conf + 0x174) & ~7 ) | 2;
//	out32(conf + 0x174, l);

	/* stopping timer */
	out32(gpt3 + OMAP3530_GPT_TCLR, 0);

	sleep(1);

	/* setting the initial timer counter value
	 * cada tick é 80ns
	 */
//	out32(gpt3 + OMAP3530_GPT_TLDR, 0xffffffed);
//	out32(gpt3 + OMAP3530_GPT_TLDR, 0xfffffff3);
//	out32(gpt3 + OMAP3530_GPT_TCRR, 0xfffffff3);
	out32(gpt3 + OMAP3530_GPT_TLDR, 0xffffff80);
	out32(gpt3 + OMAP3530_GPT_TCRR, 0xffffff80);
//	out32(gpt3 + OMAP3530_GPT_TLDR, 0xffffff00);
//	out32(gpt3 + OMAP3530_GPT_TCRR, 0xffffff00);

	/* enabling the interrupt */
//	out32(gpt3 + OMAP3530_GPT_TCLR, 0);
	out32(gpt3 + OMAP3530_GPT_TIER, 2);

	sleep(1);

	/* starting timer */
	out32(gpt3 + OMAP3530_GPT_TCLR, 3);
	/* (1<<12) - toggle
	 * [11:10] - 0x1: Overflow trigger (1<<10)
	 * 3 start and reload
	 */
//	out32(gpt3 + OMAP3530_GPT_TCLR, 3 | (1<<12) | (1<<10));

	//	l = in32(gpt3 + OMAP3530_GPT_TISTAT);
//	printf("OMAP3530_GPT_TLDR: %x\n", in32(gpt3 + OMAP3530_GPT_TLDR));
//	printf("OMAP3530_GPT_TCRR: %x\n", in32(gpt3 + OMAP3530_GPT_TCRR));
//	for (;;)
//	while (in32(gpt3 + OMAP3530_GPT_TCRR) != 0)
//		printf("OMAP3530_GPT_TCRR: %x\n", in32(gpt3 + OMAP3530_GPT_TCRR));
	//printf("OMAP3530_GPT_TCLR: %x\n", l);

//	l = in32(conf + OMAP3530_SYSCTL_PADCONF(78)) & ~(18 << 16);
//	out32(conf + OMAP3530_SYSCTL_PADCONF(78), l);

//	l = in32(gpio5 + OMAP2420_GPIO_SETDATAOUT) & ~(1 << 11);
//	l = in32(gpio5 + OMAP2420_GPIO_SETDATAOUT) | (1 << 11);
//	out32(gpio5 + OMAP2420_GPIO_SETDATAOUT, l);

//	l = in32(gpio5 + OMAP2420_GPIO_RISINGDETECT) | (1 << 11);
//	out32(gpio5 + OMAP2420_GPIO_RISINGDETECT, l);
//
//	l = in32(gpio5 + OMAP2420_GPIO_FALLINGDETECT) | (1 << 11);
//	out32(gpio5 + OMAP2420_GPIO_FALLINGDETECT, l);
//
//	out32(gpio5 + OMAP2420_GPIO_SETIRQENABLE1, (1 << 11));


	//return EXIT_SUCCESS;
	/* precisa definir a função de irq */
	printf("Esperando interrupção\n");
	sleep(10);
	out32(gpt3 + OMAP3530_GPT_TCLR, 0);
	out32(gpt3 + OMAP3530_GPT_TIER, 0);
	InterruptDetach (id);
	printf("Fim\n");

//	for (;;) {
//		int t=0;
//		InterruptWait(NULL, NULL);
//		printf("Interrupcao acionada\n");
//		if (t)
//			out32(gpio5 + OMAP2420_GPIO_CLEARDATAOUT, (1 << 11));
//		else
//			out32(gpio5 + OMAP2420_GPIO_SETDATAOUT, (1 << 11));
//
//		t = (~t) & 1;
//	}

	return EXIT_SUCCESS;
}

struct sigevent *intTeste (void *args, int i)
{
	InterruptDisable();
//	out32(gpio5 + OMAP2420_GPIO_IRQSTATUS1, (1 << 11));
//	out32(gpt3 + OMAP3530_GPT_TIER, 2);
	if (t)
		out32(gpio5 + OMAP2420_GPIO_CLEARDATAOUT, (1 << 11));
	else
		out32(gpio5 + OMAP2420_GPIO_SETDATAOUT, (1 << 11));

	t = (~t) & 1;
	out32(gpt3 + OMAP3530_GPT_TISR, 2);
	InterruptEnable();
	return NULL;//&event;
}
