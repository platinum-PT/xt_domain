# ipt_domain

A netfilter module, use can use iptables to match DNS domain.

## Install

    # make KERNEL_SRC=<your kernel source> IPTABLES_SRC=<your iptables source> install

## Usage

    # iptables -m domain -h
    domain v0.0.3 options:
      --name "www.chinaunix.net"            Match the domain named "www.chinaunix.net"
      --name "chinaunix.net"                Match the domain named "chinaunix.net"
                                            include www.chinunix.net, bbs.chinaunix.net, man.chinaunix.net
## Version supporting

kernel: >= 2.6.38

iptables: >= 1.4.11
