#include <stdio.h>
#include <stdlib.h>
#include <modbus/modbus.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	struct eeprom
	{
		unsigned char ip[4];
		unsigned char mac[6];
		unsigned short length_table[13];
		unsigned char jump_table[13];
	};
	struct eeprom e;

	modbus_t *mb;
	uint16_t tab_reg[256];
	uint8_t bit_reg[256];
	int rc;
	int i;

	if (argc != 2)
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


	scanf("ip = %hhu.%hhu.%hhu.%hhu\n",&e.ip[0],&e.ip[1],&e.ip[2],&e.ip[3]);
	scanf("mac = %hhx:%hhx:%hhx:%hhx:%hhx:%hhx\n",&e.mac[0],&e.mac[1],&e.mac[2],&e.mac[3],&e.mac[4],&e.mac[5]);
	scanf("startlevel = %hu\nprecalibrationdelay = %hu\ncalibration = %hu\npostcalibrationdelay = %hu\ncelldelay = %hu\ncelllevel = %hu\nzerodelay = %hu\nzerotest = %hu\ntotalmercurydelay = %hu\ntotalmercury = %hu\nelementalmercurydelay = %hu\nelementalmercury = %hu\npurge = %hu\n",&e.length_table[0],&e.length_table[1],&e.length_table[2],&e.length_table[3],&e.length_table[4],&e.length_table[5],&e.length_table[6],&e.length_table[7],&e.length_table[8],&e.length_table[9],&e.length_table[10],&e.length_table[11],&e.length_table[12]);

	rc = modbus_write_register(mb,100,(uint16_t)e.ip[0]<<8)|e.ip[1]);
	rc = modbus_write_register(mb,101,(uint16_t)e.ip[2]<<8)|e.ip[3]);
	rc = modbus_write_register(mb,102,(uint16_t)e.mac[0]<<8)|e.mac[1]);
	rc = modbus_write_register(mb,103,(uint16_t)e.mac[2]<<8)|e.mac[3]);
	rc = modbus_write_register(mb,104,(uint16_t)e.mac[4]<<8)|e.mac[5]);
	rc = modbus_write_register(mb,105,e.length_table[0]);
	rc = modbus_write_register(mb,106,e.length_table[1]);
	rc = modbus_write_register(mb,107,e.length_table[2]);
	rc = modbus_write_register(mb,108,e.length_table[3]);
	rc = modbus_write_register(mb,109,e.length_table[4]);
	rc = modbus_write_register(mb,110,e.length_table[5]);
	rc = modbus_write_register(mb,111,e.length_table[6]);
	rc = modbus_write_register(mb,112,e.length_table[7]);
	rc = modbus_write_register(mb,113,e.length_table[8]);
	rc = modbus_write_register(mb,114,e.length_table[9]);
	rc = modbus_write_register(mb,115,e.length_table[10]);
	rc = modbus_write_register(mb,116,e.length_table[11]);
	rc = modbus_write_register(mb,117,e.length_table[12]);




	if (rc == -1) {
		fprintf(stderr, "write registers: %s\n", modbus_strerror(errno));
		return -1;
	}

	modbus_close(mb);
	modbus_free(mb);
}
