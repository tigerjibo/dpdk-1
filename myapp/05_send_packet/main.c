
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
#include <rte_hexdump.h>


#define DEBUG(s) fprintf(stderr, "[%s:%d] %s\n", __func__, __LINE__, s)

#define RX_RING_SIZE 128
#define TX_RING_SIZE 512

#define NUM_MBUFS       8191
#define MBUF_CACHE_SIZE 250
#define BURST_SIZE      32


static const struct rte_eth_conf port_conf_default = {
    .rxmode = { .max_rx_pkt_len = ETHER_MAX_LEN }
};


static __attribute((noreturn)) void lcore_main(struct rte_mempool* mbuf_pool)
{
    uint8_t raw[] = { 
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x09, 0x0f, 0x09, 0x00, 0x0d, 0x08, 0x06, 0x00, 0x01,
        0x08, 0x00, 0x06, 0x04, 0x00, 0x01, 0x00, 0x09, 0x0f, 0x09, 0x00, 0x0d, 0x0a, 0xd2, 0x7c, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0xd2, 0x7c, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    const uint8_t num_ports = rte_eth_dev_count();

    uint8_t port;
    for (port = 0; port < num_ports; port++) {
        if (rte_eth_dev_socket_id(port) > 0 && rte_eth_dev_socket_id(port) != (int)rte_socket_id())
            printf("WARNING: port %u is on remote NUMA node to "
                    "polling thread. \n\tPerformance will "
                    "not be optimal. \n ", port);
    }
    printf("\nCore %u Sending packets. [Ctrl+C to quit]\n", rte_lcore_id());


    struct rte_mbuf* send_buffer = rte_pktmbuf_alloc(mbuf_pool);
    if (send_buffer == NULL) {
		rte_exit(EXIT_FAILURE, "rte_pktmbuf_alloc() failed\n");
    }

    /* printf("print send_buffer's room size: %d \n", */
    /*         rte_pktmbuf_data_room_size(mbuf_pool)); */


    memcpy(rte_pktmbuf_mtod(send_buffer, void*), raw, sizeof raw);
    send_buffer->nb_segs = 1;
    send_buffer->next = NULL;
    send_buffer->pkt_len  = (uint16_t)sizeof(raw);
    send_buffer->data_len = (uint16_t)sizeof(raw);

    rte_pktmbuf_dump(stdout, send_buffer, sizeof(struct rte_mbuf));

    for (;;) {

        for (port=0; port<num_ports; port++) {
            const uint16_t num_tx = rte_eth_tx_burst(port, 0, &send_buffer, 1);
            if (num_tx < 1) {
                printf("failed sending \n");
            }
            printf("send %u packet \n", num_tx);
        }
        exit(-1);

    }
    rte_pktmbuf_free(send_buffer);
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
    
    struct rte_mempool* mbuf_pool = rte_pktmbuf_pool_create(
            "MBUF_POOL_SLANK", NUM_MBUFS * num_ports, 
            MBUF_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());  

    if (mbuf_pool == NULL)
        rte_exit(EXIT_FAILURE, "rtembuf_pool_create()");

    uint8_t portid;
    for (portid=0; portid < num_ports; portid++) {
        if (port_init(portid, mbuf_pool) != 0)
            rte_exit(EXIT_FAILURE, "Cannot init port %"PRIu8 "\n", portid);
    }

    if (rte_lcore_count() > 1) 
        printf("WARNING: Too many lcores enabled. Only 1 used. \n");

    lcore_main(mbuf_pool);

    return 0;
}



