#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <modbus/modbus.h>

int main(int argc, char *argv[])
{
	uint8_t query[MODBUS_RTU_MAX_ADU_LENGTH];
	int rc;
	int socket;
	modbus_t *ctx;
	modbus_mapping_t *mb_mapping;

	if (argc == 1)
	{
		printf("name serial port\n");
		exit(1);
	}

	ctx = modbus_new_rtu(argv[1], 9600, 'N', 8, 1);
	modbus_set_slave(ctx, 50);
	modbus_rtu_set_serial_mode(ctx, MODBUS_RTU_RS232);

	if (ctx == NULL)
	{
		fprintf(stderr, "Failed to create modbus context: %s\n", modbus_strerror(errno));
		modbus_free(ctx);
		return 1;
	}
	else
	{
		printf("create modbus conttext ok\n");
	}

	mb_mapping = modbus_mapping_new(10, 10, 10, 10);
	if (mb_mapping == NULL)
	{
  		fprintf(stderr, "Failed to allocate the mapping: %s\n", modbus_strerror(errno));
		modbus_free(ctx);
		return 1;
	}
	else
	{
		printf("allocate mapping ok\n");
	}

	mb_mapping->tab_registers[0] = 0xABCD;
	mb_mapping->tab_registers[1] = 0xDEAD;

	rc = modbus_connect(ctx);
	if (rc == -1)
	{
		fprintf(stderr, "Unable to connect %s\n", modbus_strerror(errno));
		modbus_free(ctx);
		return 1;
	}

	for (;;)
	{
		rc = modbus_receive(ctx, query);
		if (rc != -1)
		{
			/* rc is the query size */
			modbus_reply(ctx, query, rc, mb_mapping);
		}
		else
		{
			/* Connection closed by the client or error */
			//modbus_close(ctx);
			//modbus_tcp_accept(ctx, &socket);
			break;
		}
    	}

	printf("Quit the loop: %s\n", modbus_strerror(errno));
	modbus_mapping_free(mb_mapping);
	modbus_close(ctx);
	modbus_free(ctx);
	return 0;
}
