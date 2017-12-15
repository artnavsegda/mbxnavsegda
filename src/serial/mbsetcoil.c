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

	if (argc != 5)
	{
		printf("name serial port, id, register and value\n");
		exit(1);
	}
	int setvalue = atoi(argv[4]);
	int setregister = atoi(argv[3]);

	mb = modbus_new_rtu(argv[1], 9600, 'N', 8, 1);

	modbus_set_slave(mb, atoi(argv[2]));

	if (modbus_connect(mb) == -1)
	{
		fprintf(stderr, "modbus connect: %s\n", modbus_strerror(errno));
		modbus_free(mb);
		return -1;
	}
	printf("%d=%d\n",setregister, setvalue);
	rc = modbus_write_bit(mb,setregister,setvalue);
	if (rc == -1) {
		fprintf(stderr, "write registers: %s\n", modbus_strerror(errno));
		return -1;
	}

	modbus_close(mb);
	modbus_free(mb);
}
