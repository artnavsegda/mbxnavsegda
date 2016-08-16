#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <modbus/modbus.h>

#define _BV(bit) (1 << (bit))

#define U3_IGNIT 1
#define SERVO_1_LEFT_OUT 7
#define SERVO_1_RIGHT_OUT 6
#define SERVO_1_LEFT_IN 5
#define SERVO_1_RIGHT_IN 4

#define SERVO_2_LEFT_OUT 3
#define SERVO_2_RIGHT_OUT 2
#define SERVO_2_LEFT_IN 1
#define SERVO_2_RIGHT_IN 7

#define SERVO_3_LEFT_OUT 6
#define SERVO_3_RIGHT_OUT 5
#define SERVO_3_LEFT_IN 4
#define SERVO_3_RIGHT_IN 3

#define SERVO_4_LEFT_OUT 2
#define SERVO_4_RIGHT_OUT 1
#define SERVO_4_LEFT_IN 7
#define SERVO_4_RIGHT_IN 6

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

enum pca9557_direction {
	PCA9557_DIR_INPUT,
	PCA9557_DIR_OUTPUT,
};

int modenumber = 0;

void pca9557_init(int fd, char addr)
{
	ioctl(fd,I2C_SLAVE,addr);
	// polarity all bits retained
	i2c_smbus_write_byte_data(fd, 0x02, 0x00);
}

void pca9557_set_pin_dir(int fd, char addr, char port, enum pca9557_direction direction)
{
	char state;
	ioctl(fd,I2C_SLAVE,addr);
	state = i2c_smbus_read_byte_data(fd,0x03);
	if (direction == PCA9557_DIR_INPUT)
		state |= _BV(port);
	else if (direction == PCA9557_DIR_OUTPUT)
		state &= ~_BV(port);
	i2c_smbus_write_byte_data(fd, 0x03, state);
}

void pca9557_set_pin_level(int fd, char addr, char port, bool level)
{
	char state;
	ioctl(fd,I2C_SLAVE,addr);
	state = i2c_smbus_read_byte_data(fd,0x01);
	if (level)
		state |= _BV(port);
	else
		state &= ~_BV(port);
	i2c_smbus_write_byte_data(fd, 0x01, state);
};

char pca9557_get_pin_level(int fd, char addr, char port)
{
	char state;
	ioctl(fd,I2C_SLAVE,addr);
	state = i2c_smbus_read_byte_data(fd,0x00);
	return state & _BV(port);
}

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
        switch(modetoenter)
	{
		case STARTLEVEL:
		break;
		case CELLDELAY:
			pca9557_set_pin_level(U1, SERVO_1_LEFT_OUT, false);
			pca9557_set_pin_level(U1, SERVO_1_RIGHT_OUT, true);
		break;
		case CELLLEVEL:
			pca9557_set_pin_level(U1, SERVO_1_LEFT_OUT, false);
			pca9557_set_pin_level(U1, SERVO_1_RIGHT_OUT, true);
		break;
		case ZERODELAY:
			//writecoil(100, 0);
			//writecoil(4, 1);
			pca9557_set_pin_level(0x1a, VALVE_ZM, true);
		return;
		break;
		case ZEROTEST:
			//writecoil(100, 0);
			//writecoil(4, 1);
			pca9557_set_pin_level(0x1a, VALVE_ZM, true);
		break;
		case PURGE:
			//writecoil(102, false);
		break;
		case TOTALMERCURYDELAY:
		break;
		case TOTALMERCURY:
		break;
		case ELEMENTALMERCURYDELAY:
			//writecoil(101, false);
			pca9557_set_pin_level(U3, VALVE_TE, true);
		break;
		case ELEMENTALMERCURY:
			pca9557_set_pin_level(U3, VALVE_TE, true);
		break;
		case PRECALIBRATIONDELAY:
			//writecoil(99, false);
			//writecoil(4, 1);
			//pca9557_set_pin_level(U1, SERVO_1_LEFT_OUT, false);
			//pca9557_set_pin_level(U1, SERVO_1_RIGHT_OUT, true);
		break;
		case CALIBRATION:
			//writecoil(99, false);
			//writecoil(4, 1);
			pca9557_set_pin_level(U3, VALVE_CM, true);
		break;
		case POSTCALIBRATIONDELAY:
			//writecoil(99, false);
			//writecoil(4, 1);
			pca9557_set_pin_level(U3, VALVE_CM, true);
		break;
		default:
		break;
	}
	printf("next mode duration %d seconds\n",modeseconds(sequence(modenumber)));
}

