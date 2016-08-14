#include <stdio.h>
#include <stdlib.h>
#include <modbus.h>
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

	mb = modbus_new_rtu(argv[1], 115200, 'N', 8, 1);
	modbus_set_slave(mb, 1);
	modbus_rtu_set_serial_mode(mb, MODBUS_RTU_RS232);
	if (modbus_connect(mb) == -1)
	{
		fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
		modbus_free(mb);
		return -1;
	}

	/* Read 5 registers from the address 10 */
	rc = modbus_read_registers(mb, 0, 10, tab_reg);
	if (rc == -1) {
		fprintf(stderr, "%s\n", modbus_strerror(errno));
		return -1;
	}

	for (i=0; i < rc; i++) {
		printf("reg[%d]=%d (0x%X)\n", i, tab_reg[i], tab_reg[i]);
	}

	modbus_close(mb);
	modbus_free(mb);
}
