#include <stdio.h>
#include <stdlib.h>
#include <modbus/modbus.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	modbus_t *mb;
	uint16_t tab_reg[10];
	int rc;
	int i;

	if (argc == 1)
	{
		printf("name serial port\n");
		exit(1);
	}

	mb = modbus_new_rtu(argv[1], 9600, 'N', 8, 1);
	modbus_set_slave(mb, 50);
	modbus_rtu_set_serial_mode(mb, MODBUS_RTU_RS232);
	if (modbus_connect(mb) == -1)
	{
		fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
		modbus_free(mb);
		return -1;
	}

	/* Read 5 registers from the address 10 */
	rc = modbus_read_registers(mb, 360, 2, tab_reg);
	if (rc == -1) {
		fprintf(stderr, "%s\n", modbus_strerror(errno));
		return -1;
	}

	printf("%f (0X%X) (0x%X)\n", modbus_get_float(&tab_reg[0]), tab_reg[0], tab_reg[1]);

	modbus_close(mb);
	modbus_free(mb);
}
