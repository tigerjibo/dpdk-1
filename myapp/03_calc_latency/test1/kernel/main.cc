

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include <slankdev.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <net/if.h>

#include <netpacket/packet.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>
#include <sys/socket.h>



#define DEBUG(s) fprintf(stderr, "[%s:%d] %s\n", __func__, __LINE__, s)

static struct {
    uint64_t total_cycles;
    uint64_t total_pkts;
} latency_numbers;

const char* dev = "enp3s0"; // IntelNIC



int main(int argc, char** argv)
{

    intfd fd;
    fd.socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    struct ifreq ifreq;
    memset(&ifreq, 0, sizeof(ifreq));
    strncpy(ifreq.ifr_name, dev, sizeof(ifreq.ifr_name)-1);
    fd.ioctl(SIOCGIFINDEX, &ifreq);

    struct sockaddr_ll sa;
    sa.sll_family = AF_PACKET;
    sa.sll_protocol = htonl(ETH_P_ALL);
    sa.sll_ifindex = ifreq.ifr_ifindex;
    fd.bind((struct sockaddr*)&sa, sizeof(sa));

    fd.ioctl(SIOCGIFFLAGS, &ifreq);
    ifreq.ifr_flags = ifreq.ifr_flags | IFF_PROMISC;
    fd.ioctl(SIOCSIFFLAGS, &ifreq);

    int count;
    for (count=0; ; count++) {
        
        uint8_t buf[1000];
        size_t res = fd.read(buf, sizeof(buf));
	    uint64_t before = rdtsc();

        fd.write(buf, res);
	    uint64_t after = rdtsc();
        latency_numbers.total_cycles += after - before;
        latency_numbers.total_pkts   += 1;

        if (latency_numbers.total_pkts > (1000 * 1000ULL)) {
            printf("Latency = %lu cycles\n",
                    latency_numbers.total_cycles / latency_numbers.total_pkts);
            latency_numbers.total_cycles = latency_numbers.total_pkts = 0;
        }

    }
    return 0;
}


