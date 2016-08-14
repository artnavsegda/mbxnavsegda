#include <stdio.h>
#include <stdlib.h>
#include <modbus.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	modbus_t *mb;
	uint16_t tab_reg[256];
	int rc;
	int i;

	if (argc == 1)
	{
		printf("name ip adress\n");
		exit(1);
	}

	mb = modbus_new_tcp(argv[1], 1502);
	while (modbus_connect(mb) == -1)
	{
		fprintf(stderr, "modbus connect: %s\n", modbus_strerror(errno));
		//modbus_free(mb);
		//return -1;
	}

	while (1)
	{
		/* Read 5 registers from the address 10 */
		rc = modbus_read_registers(mb, 100, 10, tab_reg);
		if (rc == -1) {
			fprintf(stderr, "read registers: %s\n", modbus_strerror(errno));
			return -1;
		}

		for (i=0; i < rc; i++) {
			printf("reg[%d]=%d (0x%X)\n", i, tab_reg[i], tab_reg[i]);
		}
		//usleep(200000);
		sleep(1);
	}

	modbus_close(mb);

	modbus_free(mb);
}
