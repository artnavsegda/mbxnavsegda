#include <stdio.h>
#include <stdlib.h>
#include <modbus.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	modbus_t *mb;
	uint16_t tab_reg[256];
	uint8_t bit_reg[256];
	int rc;
	int i;

	if (argc == 1)
	{
		printf("name ip adress\n");
		exit(1);
	}

	mb = modbus_new_tcp(argv[1], 502);
	if (modbus_connect(mb) == -1)
	{
		fprintf(stderr, "modbus connect: %s\n", modbus_strerror(errno));
		modbus_free(mb);
		return -1;
	}

	/* Read 5 registers from the address 10 */
	rc = modbus_read_registers(mb, 28, 1, tab_reg);
	if (rc == -1) {
		fprintf(stderr, "read registers: %s\n", modbus_strerror(errno));
		return -1;
	}

	for (i=0; i < rc; i++) {
		printf("reg[%d]=%d (0x%X)\n", i, tab_reg[i], tab_reg[i]);
	}

	rc = modbus_read_registers(mb, 28, 8, tab_reg);
	if (rc == -1) {
		fprintf(stderr, "float registers: %s\n", modbus_strerror(errno));
		return -1;
	}

	for (i=0; i < rc; i=i+2) {
		//printf("float[%d]=%d (0x%X)\n", i, tab_reg[i], tab_reg[i]);
		printf("float[%d]=%f\n", i, modbus_get_float(&tab_reg[i]));
	}

	/*rc = modbus_read_input_bits(mb, 100, 24, bit_reg);
	if (rc == -1) {
		fprintf(stderr, "read registers: %s\n", modbus_strerror(errno));
		return -1;
	}

	for (i=0; i < rc; i++) {
		printf("bit[%d]=%d (0x%X)\n", i, bit_reg[i], bit_reg[i]);
	}

	printf("reg1 - %X\n", modbus_get_byte_from_bits(bit_reg,0,8));
	printf("reg2 - %X\n", modbus_get_byte_from_bits(bit_reg,8,8));
	printf("reg3 - %X\n", modbus_get_byte_from_bits(bit_reg,16,8));

	rc = modbus_read_bits(mb, 100, 24, bit_reg);
	if (rc == -1) {
		fprintf(stderr, "read registers: %s\n", modbus_strerror(errno));
		return -1;
	}

	for (i=0; i < rc; i++) {
		printf("coil[%d]=%d (0x%X)\n", i, bit_reg[i], bit_reg[i]);
	}

	printf("coil1 - %X\n", modbus_get_byte_from_bits(bit_reg,0,8));
	printf("coil2 - %X\n", modbus_get_byte_from_bits(bit_reg,8,8));
	printf("coil3 - %X\n", modbus_get_byte_from_bits(bit_reg,16,8));*/

	modbus_close(mb);
	modbus_free(mb);
}
