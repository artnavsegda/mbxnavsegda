#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <modbus/modbus.h>

#define STARTLEVEL 5
#define CELLDELAY 7
#define CELLLEVEL 8
#define ZERODELAY 11
#define ZEROTEST 12
#define PURGE 13
#define TOTALMERCURYDELAY 21
#define TOTALMERCURY 22
#define ELEMENTALMERCURYDELAY 26
#define ELEMENTALMERCURY 27
#define PRECALIBRATIONDELAY 31
#define CALIBRATION 32
#define POSTCALIBRATIONDELAY 33

#define STARTLEVELSECONDS 8
#define CELLDELAYSECONDS 12
#define CELLLEVELSECONDS 30
#define ZERODELAYSECONDS 12
#define ZEROTESTSECONDS 30
#define PURGESECONDS 32768
#define TOTALMERCURYDELAYSECONDS 15
#define TOTALMERCURYSECONDS 720
#define ELEMENTALMERCURYDELAYSECONDS 5
#define ELEMENTALMERCURYSECONDS 100
#define PRECALIBRATIONDELAYSECONDS 5
#define CALIBRATIONSECONDS 64
#define POSTCALIBRATIONDELAYSECONDS 10

int modenumber = 0;

int modeseconds(int modeneed)
{
 switch (modeneed)
 {
	 case STARTLEVEL: return STARTLEVELSECONDS;
	 case CELLDELAY: return CELLDELAYSECONDS;
	 case CELLLEVEL:	return CELLLEVELSECONDS;
	 case ZERODELAY: return ZERODELAYSECONDS;
	 case ZEROTEST: return ZEROTESTSECONDS;
	 case PURGE: return PURGESECONDS;
	 case TOTALMERCURYDELAY: return TOTALMERCURYDELAYSECONDS;
	 case TOTALMERCURY: return TOTALMERCURYSECONDS;
	 case ELEMENTALMERCURYDELAY: return ELEMENTALMERCURYDELAYSECONDS;
	 case ELEMENTALMERCURY: return ELEMENTALMERCURYSECONDS;
	 case PRECALIBRATIONDELAY: return PRECALIBRATIONDELAYSECONDS;
	 case CALIBRATION: return CALIBRATIONSECONDS;
	 case POSTCALIBRATIONDELAY: return POSTCALIBRATIONDELAYSECONDS;
 }
 return 0;
}

int sequence(int modetosequence)
{
	switch(modetosequence)
	{
		case STARTLEVEL:
			return CELLDELAY;
		break;
		case CELLDELAY:
			return CELLLEVEL;
		break;
		case CELLLEVEL:
			return TOTALMERCURYDELAY;
		break;
		case ZERODELAY:
			return ZEROTEST;
		break;
		case ZEROTEST:
			return TOTALMERCURYDELAY;
		break;
		case PURGE:
			return TOTALMERCURYDELAY;
		break;
		case TOTALMERCURYDELAY:
			return TOTALMERCURY;
		break;
		case TOTALMERCURY:
			return TOTALMERCURY;
		break;
		case ELEMENTALMERCURYDELAY:
			return ELEMENTALMERCURY;
		break;
		case ELEMENTALMERCURY:
			return TOTALMERCURY;
		break;
		case PRECALIBRATIONDELAY:
			return CALIBRATION;
		break;
		case CALIBRATION:
			return POSTCALIBRATIONDELAY;
		break;
		case POSTCALIBRATIONDELAY:
			return TOTALMERCURYDELAY;
		break;
		default:
			return TOTALMERCURY;
		break;
	}
	return TOTALMERCURY;
}

void entermode(int modetoenter)
{
	modenumber = modetoenter;
	printf("enter mode %d\n", modetoenter);
	alarm(modeseconds(modetoenter));
	printf("waiting %d seconds\n",modeseconds(modenumber));
	printf("next mode duration %d seconds\n",modeseconds(sequence(modenumber)));
}

void exitmode(int modetoexit)
{
	printf("exit mode %d\n",modetoexit);
	entermode(sequence(modetoexit));
}

void alarm_handler(int signal)
{
	printf("alarm\n");
	exitmode(modenumber);
}

int main(void)
{
	int socket;
	modbus_t *ctx;
	modbus_mapping_t *mb_mapping;

	signal(SIGALRM, alarm_handler);

	ctx = modbus_new_tcp("127.0.0.1", 502);
	//ctx = modbus_new_tcp("192.168.1.110", 502);
	/* modbus_set_debug(ctx, TRUE); */

	mb_mapping = modbus_mapping_new(500, 500, 500, 500);
	if (mb_mapping == NULL)
	{
  		fprintf(stderr, "Failed to allocate the mapping: %s\n", modbus_strerror(errno));
		modbus_free(ctx);
		return -1;
	}
	mb_mapping->tab_bits[0] = TRUE;
	mb_mapping->tab_bits[1] = TRUE;
	mb_mapping->tab_bits[2] = TRUE;
	mb_mapping->tab_bits[3] = FALSE;
	mb_mapping->tab_bits[4] = FALSE;

	mb_mapping->tab_registers[0] = 0;
	mb_mapping->tab_registers[1] = 1;
	mb_mapping->tab_registers[2] = 2;
	mb_mapping->tab_registers[3] = 3;
	mb_mapping->tab_registers[4] = 4;
	mb_mapping->tab_registers[5] = 5;
	mb_mapping->tab_registers[6] = 6;
	mb_mapping->tab_registers[7] = 7;

	modbus_set_float(1.0, &mb_mapping->tab_registers[8]);
	modbus_set_float(1.0, &mb_mapping->tab_registers[10]);
	modbus_set_float(1.0, &mb_mapping->tab_registers[12]);
	modbus_set_float(1.0, &mb_mapping->tab_registers[14]);
	modbus_set_float(1.0, &mb_mapping->tab_registers[16]);
	modbus_set_float(1.0, &mb_mapping->tab_registers[18]);
	modbus_set_float(1.0, &mb_mapping->tab_registers[20]);
	modbus_set_float(1.0, &mb_mapping->tab_registers[22]);
	modbus_set_float(1.0, &mb_mapping->tab_registers[24]);
	modbus_set_float(1.0, &mb_mapping->tab_registers[26]);
	modbus_set_float(1.0, &mb_mapping->tab_registers[28]);
	modbus_set_float(1.0, &mb_mapping->tab_registers[30]);
	modbus_set_float(1.0, &mb_mapping->tab_registers[32]);

	mb_mapping->tab_bits[99] = FALSE;
	mb_mapping->tab_bits[100] = FALSE;
	mb_mapping->tab_bits[101] = FALSE;
	mb_mapping->tab_bits[102] = FALSE;
	mb_mapping->tab_bits[103] = FALSE;

	entermode(STARTLEVEL);

	socket = modbus_tcp_listen(ctx, 1);
	modbus_tcp_accept(ctx, &socket);

	for (;;)
	{
		uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
		int rc;

		modbus_set_float((float)modenumber, &mb_mapping->tab_registers[8]);

		rc = modbus_receive(ctx, query);
		if (rc != -1)
		{
			/* rc is the query size */
			modbus_reply(ctx, query, rc, mb_mapping);
		}
		else
		{
			/* Connection closed by the client or error */
			modbus_close(ctx);
			modbus_tcp_accept(ctx, &socket);
			//break;
		}
    	}

	printf("Quit the loop: %s\n", modbus_strerror(errno));
	modbus_mapping_free(mb_mapping);
	modbus_close(ctx);
	modbus_free(ctx);
	return 0;
}
