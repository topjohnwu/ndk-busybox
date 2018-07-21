#ifndef __DIETDNS_H_
#define __DIETDNS_H_

#define DIET_MAXDNAME     1025    /* maximum domain name */
#define DIET_PACKETSZ 512

#define	DIET_MAXNS			8	/* max # name servers we'll track */
#define	DIET_MAXDNSRCH		6	/* max # domains in search path */
#define	DIET_MAXRESOLVSORT		10	/* number of net to sort on */

struct diet_res_state {
  int   retrans;                /* retransmission time interval */
  int   retry;                  /* number of times to retransmit */
  unsigned long options;                /* option flags - see below. */
  int   nscount;                /* number of name servers */
  struct sockaddr_in
    nsaddr_list[DIET_MAXNS];   /* address of name server */
#define nsaddr  nsaddr_list[0]          /* for backward compatibility */
  unsigned short        id;                     /* current message id */
  char  *dnsrch[DIET_MAXDNSRCH+1];   /* components of domain to search */
  char  defdname[256];          /* default domain (deprecated) */
  unsigned long pfcode;                 /* RES_PRF_ flags - see below. */
  unsigned ndots:4;             /* threshold for initial abs. query */
  unsigned nsort:4;             /* number of elements in sort_list[] */
  char  unused[3];
  struct {
    struct in_addr      addr;
    uint32_t    mask;
  } sort_list[DIET_MAXRESOLVSORT];
  char  pad[72];
};

/*
 * Values for class field
 */
enum diet_ns_class {
  diet_ns_c_invalid = 0,       /* Cookie. */
  diet_ns_c_in = 1,            /* Internet. */
  diet_ns_c_2 = 2,             /* unallocated/unsupported. */
  diet_ns_c_chaos = 3,         /* MIT Chaos-net. */
  diet_ns_c_hs = 4,            /* MIT Hesiod. */
  /* Query class values which do not appear in resource records */
  diet_ns_c_none = 254,        /* for prereq. sections in update requests */
  diet_ns_c_any = 255,         /* Wildcard match. */
  diet_ns_c_max = 65536
};

#define DIET_C_IN diet_ns_c_in

/*
 * Currently defined opcodes.
 */
enum diet_ns_opcode {
  diet_ns_o_query = 0,         /* Standard query. */
  diet_ns_o_iquery = 1,        /* Inverse query (deprecated/unsupported). */
  diet_ns_o_status = 2,        /* Name server status query (unsupported). */
  /* Opcode 3 is undefined/reserved. */
  diet_ns_o_notify = 4,        /* Zone change notification. */
  diet_ns_o_update = 5,        /* Zone update message. */
  diet_ns_o_max = 6
};

#define DIET_QUERY diet_ns_o_query

/*
 * Currently defined type values for resources and queries.
 */
enum diet_ns_type {
        diet_ns_t_invalid = 0,       /* Cookie. */
        diet_ns_t_a = 1,             /* Host address. */
        diet_ns_t_ns = 2,            /* Authoritative server. */
        diet_ns_t_md = 3,            /* Mail destination. */
        diet_ns_t_mf = 4,            /* Mail forwarder. */
        diet_ns_t_cname = 5,         /* Canonical name. */
        diet_ns_t_soa = 6,           /* Start of authority zone. */
        diet_ns_t_mb = 7,            /* Mailbox domain name. */
        diet_ns_t_mg = 8,            /* Mail group member. */
        diet_ns_t_mr = 9,            /* Mail rename name. */
        diet_ns_t_null = 10,         /* Null resource record. */
        diet_ns_t_wks = 11,          /* Well known service. */
        diet_ns_t_ptr = 12,          /* Domain name pointer. */
        diet_ns_t_hinfo = 13,        /* Host information. */
        diet_ns_t_minfo = 14,        /* Mailbox information. */
        diet_ns_t_mx = 15,           /* Mail routing information. */
        diet_ns_t_txt = 16,          /* Text strings. */
        diet_ns_t_rp = 17,           /* Responsible person. */
        diet_ns_t_afsdb = 18,        /* AFS cell database. */
        diet_ns_t_x25 = 19,          /* X_25 calling address. */
        diet_ns_t_isdn = 20,         /* ISDN calling address. */
        diet_ns_t_rt = 21,           /* Router. */
        diet_ns_t_nsap = 22,         /* NSAP address. */
        diet_ns_t_nsap_ptr = 23,     /* Reverse NSAP lookup (deprecated). */
        diet_ns_t_sig = 24,          /* Security signature. */
        diet_ns_t_key = 25,          /* Security key. */
        diet_ns_t_px = 26,           /* X.400 mail mapping. */
        diet_ns_t_gpos = 27,         /* Geographical position (withdrawn). */
        diet_ns_t_aaaa = 28,         /* Ip6 Address. */
        diet_ns_t_loc = 29,          /* Location Information. */
        diet_ns_t_nxt = 30,          /* Next domain (security). */
        diet_ns_t_eid = 31,          /* Endpoint identifier. */
        diet_ns_t_nimloc = 32,       /* Nimrod Locator. */
        diet_ns_t_srv = 33,          /* Server Selection. */
        diet_ns_t_atma = 34,         /* ATM Address */
        diet_ns_t_naptr = 35,        /* Naming Authority PoinTeR */
        diet_ns_t_kx = 36,           /* Key Exchange */
        diet_ns_t_cert = 37,         /* Certification record */
        diet_ns_t_a6 = 38,           /* IPv6 address (deprecates AAAA) */
        diet_ns_t_dname = 39,        /* Non-terminal DNAME (for IPv6) */
        diet_ns_t_sink = 40,         /* Kitchen sink (experimentatl) */
        diet_ns_t_opt = 41,          /* EDNS0 option (meta-RR) */
        diet_ns_t_tsig = 250,        /* Transaction signature. */
        diet_ns_t_ixfr = 251,        /* Incremental zone transfer. */
        diet_ns_t_axfr = 252,        /* Transfer zone of authority. */
        diet_ns_t_mailb = 253,       /* Transfer mailbox records. */
        diet_ns_t_maila = 254,       /* Transfer mail agent records. */
        diet_ns_t_any = 255,         /* Wildcard match. */
        diet_ns_t_zxfr = 256,        /* BIND-specific, nonstandard. */
        diet_ns_t_max = 65536
};