void exitmode(int modetoexit)
{
	printf("exit mode %d\n",modetoexit);
        switch(modetoexit)
	{
		case STARTLEVEL:
		break;
		case CELLDELAY:
		break;
		case CELLLEVEL:
			//celllevelavg = average(runner,CELLLEVELSECONDS,runflag,MEMORYUSE)/CELLLEVELSECONDS;
			//celltempavg = average(temprunner,CELLLEVELSECONDS,temprunflag,CELLLEVELSECONDS)/CELLLEVELSECONDS;
			pca9557_set_pin_level(U1, SERVO_1_LEFT_OUT, true);
			pca9557_set_pin_level(U1, SERVO_1_RIGHT_OUT, false);
		break;
		case ZERODELAY:
		break;
		case ZEROTEST:
			//zerolevelavg = average(runner,ZEROTESTSECONDS,runflag,MEMORYUSE)/ZEROTESTSECONDS;
			pca9557_set_pin_level(U3, VALVE_ZM, false);
			//writecoil(4, false);
		break;
		case PURGE:
			//writecoil(103, false);
		break;
		case TOTALMERCURYDELAY:
		break;
		case TOTALMERCURY:
		break;
		case ELEMENTALMERCURYDELAY:
		break;
		case ELEMENTALMERCURY:
                        pca9557_set_pin_level(U3, VALVE_TE, false);
			//writecoil(5, false);
		break;
		case PRECALIBRATIONDELAY:
		break;
		case CALIBRATION:
			//coefficent = average(runner,CALIBRATIONSECONDS,runflag,MEMORYUSE)/CALIBRATIONSECONDS;
			pca9557_set_pin_level(U3, VALVE_CM, false);
			//pca9557_set_pin_level(U1, SERVO_1_LEFT_OUT, true);
			//pca9557_set_pin_level(U1, SERVO_1_RIGHT_OUT, false);
			//writecoil(5, false);
		break;
		case POSTCALIBRATIONDELAY:
			pca9557_set_pin_level(U3, VALVE_CM, true);
			//writecoil(5, false);
		break;
		default:
		break;
	}
	entermode(sequence(modetoexit));
}

void alarm_handler(int signal)
{
	printf("alarm\n");
	exitmode(modenumber);
}

