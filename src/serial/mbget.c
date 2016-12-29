#include <stdio.h>
#include <stdlib.h>
#include <modbus/modbus.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	modbus_t *mb;
	uint16_t tab_reg[256];
	uint8_t bit_reg[256];
	int rc;
	int i;

	if (argc != 3)
	{
		printf("name serial port and register\n");
		exit(1);
	}
	int setvalue = 0;
	int setregister;
	sscanf(argv[2],"%d",&setregister);

	mb = modbus_new_rtu(argv[1], 9600, 'N', 8, 1);

	modbus_set_slave(mb, 50);

	if (modbus_connect(mb) == -1)
	{
		fprintf(stderr, "modbus connect: %s\n", modbus_strerror(errno));
		modbus_free(mb);
		return -1;
	}

	rc = modbus_read_registers(mb, setregister, 1, tab_reg);
	if (rc == -1) {
		fprintf(stderr, "read registers: %s\n", modbus_strerror(errno));
		return -1;
	}

	printf("%d (0x%X)\n", tab_reg[0], tab_reg[0]);

	modbus_close(mb);
	modbus_free(mb);
}
