
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

#define RX_RING_SIZE 128
#define TX_RING_SIZE 512

#define NUM_MBUFS       8191
#define MBUF_CACHE_SIZE 250
#define BURST_SIZE      32


static const struct rte_eth_conf port_conf_default = {
    .rxmode = { .max_rx_pkt_len = ETHER_MAX_LEN }
};

static struct {
	uint64_t total_cycles;
	uint64_t total_pkts;
} latency_numbers;


static uint16_t add_timestamps(uint8_t port __rte_unused, uint16_t qidx __rte_unused,
        struct rte_mbuf **pkts, uint16_t nb_pkts,
        uint16_t max_pkts __rte_unused, void *_ __rte_unused)
{
	unsigned i;
	uint64_t now = rte_rdtsc();

	for (i = 0; i < nb_pkts; i++) {
		pkts[i]->udata64 = now;
    }
	return nb_pkts;
}

static uint16_t calc_latency(uint8_t port __rte_unused, uint16_t qidx __rte_unused,
        struct rte_mbuf **pkts, uint16_t nb_pkts, void *_ __rte_unused)
{
	uint64_t cycles = 0;
	uint64_t now = rte_rdtsc();
	unsigned i;

	for (i = 0; i < nb_pkts; i++) {
		cycles += now - pkts[i]->udata64;
    }
	latency_numbers.total_cycles += cycles;
	latency_numbers.total_pkts += nb_pkts;

	if (latency_numbers.total_pkts > (1000 * 1000ULL)) {
        /* printf("Latency = %lu cycles\n", */
        /*         latency_numbers.total_cycles / latency_numbers.total_pkts); */
        latency_numbers.total_cycles = latency_numbers.total_pkts = 0;
	}
	return nb_pkts;
}


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
        struct rte_mbuf* bufs[BURST_SIZE];
        for (port=0; port<num_ports; port++) {
            const uint16_t num_rx = rte_eth_rx_burst(port, 0, bufs, BURST_SIZE);

            if (unlikely(num_rx == 0))
                continue;

            const uint16_t num_tx = rte_eth_tx_burst(port, 0, bufs, num_rx);

            if (unlikely(num_tx < num_rx)) {
                uint16_t buf;
                for (buf=num_tx; buf<num_rx; buf++)
                    rte_pktmbuf_free(bufs[buf]);
            }
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
	rte_eth_add_rx_callback(port, 0, add_timestamps, NULL);
	rte_eth_add_tx_callback(port, 0, calc_latency, NULL);
    return 0;
}


int main(int argc, char** argv)
{
	int ret = rte_eal_init(argc, argv);
	if (ret < 0)
	    rte_exit(EXIT_FAILURE, "rte_eal_init() failed\n");

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

    lcore_main();
    return 0;
}




