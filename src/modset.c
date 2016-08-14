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

	if (argc != 4)
	{
		printf("name ip adress offset and byte\n");
		exit(1);
	}
	uint8_t scanbyte;
	sscanf(argv[3],"%hhu",&scanbyte);
	modbus_set_bits_from_byte(bit_reg, 0, scanbyte);
	int offset;
	sscanf(argv[2],"%d",&offset);

	mb = modbus_new_tcp(argv[1], 502);
	if (modbus_connect(mb) == -1)
	{
		fprintf(stderr, "modbus connect: %s\n", modbus_strerror(errno));
		modbus_free(mb);
		return -1;
	}

	/* Read 5 registers from the address 10 */
	//rc = modbus_read_registers(mb, 100, 10, tab_reg);
	rc = modbus_write_bit(mb,106,TRUE);
	//rc = modbus_write_bits(mb,offset,7,bit_reg);
	if (rc == -1) {
		fprintf(stderr, "write registers: %s\n", modbus_strerror(errno));
		return -1;
	}

	/*for (i=0; i < rc; i++) {
		printf("reg[%d]=%d (0x%X)\n", i, tab_reg[i], tab_reg[i]);
	}

	rc = modbus_read_bits(mb, 100, 24, bit_reg);
	if (rc == -1) {
		fprintf(stderr, "read registers: %s\n", modbus_strerror(errno));
		return -1;
	}*/

	for (i=0; i < 8; i++) {
		printf("bit[%d]=%d (0x%X)\n", i, bit_reg[i], bit_reg[i]);
	}

	/*printf("reg1 - %X\n", modbus_get_byte_from_bits(bit_reg,0,8));
	printf("reg2 - %X\n", modbus_get_byte_from_bits(bit_reg,8,8));
	printf("reg3 - %X\n", modbus_get_byte_from_bits(bit_reg,16,8));*/

	modbus_close(mb);
	modbus_free(mb);
}
