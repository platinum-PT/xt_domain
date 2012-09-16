#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/udp.h>

#include <linux/netfilter/x_tables.h>
#include "xt_domain.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChinaUnix - Platinum <cu.platinum@gmail.com>, http://weibo.com/bjpt");
MODULE_DESCRIPTION("Xtables: DNS address match");
MODULE_ALIAS("ipt_domain");
MODULE_VERSION("0.0.3");

static bool
domain (const struct sk_buff *skb, struct xt_action_param *par)
{
        const struct xt_domain_info *info = par->matchinfo;
        struct iphdr *iph = ip_hdr(skb);
        struct udphdr *udph, _udph;

        if (iph->protocol != IPPROTO_UDP)
                return false;

        udph = skb_header_pointer(skb, iph->ihl << 2, sizeof(_udph), &_udph);
        if (udph == NULL)
                return false;

        return ((udph->len >= __constant_htons(8 + 12 + info->len)) &&
                !memcmp(info->name, (void *)udph + 8 + 12, info->len));

}

static __read_mostly struct xt_match domain_match= {
        .name		= "domain",
        .revision	= 0,
        .family		= NFPROTO_IPV4,
        .match		= domain,
        .matchsize	= sizeof(struct xt_domain_info),
        .me		= THIS_MODULE,
};

static int __init init(void)
{
        return xt_register_match(&domain_match);
}

static void __exit fini(void)
{
        xt_unregister_match(&domain_match);
}

module_init(init);
module_exit(fini);
