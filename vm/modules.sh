

# modprobe uio_pci_generic
modprobe uio
insmod /home/vagrant/Desktop/dpdk/x86_64-native-linuxapp-gcc/kmod/igb_uio.ko
insmod /home/vagrant/Desktop/dpdk/x86_64-native-linuxapp-gcc/kmod/rte_kni.ko

modprobe vfio-pci