int main(void)
{
        int socket, fd;
        modbus_t *ctx;
        modbus_mapping_t *mb_mapping;

        signal(SIGALRM, alarm_handler);

        fd = open("/dev/i2c-1",O_RDWR);

        if (fd<=1)
        {
                printf("error on device\n");
                exit(1);
        }
        pca9557_init(fd, 0x18);
	pca9557_set_pin_dir(fd, 0x18, SERVO_1_LEFT_OUT, PCA9557_DIR_OUTPUT);
	pca9557_set_pin_dir(fd, 0x18, SERVO_1_RIGHT_OUT, PCA9557_DIR_OUTPUT);
	pca9557_set_pin_dir(fd, 0x18, SERVO_1_LEFT_IN, PCA9557_DIR_INPUT);
	pca9557_set_pin_dir(fd, 0x18, SERVO_1_RIGHT_IN, PCA9557_DIR_INPUT);
	pca9557_set_pin_dir(fd, 0x18, SERVO_2_LEFT_OUT, PCA9557_DIR_OUTPUT);
	pca9557_set_pin_dir(fd, 0x18, SERVO_2_RIGHT_OUT, PCA9557_DIR_OUTPUT);
	pca9557_set_pin_dir(fd, 0x18, SERVO_2_LEFT_IN, PCA9557_DIR_INPUT);
	pca9557_init(0x19);
	pca9557_set_pin_dir(fd, 0x19, SERVO_2_RIGHT_IN, PCA9557_DIR_INPUT);
	pca9557_set_pin_dir(fd, 0x19, SERVO_3_LEFT_OUT, PCA9557_DIR_OUTPUT);
	pca9557_set_pin_dir(fd, 0x19, SERVO_3_RIGHT_OUT, PCA9557_DIR_OUTPUT);
	pca9557_set_pin_dir(fd, 0x19, SERVO_3_LEFT_IN, PCA9557_DIR_INPUT);
	pca9557_set_pin_dir(fd, 0x19, SERVO_3_RIGHT_IN, PCA9557_DIR_INPUT);
	pca9557_set_pin_dir(fd, 0x19, SERVO_4_LEFT_OUT, PCA9557_DIR_OUTPUT);
	pca9557_set_pin_dir(fd, 0x19, SERVO_4_RIGHT_OUT, PCA9557_DIR_OUTPUT);
	pca9557_init(0x1a);
	pca9557_set_pin_dir(fd, 0x1a, SERVO_4_LEFT_IN, PCA9557_DIR_INPUT);
	pca9557_set_pin_dir(fd, 0x1a, SERVO_4_RIGHT_IN, PCA9557_DIR_INPUT);
	pca9557_set_pin_dir(fd, 0x1a, U3_IGNIT, PCA9557_DIR_OUTPUT);
	pca9557_set_pin_level(fd, 0x1a, U3_IGNIT, true);

        ctx = modbus_new_tcp("127.0.0.1", 502);
        /* modbus_set_debug(ctx, TRUE); */

        mb_mapping = modbus_mapping_new(500, 500, 500, 500);
        if (mb_mapping == NULL)
        {
                fprintf(stderr, "Failed to allocate the mapping: %s\n",
                modbus_strerror(errno));
                modbus_free(ctx);
                return -1;
        }

        entermode(STARTLEVEL);

        socket = modbus_tcp_listen(ctx, 1);
        modbus_tcp_accept(ctx, &socket);
        float popugai = (3.163/1.6)/4095;

        for (;;)
        {
                uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
                int rc;

                modbus_set_float((float)modenumber, &mb_mapping->tab_registers[8]);
                mb_mapping->tab_input_registers[0] = 0x16;
                ioctl(fd,I2C_SLAVE,0x09);
                mb_mapping->tab_registers[100] = i2c_smbus_read_word_data(fd,0x00);
                mb_mapping->tab_registers[101] = i2c_smbus_read_word_data(fd,0x01);
                mb_mapping->tab_registers[102] = i2c_smbus_read_word_data(fd,0x02);
                mb_mapping->tab_registers[103] = i2c_smbus_read_word_data(fd,0x03);
                mb_mapping->tab_registers[104] = i2c_smbus_read_word_data(fd,0x04);
                mb_mapping->tab_registers[105] = i2c_smbus_read_word_data(fd,0x05);
                mb_mapping->tab_registers[106] = i2c_smbus_read_word_data(fd,0x06);
                mb_mapping->tab_registers[107] = i2c_smbus_read_word_data(fd,0x07);
                mb_mapping->tab_registers[108] = i2c_smbus_read_word_data(fd,0x08);
                mb_mapping->tab_registers[109] = i2c_smbus_read_word_data(fd,0x09);
                mb_mapping->tab_registers[110] = i2c_smbus_read_word_data(fd,0x0a);
                mb_mapping->tab_registers[111] = i2c_smbus_read_word_data(fd,0x0b);
                modbus_set_float((i2c_smbus_read_word_data(fd,0x00)-180)*popugai,&mb_mapping->tab_registers[200]);
                modbus_set_float((i2c_smbus_read_word_data(fd,0x01)-180)*popugai,&mb_mapping->tab_registers[202]);
                modbus_set_float((i2c_smbus_read_word_data(fd,0x02)-180)*popugai,&mb_mapping->tab_registers[204]);
                modbus_set_float((i2c_smbus_read_word_data(fd,0x03)-180)*popugai,&mb_mapping->tab_registers[206]);
                modbus_set_float((i2c_smbus_read_word_data(fd,0x04)-180)*popugai,&mb_mapping->tab_registers[208]);
                modbus_set_float((i2c_smbus_read_word_data(fd,0x05)-180)*popugai,&mb_mapping->tab_registers[210]);
                modbus_set_float((i2c_smbus_read_word_data(fd,0x06)-180)*popugai,&mb_mapping->tab_registers[212]);
                modbus_set_float((i2c_smbus_read_word_data(fd,0x07)-180)*popugai,&mb_mapping->tab_registers[214]);
                modbus_set_float(((((i2c_smbus_read_word_data(fd,0x03)-180)*popugai)-0.5)*100),&mb_mapping->tab_registers[216]);

                ioctl(fd,I2C_SLAVE,0x18);
                modbus_set_bits_from_byte(mb_mapping->tab_input_bits,100,i2c_smbus_read_byte_data(fd,0x00));
                i2c_smbus_write_byte_data(fd,0x01,modbus_get_byte_from_bits(mb_mapping->tab_bits,100,8));
                ioctl(fd,I2C_SLAVE,0x19);
                modbus_set_bits_from_byte(mb_mapping->tab_input_bits,108,i2c_smbus_read_byte_data(fd,0x00));
                i2c_smbus_write_byte_data(fd,0x01,modbus_get_byte_from_bits(mb_mapping->tab_bits,108,8));
                ioctl(fd,I2C_SLAVE,0x1a);
                modbus_set_bits_from_byte(mb_mapping->tab_input_bits,116,i2c_smbus_read_byte_data(fd,0x00));
                i2c_smbus_write_byte_data(fd,0x01,modbus_get_byte_from_bits(mb_mapping->tab_bits,116,8));

                rc = modbus_receive(ctx, query);
                if (rc != -1)
                {
                        /* rc is the query size */
                        modbus_reply(ctx, query, rc, mb_mapping);
                }
                else
                {
                        /* Connection closed by the client or error */
                        //break;
                        printf("Con closed\n");
                        modbus_close(ctx);
                        modbus_tcp_accept(ctx, &socket);
                }
        }

        printf("Quit the loop: %s\n", modbus_strerror(errno));

        modbus_mapping_free(mb_mapping);
        modbus_close(ctx);
        modbus_free(ctx);

        return 0;
}
