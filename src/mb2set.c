#include <stdio.h>
#include <stdlib.h>
#include <modbus/modbus.h>
#include <errno.h>

#define MSB(u16) (((uint8_t* )&u16)[1])
#define LSB(u16) (((uint8_t* )&u16)[0])

int main(int argc, char *argv[])
{
	modbus_t *mb;
	uint16_t tab_reg[256];
	int rc;
	int i;

	if (argc != 2)
	{
		printf("name ip adress\n");
		exit(1);
	}
	int setvalue = 0;

	mb = modbus_new_tcp(argv[1], 502);
	if (modbus_connect(mb) == -1)
	{
		fprintf(stderr, "modbus connect: %s\n", modbus_strerror(errno));
		modbus_free(mb);
		return -1;
	}

	/* Read 5 registers from the address 100 */
	rc = modbus_read_registers(mb, 100, 5, tab_reg);
	if (rc == -1) {
		fprintf(stderr, "read registers: %s\n", modbus_strerror(errno));
		return -1;
	}

	printf("ip = %hhu.%hhu.%hhu.%hhu\n",MSB(tab_reg[0]),LSB(tab_reg[0]),MSB(tab_reg[1]),LSB(tab_reg[1]));
	printf("mac = %hhx:%hhx:%hhx:%hhx:%hhx:%hhx\n",MSB(tab_reg[2]),LSB(tab_reg[2]),MSB(tab_reg[3]),LSB(tab_reg[3]),MSB(tab_reg[4]),LSB(tab_reg[4]));
	printf("startlevel = %hu\nprecalibrationdelay = %hu\ncalibration = %hu\npostcalibrationdelay = %hu\ncelldelay = %hu\ncelllevel = %hu\nzerodelay = %hu\nzerotest = %hu\ntotalmercurydelay = %hu\ntotalmercury = %hu\nelementalmercurydelay = %hu\nelementalmercury = %hu\npurge = %hu\n",tab_reg[5],tab_reg[6],tab_reg[7],tab_reg[8],tab_reg[9],tab_reg[10],tab_reg[11],tab_reg[12],tab_reg[13],tab_reg[14],tab_reg[15],tab_reg[16],tab_reg[17]);

	modbus_close(mb);
	modbus_free(mb);
}
