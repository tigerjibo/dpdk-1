
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#include <rte_config.h>
#include <rte_version.h>
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_ether.h>
#include <rte_cycles.h>
#include <rte_lcore.h>
#include <rte_mbuf.h>
#include <rte_hexdump.h>


#define DEBUG(s) fprintf(stderr, "[%s:%d] %s\n", __func__, __LINE__, s)

#define RX_RING_SIZE 128
#define TX_RING_SIZE 512

#define NUM_MBUFS       8191
#define MBUF_CACHE_SIZE 250
#define BURST_SIZE      32

// #ifdef __cplusplus
// extern "C" {
// #endif
//
// static const struct rte_eth_conf port_conf_default = {
//     .rxmode = { .max_rx_pkt_len = ETHER_MAX_LEN }
// };
//
// #ifdef __cplusplus
// }
// #endif


#if 0
static __attribute((noreturn)) void lcore_main(void)
{
    const uint8_t num_ports = rte_eth_dev_count();

    uint8_t port;
    for (port = 0; port < num_ports; port++) {
        if (rte_eth_dev_socket_id(port) > 0 && rte_eth_dev_socket_id(port) != (int)rte_socket_id())
            printf("WARNING: port %u is on remote NUMA node to "
                    "polling thread. \n\tPerformance will "
                    "not be optimal. \n ", port);
    }
    printf("\nCore %u forwarding packets. [Ctrl+C to quit]\n", rte_lcore_id());


    for (;;) {
        
        for (port=0; port<num_ports; port++) {
            struct rte_mbuf* bufs[BURST_SIZE];
            const uint16_t num_rx = rte_eth_rx_burst(port, 0, bufs, BURST_SIZE);

            if (unlikely(num_rx == 0))
                continue;

            uint16_t i;
            for (i=0; i<num_rx; i++) {
                rte_pktmbuf_dump(stdout, bufs[i], sizeof(struct rte_mbuf));
                /* rte_hexdump(stdout, "recv packet", */
                /*         rte_pktmbuf_mtod(bufs[i], void*) , */
                /*         rte_pktmbuf_data_len(bufs[i])     ); */
                uint8_t* head = rte_pktmbuf_mtod(bufs[i], uint8_t*);
                memset(head , 0xee, 6);
            }


            const uint16_t num_tx = 0;
            /* const uint16_t num_tx = rte_eth_tx_burst(port, 0, bufs, num_rx); */
            /* printf("Reflect %d packet !! \n", num_rx); */
            if (unlikely(num_tx < num_rx)) {
                uint16_t buf;
                for (buf=num_tx; buf<num_rx; buf++)
                    rte_pktmbuf_free(bufs[buf]);
            }

            printf("\n\n");

        }
    }
}



static int port_init(uint8_t port, struct rte_mempool* mbuf_pool)
{

    struct rte_eth_conf port_conf = port_conf_default;
    int ret;
    uint16_t q;

    if (port >= rte_eth_dev_count())
        return -1;

    const uint16_t rx_rings = 1;
    const uint16_t tx_rings = 1;
    ret = rte_eth_dev_configure(port, rx_rings, tx_rings, &port_conf);
    if (ret != 0)
        return ret;

    for (q=0; q < rx_rings; q++) {
        ret = rte_eth_rx_queue_setup(port, q, RX_RING_SIZE, 
                rte_eth_dev_socket_id(port), NULL, mbuf_pool);
        if (ret < 0)
            return ret;
    }

    for (q=0; q < tx_rings; q++) {
        ret = rte_eth_tx_queue_setup(port, q, TX_RING_SIZE, 
                rte_eth_dev_socket_id(port), NULL);
        if (ret < 0)
            return ret;
    }

    ret = rte_eth_dev_start(port);
    if (ret < 0)
        return ret;

    struct ether_addr addr;
    rte_eth_macaddr_get(port, &addr);
    printf("Port %u MAC: %02" PRIx8 " %02" PRIx8 " %02" PRIx8
            " %02" PRIx8 " %02" PRIx8 " %02" PRIx8 "\n",
            (unsigned)port,
            addr.addr_bytes[0], addr.addr_bytes[1],
            addr.addr_bytes[2], addr.addr_bytes[2],
            addr.addr_bytes[3], addr.addr_bytes[4]);

	rte_eth_promiscuous_enable(port);

    return 0;
}
#endif


int main(int argc, char** argv)
{

	int ret = rte_eal_init(argc, argv);
	if (ret < 0)
		rte_exit(EXIT_FAILURE, "rte_eal_init() failed\n");

    uint32_t num_ports = rte_eth_dev_count();
    printf("%d ports found  \n", num_ports);
    if (num_ports < 1)
        rte_exit(EXIT_FAILURE, "rte_eth_dev_count()");
    //
    // struct rte_mempool* mbuf_pool = rte_pktmbuf_pool_create(
    //         "MBUF_POOL_SLANK", NUM_MBUFS * num_ports, 
    //         MBUF_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());  
    //
    // if (mbuf_pool == NULL)
    //     rte_exit(EXIT_FAILURE, "rtembuf_pool_create()");
    //
    // uint8_t portid;
    // for (portid=0; portid < num_ports; portid++) {
    //     if (port_init(portid, mbuf_pool) != 0)
    //         rte_exit(EXIT_FAILURE, "Cannot init port %"PRIu8 "\n", portid);
    // }
    //
    // if (rte_lcore_count() > 1) 
    //     printf("WARNING: Too many lcores enabled. Only 1 used. \n");
    //
    // lcore_main();
    //
    // rte_log(RTE_LOG_DEBUG, RTE_LOGTYPE_EAL, "test test \n");
    //
    // DEBUG("finish");
    // return 0;
}



