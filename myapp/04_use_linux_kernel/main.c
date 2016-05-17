
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include <rte_cycles.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <net/if.h>

#include <netpacket/packet.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>
#include <sys/socket.h>


#define DEBUG(s) fprintf(stderr, "[%s:%d] %s\n", __func__, __LINE__, s)


const char* dev = "enp3s0";





/* int main(int argc, char** argv) */
int main(void)
{

    struct ifreq ifreq;
    struct sockaddr_ll sa;

    int fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (fd < 0) {
        perror("socket");
        exit(-1);
    }
    DEBUG("opened socket");

    memset(&ifreq, 0, sizeof(ifreq));
    strncpy(ifreq.ifr_name, dev, sizeof(ifreq.ifr_name)-1);
    int res = ioctl(fd, SIOCGIFINDEX, &ifreq);
    if (res < 0) {
        perror("ioctl");
        exit(-1);
    }
    DEBUG("get ifindex");

    sa.sll_family = AF_PACKET;
    sa.sll_protocol = htonl(ETH_P_ALL);
    sa.sll_ifindex = ifreq.ifr_ifindex;
    res = bind(fd, (struct sockaddr*)&sa, sizeof(sa));
    if (res < 0) {
        perror("bind");
        exit(-1);
    }
    DEBUG("binded to device");

    res = ioctl(fd, SIOCGIFFLAGS, &ifreq);
    if (res < 0) {
        perror("ioctl");
        exit(-1);
    }
    ifreq.ifr_flags = ifreq.ifr_flags | IFF_PROMISC;
    res = ioctl(fd, SIOCSIFFLAGS, &ifreq);
    if (res < 0) {
        perror("ioctl");
        exit(-1);
    }
    DEBUG("set promisc");

    printf("fd is ready %d \n", fd);
    /* uint8_t buf[1000]; */
    /* memset(buf, 0xff, sizeof(buf)); */
    /* res = write(fd, buf, sizeof(buf)); */
    /* if (res < 0) { */
    /*     perror("write"); */
    /*     exit(-1); */
    /* } */
    
    int count;
    for (count=0; ; count++) {
        
        /* const uint16_t num_rx = rte_eth_rx_burst(port, 0, bufs, BURST_SIZE); */
        printf("test \n");
        uint8_t buf[1000];
        res = read(fd, buf, sizeof(buf));
        printf("test \n");
        if (res < 0) {
            perror("read");
            exit(-1);
        }
	    uint64_t before = rte_rdtsc();
        printf("recv \n");

        /* const uint16_t num_tx = rte_eth_tx_burst(port, 0, bufs, num_rx); */
        res = write(fd, buf, res);
        if (res < 0) {
            perror("write");
            exit(-1);
        }
	    uint64_t after = rte_rdtsc();
        printf("send \n");

        printf("%d: micro ltncy : %lu \n", count, after-before);

    }

    return 0;
}
