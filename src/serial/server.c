#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <modbus/modbus.h>

int main(int argc, char *argv[])
{
	uint8_t query[MODBUS_RTU_MAX_ADU_LENGTH];
	int rc;
	modbus_t *ctx;
	modbus_mapping_t *mb_mapping;

	if (argc != 2)
	{
		printf("name serial port\n");
		exit(1);
	}

	ctx = modbus_new_rtu(argv[1], 9600, 'N', 8, 1);
	modbus_set_slave(ctx, 50);
	if (ctx == NULL)
	{
  		fprintf(stderr, "Failed to create modbus context: %s\n", modbus_strerror(errno));
		modbus_free(ctx);
		return -1;
	}

	mb_mapping = modbus_mapping_new(500, 500, 500, 500);
	if (mb_mapping == NULL)
	{
  		fprintf(stderr, "Failed to allocate the mapping: %s\n", modbus_strerror(errno));
		modbus_free(ctx);
		return -1;
	}

	rc = modbus_connect(ctx);
	if (rc == -1)
	{
  		fprintf(stderr, "Failed to connect: %s\n", modbus_strerror(errno));
		modbus_free(ctx);
		return -1;
	}

	for (;;)
	{
		rc = modbus_receive(ctx, query);
		if (rc != -1)
		{
			/* rc is the query size */
			if (modbus_reply(ctx, query, rc, mb_mapping) == -1)
				break;
		}
		else
		{
			/* Connection closed by the client or error */
			modbus_close(ctx);
			//rc = modbus_connect(ctx);
			break;
		}
    	}

	printf("Quit the loop: %s\n", modbus_strerror(errno));
	modbus_mapping_free(mb_mapping);
	modbus_close(ctx);
	modbus_free(ctx);
	return 0;
}
