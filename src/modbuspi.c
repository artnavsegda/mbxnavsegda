#include <stdio.h>
#include <stdlib.h>
#include <modbus.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	int socket;
	modbus_t *mb;
	modbus_mapping_t *mb_mapping;
	uint16_t tab_reg[256];
	int rc;
	int i;

	if (argc == 1)
	{
		printf("name ip adress\n");
		exit(1);
	}

	mb = modbus_new_tcp(argv[1], 502);

	if (modbus_connect(mb) == -1)
	{
		fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
		modbus_free(mb);
		return -1;
	}

	socket = modbus_tcp_listen(mb, 1);

	modbus_tcp_accept(mb, &socket);

	mb_mapping = modbus_mapping_new(MODBUS_MAX_READ_BITS, 0,MODBUS_MAX_READ_REGISTERS, 0);

	if (mb_mapping == NULL)
	{
		fprintf(stderr, "Failed to allocate the mapping: %s\n", modbus_strerror(errno));
		modbus_free(mb);
		return -1;
	}

	while(1)
	{
		printf("lol\n");
		uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
		rc = modbus_receive(mb, query);
		if (rc >= 0)
		{
			printf("Replying to request.\n");
			modbus_reply(mb, query, rc, mb_mapping);
		} 
		else 
		{
				printf("Connection closed by the client or server\n");
				break;
		}
	}

	/* Read 5 registers from the address 10 */
	rc = modbus_read_registers(mb, 10, 40, tab_reg);
	if (rc == -1)
	{
		fprintf(stderr, "%s\n", modbus_strerror(errno));
		return -1;
	}

	for (i=0; i < rc; i=i+2) {
		printf("reg[%d]=%f (0x%X)\n", i, modbus_get_float(&tab_reg[i]), tab_reg[i]);
	}

	modbus_close(mb);
	modbus_free(mb);
}
