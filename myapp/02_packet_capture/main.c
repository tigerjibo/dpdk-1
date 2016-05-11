
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <rte_version.h>
#include <rte_ethdev.h>
#include <rte_ether.h>

#define DEBUG(s) fprintf(stderr, "[%s:%d] %s\n", __func__, __LINE__, s)


static void print_info_port(int port_id)
{

	if (!rte_eth_dev_is_valid_port(port_id))
		rte_exit(EXIT_FAILURE, "rte_eth_dev_is_valid_port() failed\n");

    printf("-PortID:%d--------------------\n", port_id);

	struct rte_eth_dev_info dev_info;
	memset(&dev_info, 0, sizeof(dev_info));
	rte_eth_dev_info_get(port_id, &dev_info);
	printf("PCI Info: %04x:%02x:%02x.%x \n",
		dev_info.pci_dev->addr.domain, dev_info.pci_dev->addr.bus,
		dev_info.pci_dev->addr.devid, dev_info.pci_dev->addr.function);


    struct ether_addr addr;
	rte_eth_macaddr_get(port_id, &addr);

    printf("HW Addr : ");
    int i;
    for (i=0; i<6; i++) {
        printf("%02x", addr.addr_bytes[i]);
        if (i<5) printf(":");
    }
    printf("\n");
    printf("-----------------------------\n");
    
}


static void print_info_all(void)
{
    uint32_t num_ports = rte_eth_dev_count();
    printf("%u NICs found. \n", num_ports);

    uint32_t i;
    for (i=0; i<num_ports; i++) {
        print_info_port(i);
    }
}


int main(int argc, char** argv)
{

	int ret = rte_eal_init(argc, argv);
	if (ret < 0)
		rte_exit(EXIT_FAILURE, "rte_eal_init() failed\n");

    printf("\n\n\n");
    DEBUG("start");
    print_info_all();
    DEBUG("finish");


    return 0;
}
