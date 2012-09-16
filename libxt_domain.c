#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <xtables.h>
#include "xt_domain.h"

static void help(void)
{
	printf(
	"domain v%s options:\n"
	"  --name \"www.chinaunix.net\"            Match the domain named \"www.chinaunix.net\"\n"
	"  --name \"chinaunix.net\"                Match the domain named \"chinaunix.net\"\n"
	"                                        include www.chinunix.net, bbs.chinaunix.net, man.chinaunix.net\n",
	DOMAIN_VERSION);
}

static struct option opts[] = {
	{ "name", 1, 0, '1' },
	{ 0 }
};

static void
parse_name (char *name, struct xt_domain_info *info)
{
	int i, char_len = 0;
	int len = strlen(name);

	if (len + 1 > MAX_CHAR)
		xtables_error(PARAMETER_PROBLEM,
			   "The strings `%s' is too long (MAX_CHAR = %d",
			   name, MAX_CHAR);

	memcpy(info->name + 1, name, len);
	memcpy(info->org_name, name, len);
	info->name[0] = '.';

	for (i = len; i >= 0; i--) {
		if (info->name[i] == '.') {
			info->name[i] = char_len;
			char_len = 0;
		} else {
			char_len++;
		}
	}

	/* 还包括一个前缀 "." */
	info->len = len + 1;
}

static int
parse (int c, char **argv, int invert, unsigned int *flags,
       const void *entry, struct xt_entry_match **match)
{
	struct xt_domain_info *info = (struct xt_domain_info *)(*match)->data;

	xtables_check_inverse(optarg, &invert, &optind, 0, argv);

	switch (c) {
		case '1':
			parse_name(argv[optind - 1], info);
			*flags = 1;
			break;

		default:
			return 0;
	}

	return 1;
}

static void
final_check (unsigned int flags)
{
	if (!flags) xtables_error(PARAMETER_PROBLEM, "domain expection an option");
}

static void
print (const void *ip, const struct xt_entry_match *match, int numeric)
{
	const struct xt_domain_info *info = (const struct xt_domain_info *)match->data;

	printf("domain --name \"%s\"", info->org_name);
}

static void
save (const void *ip, const struct xt_entry_match *match)
{

	const struct xt_domain_info *info = (const struct xt_domain_info *)match->data;

	printf("--name \"%s\" ", info->org_name);
}

static struct xtables_match domain = {
	.name		= "domain",
	.version	= XTABLES_VERSION,
	.family		= NFPROTO_IPV4,
	.size		= XT_ALIGN(sizeof(struct xt_domain_info)),
	.userspacesize	= XT_ALIGN(sizeof(struct xt_domain_info)),
	.help		= help,
	.parse		= parse,
	.final_check	= final_check,
	.print		= print,
	.save		= save,
	.extra_opts	= opts
};

void
_init (void)
{
	xtables_register_match(&domain);
}
