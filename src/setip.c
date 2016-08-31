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
		printf("program ip-adress new-ip\n");
		exit(1);
	}
	unsigned char ip[4];
	sscanf(argv[2],"%hhu.%hhu.%hhu.%hhu",&ip[0],&ip[1],&ip[2],&ip[3]);

	mb = modbus_new_tcp(argv[1], 502);
	if (modbus_connect(mb) == -1)
	{
		fprintf(stderr, "modbus connect: %s\n", modbus_strerror(errno));
		modbus_free(mb);
		return -1;
	}
	uint16t halfip;
	MSB(halfip) = ip[0];
	LSB(halfip) = ip[1];
	rc = modbus_write_register(mb,100,halfip);
	if (rc == -1) {
		fprintf(stderr, "write registers: %s\n", modbus_strerror(errno));
		return -1;
	}
	MSB(halfip) = ip[2];
	LSB(halfip) = ip[3];
	rc = modbus_write_register(mb,101,halfip);
	if (rc == -1) {
		fprintf(stderr, "write registers: %s\n", modbus_strerror(errno));
		return -1;
	}

	modbus_close(mb);
	modbus_free(mb);
}
