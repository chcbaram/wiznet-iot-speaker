#include "wiznet.h"


/* Socket */
#define SOCKET_DHCP       1
#define SOCKET_DNS        2

/* Retry count */
#define DHCP_RETRY_COUNT  5
#define DNS_RETRY_COUNT   5

#define ETHERNET_BUF_MAX_SIZE (1024 * 2)


static void wiznetPrintInfo(wiz_NetInfo *p_info);

/* DHCP */
static void wizchip_dhcp_init(void);
static void wizchip_dhcp_assign(void);
static void wizchip_dhcp_conflict(void);

/* Timer */
static void repeating_timer_callback(void);


static uint8_t memsize[2][8] = 
    {{8, 8, 8, 8, 8, 8, 8, 8}, 
     {8, 8, 8, 8, 8, 8, 8, 8}};

static uint8_t g_ethernet_buf[ETHERNET_BUF_MAX_SIZE] = {
    0,
}; // common buffer

/* DHCP */
static uint8_t g_dhcp_get_ip_flag = 0;

/* DNS */
static uint8_t g_dns_target_domain[] = "www.wiznet.io";
static uint8_t g_dns_target_ip[4] = {
    0,
};
static uint8_t g_dns_get_ip_flag = 0;


static bool is_init = false;

static wiz_NetInfo g_net_info =
    {
        .mac  = {0x00, 0x08, 0xDC, 0x12, 0x34, 0x56}, // MAC address
        .ip   = {172,  30,   1,  55},                 // IP address
        .sn   = {255, 255, 255,   0},                 // Subnet Mask
        .gw   = {172,  30,   1, 254},                 // Gateway
        .dns  = {8, 8, 8, 8},                         // DNS server
        .dhcp = NETINFO_DHCP                          // DHCP enable/disable
};





bool wiznetInit(void)
{
  bool ret = true;

  for (int i=0; i<8; i++)
  {
    logPrintf("0x%X\n", getSn_SR(i));
  }

  if (ctlwizchip(CW_INIT_WIZCHIP, (void *)memsize) == -1)
  {
    ret = false;
  }

  logPrintf("[%s] wiznetInit()\n", ret ? "OK":"NG");

  is_init = ret;

  wizchip_dhcp_init();
  
  DNS_init(SOCKET_DNS, g_ethernet_buf);

  // wiznetPrintInfo();

  for (int i=0; i<8; i++)
  {
    logPrintf("0x%X\n", getSn_SR(i));
  }
  return ret;
}


void wiznetPrintInfo(wiz_NetInfo *p_info)
{
  uint8_t tmp_str[8] = {
      0,
  };
  wiz_NetInfo net_info;


  if (is_init == false)
    return;

  if (p_info != NULL)
  {
    net_info = *p_info;

  }
  ctlnetwork(CN_GET_NETINFO, (void *)&net_info);
  ctlwizchip(CW_GET_ID, (void *)tmp_str);

  if (net_info.dhcp == NETINFO_DHCP)
  {
    logPrintf("     %s config : DHCP\n", (char *)tmp_str);
  }
  else
  {
    logPrintf("     %s config : static\n", (char *)tmp_str);
  }

  logPrintf("     MAC          : %02X:%02X:%02X:%02X:%02X:%02X\n", net_info.mac[0], net_info.mac[1], net_info.mac[2], net_info.mac[3], net_info.mac[4], net_info.mac[5]);
  logPrintf("     IP           : %d.%d.%d.%d\n", net_info.ip[0], net_info.ip[1], net_info.ip[2], net_info.ip[3]);
  logPrintf("     Subnet Mask  : %d.%d.%d.%d\n", net_info.sn[0], net_info.sn[1], net_info.sn[2], net_info.sn[3]);
  logPrintf("     Gateway      : %d.%d.%d.%d\n", net_info.gw[0], net_info.gw[1], net_info.gw[2], net_info.gw[3]);
  logPrintf("     DNS          : %d.%d.%d.%d\n", net_info.dns[0], net_info.dns[1], net_info.dns[2], net_info.dns[3]);
}

void wizchip_dhcp_init(void)
{
    logPrintf(" DHCP client running\n");

    DHCP_init(SOCKET_DHCP, g_ethernet_buf);

    reg_dhcp_cbfunc(wizchip_dhcp_assign, wizchip_dhcp_assign, wizchip_dhcp_conflict);
}

static void wizchip_dhcp_assign(void)
{
    getIPfromDHCP(g_net_info.ip);
    getGWfromDHCP(g_net_info.gw);
    getSNfromDHCP(g_net_info.sn);
    getDNSfromDHCP(g_net_info.dns);

    g_net_info.dhcp = NETINFO_DHCP;

    /* Network initialize */
    ctlnetwork(CN_SET_NETINFO, (void *)&g_net_info);

    wiznetPrintInfo(&g_net_info);
    logPrintf(" DHCP leased time : %ld seconds\n", getDHCPLeasetime());
}

static void wizchip_dhcp_conflict(void)
{
    logPrintf(" Conflict IP from DHCP\n");

    // halt or reset or any...
    while (1)
        ; // this example is halt.
}

/* Timer */
static void repeating_timer_callback(void)
{
  DHCP_time_handler();
  DNS_time_handler();
}


/* Initialize */
uint8_t retval     = 0;
uint8_t dhcp_retry = 0;
uint8_t dns_retry  = 0;

void wiznetUpdate(void)
{

  /* Assigned IP through DHCP */
  if (g_net_info.dhcp == NETINFO_DHCP)
  {
      retval = DHCP_run();

      if (retval == DHCP_IP_LEASED)
      {
          if (g_dhcp_get_ip_flag == 0)
          {
              logPrintf(" DHCP success\n");

              g_dhcp_get_ip_flag = 1;
          }
      }
      else if (retval == DHCP_FAILED)
      {
          g_dhcp_get_ip_flag = 0;
          dhcp_retry++;

          if (dhcp_retry <= DHCP_RETRY_COUNT)
          {
              logPrintf(" DHCP timeout occurred and retry %d\n", dhcp_retry);
          }
      }

      if (dhcp_retry > DHCP_RETRY_COUNT)
      {
          logPrintf(" DHCP failed\n");

          DHCP_stop();

          while (1)
              ;
      }
  }

      // /* Get IP through DNS */
      // if ((g_dns_get_ip_flag == 0) && (retval == DHCP_IP_LEASED))
      // {
      //     while (1)
      //     {
      //         if (DNS_run(g_net_info.dns, g_dns_target_domain, g_dns_target_ip) > 0)
      //         {
      //             logPrintf(" DNS success\n");
      //             logPrintf(" Target domain : %s\n", g_dns_target_domain);
      //             logPrintf(" IP of target domain : %d.%d.%d.%d\n", g_dns_target_ip[0], g_dns_target_ip[1], g_dns_target_ip[2], g_dns_target_ip[3]);

      //             g_dns_get_ip_flag = 1;

      //             break;
      //         }
      //         else
      //         {
      //             dns_retry++;

      //             if (dns_retry <= DNS_RETRY_COUNT)
      //             {
      //                 logPrintf(" DNS timeout occurred and retry %d\n", dns_retry);
      //             }
      //         }

      //         if (dns_retry > DNS_RETRY_COUNT)
      //         {
      //             logPrintf(" DNS failed\n");

      //             while (1)
      //                 ;
      //         }
      //     }
      // }
}