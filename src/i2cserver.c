#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <modbus.h>

int main(void)
{
    int socket, fd;
    modbus_t *ctx;
    modbus_mapping_t *mb_mapping;

    fd = open("/dev/i2c-1",O_RDWR);


	if (fd<=1)
	{
		printf("error on device\n");
		exit(1);
	}
	ioctl(fd,I2C_SLAVE,0x18);
	i2c_smbus_write_byte_data(fd,0x02,0x00);
	i2c_smbus_write_byte_data(fd,0x03,0x00);
	ioctl(fd,I2C_SLAVE,0x19);
	i2c_smbus_write_byte_data(fd,0x02,0x00);
	i2c_smbus_write_byte_data(fd,0x03,0x00);
	ioctl(fd,I2C_SLAVE,0x1a);
	i2c_smbus_write_byte_data(fd,0x02,0x00);
	i2c_smbus_write_byte_data(fd,0x03,0x00);

    ctx = modbus_new_tcp("127.0.0.1", 502);
    /* modbus_set_debug(ctx, TRUE); */

    mb_mapping = modbus_mapping_new(500, 500, 500, 500);
    if (mb_mapping == NULL) {
        fprintf(stderr, "Failed to allocate the mapping: %s\n",
                modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    socket = modbus_tcp_listen(ctx, 1);
    modbus_tcp_accept(ctx, &socket);
    float popugai = (3.163/1.6)/4095;

    for (;;) {
        uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
        int rc;
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
        if (rc != -1) {
            /* rc is the query size */
            modbus_reply(ctx, query, rc, mb_mapping);
        } else {
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
