
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#include <rte_version.h>
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_ether.h>
#include <rte_cycles.h>
#include <rte_lcore.h>
#include <rte_mbuf.h>


#define DEBUG(s) fprintf(stderr, "[%s:%d] %s\n", __func__, __LINE__, s)

#define NUM_MBUFS 8191
#define MBUF_CACHE_SIZE 250
#define BURST_SIZE 32



static int port_init(uint8_t port, struct rte_mempool* mbuf_pool)
{
    printf("port number: %d \n", port);
    if (mbuf_pool == NULL) 
        exit(-1);
    return 0;
}



int main(int argc, char** argv)
{

	int ret = rte_eal_init(argc, argv);
	if (ret < 0)
		rte_exit(EXIT_FAILURE, "rte_eal_init() failed\n");

    printf("\n\n\n");
    DEBUG("start");

    uint32_t num_ports = rte_eth_dev_count();
    printf("%d ports found  \n", num_ports);
    if (num_ports < 1)
        rte_exit(EXIT_FAILURE, "rte_eth_dev_count()");
    
    struct rte_mempool* mbuf_pool = rte_pktmbuf_pool_create("MBUF_POOL", NUM_MBUFS * num_ports, 
            MBUF_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());  
    if (mbuf_pool == NULL)
        rte_exit(EXIT_FAILURE, "rtembuf_pool_create()");

    uint8_t portid;
    for (portid=0; portid < num_ports; portid++) {
        if (port_init(portid, mbuf_pool) != 0)
            rte_exit(EXIT_FAILURE, "Cannot init port %"PRIu8 "\n", portid);
    }

    if (rte_lcore_count() > 1) {
        printf("WARNING: Too many lcores enabled. Only 1 used. \n");
    }

    DEBUG("finish");
    return 0;
}




