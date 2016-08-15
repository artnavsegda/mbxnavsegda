#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <modbus/modbus.h>

int main(void)
{
	int socket;
	modbus_t *ctx;
	modbus_mapping_t *mb_mapping;

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

	socket = modbus_tcp_listen(ctx, 1);
	modbus_tcp_accept(ctx, &socket);

	for (;;)
	{
		uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
		int rc;

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