#define DIET_T_A             diet_ns_t_a
#define DIET_T_NS            diet_ns_t_ns
#define DIET_T_CNAME         diet_ns_t_cname
#define DIET_T_SOA           diet_ns_t_soa
#define DIET_T_PTR           diet_ns_t_ptr
#define DIET_T_MX            diet_ns_t_mx
#define DIET_T_TXT           diet_ns_t_txt
#define DIET_T_AAAA          diet_ns_t_aaaa
#define DIET_T_ANY           diet_ns_t_any

/*
 * Currently defined response codes.
 */
enum diet_ns_rcode {
        diet_ns_r_noerror = 0,       /* No error occurred. */
        diet_ns_r_formerr = 1,       /* Format error. */
        diet_ns_r_servfail = 2,      /* Server failure. */
        diet_ns_r_nxdomain = 3,      /* Name error. */
        diet_ns_r_notimpl = 4,       /* Unimplemented. */
        diet_ns_r_refused = 5,       /* Operation refused. */
        /* these are for BIND_UPDATE */
        diet_ns_r_yxdomain = 6,      /* Name exists */
        diet_ns_r_yxrrset = 7,       /* RRset exists */
        diet_ns_r_nxrrset = 8,       /* RRset does not exist */
        diet_ns_r_notauth = 9,       /* Not authoritative for zone */
        diet_ns_r_notzone = 10,      /* Zone of record different from zone section */
        diet_ns_r_max = 11,
        /* The following are TSIG extended errors */
        diet_ns_r_badsig = 16,
        diet_ns_r_badkey = 17,
        diet_ns_r_badtime = 18
};



#define DIET_NOERROR         diet_ns_r_noerror
#define DIET_FORMERR         diet_ns_r_formerr
#define DIET_SERVFAIL        diet_ns_r_servfail
#define DIET_NXDOMAIN        diet_ns_r_nxdomain
#define DIET_NOTIMP          diet_ns_r_notimpl
#define DIET_REFUSED         diet_ns_r_refused
#define DIET_YXDOMAIN        diet_ns_r_yxdomain
#define DIET_YXRRSET         diet_ns_r_yxrrset
#define DIET_NXRRSET         diet_ns_r_nxrrset
#define DIET_NOTAUTH         diet_ns_r_notauth
#define DIET_NOTZONE         diet_ns_r_notzone


#define DIET_RES_RECURSE	0x00000040	/* recursion desired */

extern struct diet_res_state _diet_res;
extern int diet_h_errno;

int __dns_gethostbyx_r(const char* name, struct hostent* result,
			char *buf, size_t buflen,
			struct hostent **RESULT, int *h_errnop, int lookfor);
int diet_gethostbyname2_r(const char* name, int AF, struct hostent* result,
				char *buf, size_t buflen,
				struct hostent **RESULT, int *h_errnop);
int diet_res_query(const char *dname, int class, int type, unsigned char *answer, int anslen);
int diet_res_mkquery(int op, const char *dname, int class, int type, char* data,
        int datalen, const unsigned char* newrr, char* buf, int buflen);
const char* diet_gai_strerror(int error);
int diet_getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);
void diet_freeaddrinfo(struct addrinfo *res);
int diet_res_init(void);
int diet_dn_expand(const unsigned char *msg, const unsigned char *eomorig, const unsigned char *comp_dn, unsigned char *exp_dn, int length);

#endif
