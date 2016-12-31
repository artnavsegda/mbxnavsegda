#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <modbus/modbus.h>

int main(void)
{
	uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
	int rc;
	int socket;
	modbus_t *ctx;
	modbus_mapping_t *mb_mapping;

	ctx = modbus_new_tcp("127.0.0.1", 1100);

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

	mb_mapping = modbus_mapping_new(500, 500, 500, 500);
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

	socket = modbus_tcp_listen(ctx, 1);
	if (socket == -1)
	{
		fprintf(stderr, "Failed to create socket: %s\n", modbus_strerror(errno));
		modbus_free(ctx);
	}
	else
	{
		printf("socket create ok\n");
	}

	if (modbus_tcp_accept(ctx, &socket) == -1)
	{
		fprintf(stderr, "Failed to accept client: %s\n", modbus_strerror(errno));
		close(socket);
		modbus_free(ctx);
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
