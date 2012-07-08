#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>

/* We specify the header as being at least a pulse */
typedef struct _pulse msg_header_t;

typedef struct _my_data {
    msg_header_t hdr;
    int data;
} my_data_t;

int find_param(char *query_str, char *substr, int *ret_value) {
	int found = 0, value = 0;
	char *fs = strstr(query_str, substr);
	if (fs) {
		fs += strlen(substr);

		while (*fs <= '9' && *fs >= '0') {
			found = 1;
			value = value * 10 + *fs++ - '0';
		}

		if (found && ret_value) {
			*ret_value = value;
		}
	}
	return found;
}

int main(int argc, char *argv[]) {
	char query_str[1024];
    my_data_t msg;
    int value;
    int server_coid;
    size_t len;
    int load = 0;

    printf(	"Content-Type: text/html\n\n"
    		"<html><head><title>Configuração Onda</title></head>\n"
    		"<body><h1>Configuração Onda</h1>\n");

	if ((server_coid = name_open("onda", NAME_FLAG_ATTACH_GLOBAL)) == -1) {
		printf("Falha na conexão com onda app</body></html>\n");
	    return EXIT_SUCCESS;
	}

	len = fread(query_str, 1, 1023, stdin);
    if (len) {
    	find_param(query_str, "interval=", &msg.data);
    	if (find_param(query_str, "interval=", &msg.data)) {
   			msg.hdr.type = 0x00;
   			msg.hdr.subtype = 0x66;

			/* set onda interval */
			MsgSend(server_coid, &msg, sizeof(msg), &value, sizeof(value));
    	}
    	if (find_param(query_str, "cpuload=", NULL)) {
    		/* get cpuload time */
        	find_param(query_str, "seconds=", &load);
    	}
    }

	msg.hdr.type = 0x00;
	msg.hdr.subtype = 0x55;

	if (MsgSend(server_coid, &msg, sizeof(msg), &value, sizeof(value)) != -1) {
		printf("Bot&atilde;o pressionado %d vezes\n", value);
	}

	msg.hdr.type = 0x00;
	msg.hdr.subtype = 0x65;

	if (MsgSend(server_coid, &msg, sizeof(msg), &value, sizeof(value)) != -1) {
		printf(	"<form name=\"input\" action=\"client\" method=\"post\">\n"
	    		"Onda intervalo: <input type=\"text\" name=\"interval\" value=\"%d\"/><br>\n"
				"<input type=\"checkbox\" name=\"cpuload\" value=\"1\" />Carga na CPU\n"
				"<input type=\"text\" name=\"seconds\" value=\"0\"/><br>\n"
	    		"<input type=\"submit\" value=\"Submit\" /></form>", value);
	}

	printf("</body></html>\n");

	/* Close the connection */
	name_close(server_coid);
	fclose(stdout);

    if (load) {
		clock_t future_clock, current_clock = clock();
	    future_clock = CLOCKS_PER_SEC * load + current_clock;

	    while (current_clock < future_clock) {
	    	current_clock = clock();
	    }
    }
    return EXIT_SUCCESS;
}
