

modprobe uio_pci_generic
# modprobe uio
# insmod ${RTE_SDK}/${RTE_TARGET}/kmod/igb_uio.ko
# insmod ${RTE_SDK}/${RTE_TARGET}/kmod/rte_kni.ko

modprobe vfio-pci
