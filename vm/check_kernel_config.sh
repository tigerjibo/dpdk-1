
echo "Checking Kernel Config... "

(zcat /proc/config.gz  | grep CONFIG_UIO=m) >> /dev/null || (zcat /proc/config.gz | grep CONFIG_UIO=y) >> /dev/null
if [ $? -ne 0 ]; then
	echo " - UIO is not supported"
	exit -1
fi
echo " - UIO is supported"


zcat /proc/config.gz | grep CONFIG_HUGETLBFS=y >> /dev/null
if [ $? -ne 0 ]; then
	echo " - HUGETLBFS is not supported"
	exit -1
fi
echo " - HUGETLBFS is supported"


zcat /proc/config.gz | grep CONFIG_PROC_PAGE_MONITOR=y >> /dev/null
if [ $? -ne 0 ]; then
	echo " - PROC_PAGE_MONITOR is not supported"
	exit -1
fi
echo " - PROC_PAGE_MONITOR is supported"


zcat /proc/config.gz | grep CONFIG_HPET=y >> /dev/null
if [ $? -ne 0 ]; then
	echo " - HPET is not supported"
	exit -1
fi
echo " - HPET is supported"


zcat /proc/config.gz | grep CONFIG_HPET_MMAP=y >> /dev/null
if [ $? -ne 0 ]; then
	echo " - HPET_MMAP is not supported"
	exit -1
fi
echo " - HPET_MMAP is supported"

echo "OK!"
