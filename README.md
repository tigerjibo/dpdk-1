# DPDK開発環境 on Vagrant 

vagrant上でDPDKを使った開発を行うときの環境一式。どのホストでも動くようにする。
開発を開始できる状態まで以下の手順をふむ


## 1. 必要なパッケージをインストール

パッケージ管理システムで以下のパッケージをインストールする

 - GNU make
 - coreutils: cmp, sed, grep, arch, etc..
 - gcc: version >=4.5.x
 - libc headers
 - Linux Kernel Headers (pacmanではlinux-headersというパッケージ)


## 2. カーネルコンフィグを調べる

vm/check_kernel_config.shを実行して、必要なカーネルコンフィグが有効になっているか、
またはモジュールとして組み込まれているかを調べる。

```
$ cd /vagrant
$ sh check_kernel_config.sh
Checking Kernel Config...
 - UIO is supported
 - HUGETLBFS is supported
 - PROC_PAGE_MONITOR is supported
 - HPET is supported
 - HPET_MMAP is supported
OK!
```


## 3. Hugepagesの有効化

grubの設定を編集してカーネルパラメータを追加する。
``/etc/default/grub``を編集してgrub-mkconfigをするだけ。

```
$ sudo vim /etc/default/grub
GRUB_CMDLINE_LINUX=""  #ここを
GRUB_CMDLINE_LINUX="hugepages=1024"  #こうする

$ sudo grub-mkconfig -o /boot/grub/grub.cfg
```

これで次回起動時にHugepagesが有効になる。

## 4. HugepagesをDPDKから触れるようにする

Hugetlbfsをマウントする。

```
$ mkdir -p /mnt/huge
$ sudo vim /etc/fstab
...末尾に追記
nodev /mnt/huge hugetlbfs defaults 0 0
```

これで次回起動時にHugepagesを触るためのHugetlbfsがマウントされる。



