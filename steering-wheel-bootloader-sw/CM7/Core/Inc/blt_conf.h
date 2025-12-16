/************************************************************************************/ /**
* \file         blt_conf.h
* \brief        Bootloader configuration header file (STM32H747 CM7, CAN-only).
****************************************************************************************/
#ifndef BLT_CONF_H
#define BLT_CONF_H

/****************************************************************************************
*   C P U   D R I V E R   C O N F I G U R A T I O N
****************************************************************************************/
#define BOOT_CPU_XTAL_SPEED_KHZ (16000)
#define BOOT_CPU_SYSTEM_SPEED_KHZ (480000)
#define BOOT_CPU_BYTE_ORDER_MOTOROLA (0)
#define BOOT_CPU_USER_PROGRAM_START_HOOK (0)

/****************************************************************************************
*   C O M M U N I C A T I O N   I N T E R F A C E   C O N F I G U R A T I O N
****************************************************************************************/
/* NET disabled */
#define BOOT_COM_NET_ENABLE (0)
#define BOOT_COM_NET_TX_MAX_DATA (0)
#define BOOT_COM_NET_RX_MAX_DATA (0)
#define BOOT_COM_NET_PORT (0)
#define BOOT_COM_NET_DHCP_ENABLE (0)
#define BOOT_COM_NET_IPADDR0 (0)
#define BOOT_COM_NET_IPADDR1 (0)
#define BOOT_COM_NET_IPADDR2 (0)
#define BOOT_COM_NET_IPADDR3 (0)
#define BOOT_COM_NET_NETMASK0 (0)
#define BOOT_COM_NET_NETMASK1 (0)
#define BOOT_COM_NET_NETMASK2 (0)
#define BOOT_COM_NET_NETMASK3 (0)
#define BOOT_COM_NET_GATEWAY0 (0)
#define BOOT_COM_NET_GATEWAY1 (0)
#define BOOT_COM_NET_GATEWAY2 (0)
#define BOOT_COM_NET_GATEWAY3 (0)
#define BOOT_COM_NET_DEFERRED_INIT_ENABLE (0)

/* USB disabled */
#define BOOT_COM_USB_ENABLE (0)
#define BOOT_COM_USB_TX_MAX_DATA (0)
#define BOOT_COM_USB_RX_MAX_DATA (0)

/* CAN enabled as the ONLY interface */
#define BOOT_COM_CAN_ENABLE (1)
#define BOOT_COM_CAN_BAUDRATE (1000000)
#define BOOT_COM_CAN_TX_MSG_ID (0x7E1) // TODO: verify correct ID
#define BOOT_COM_CAN_TX_MAX_DATA (8)
#define BOOT_COM_CAN_RX_MSG_ID (0x667) // TODO: verify correct ID
#define BOOT_COM_CAN_RX_MAX_DATA (8)
#define BOOT_COM_CAN_CHANNEL_INDEX (0)

/* RS232 disabled */
#define BOOT_COM_RS232_ENABLE (0)
#define BOOT_COM_RS232_BAUDRATE (0)
#define BOOT_COM_RS232_TX_MAX_DATA (0)
#define BOOT_COM_RS232_RX_MAX_DATA (0)
#define BOOT_COM_RS232_CHANNEL_INDEX (0)

/****************************************************************************************
*   B A C K D O O R   E N T R Y   C O N F I G U R A T I O N
****************************************************************************************/
#define BOOT_BACKDOOR_HOOKS_ENABLE (0)

/****************************************************************************************
*   N O N - V O L A T I L E   M E M O R Y   D R I V E R   C O N F I G U R A T I O N
****************************************************************************************/
#define BOOT_NVM_HOOKS_ENABLE (0)
#define BOOT_NVM_SIZE_KB (2048)
#define BOOT_NVM_CHECKSUM_HOOKS_ENABLE (0)

/****************************************************************************************
*   W A T C H D O G   D R I V E R   C O N F I G U R A T I O N
****************************************************************************************/
#define BOOT_COP_HOOKS_ENABLE (0)

/****************************************************************************************
*   S E E D   /   K E Y   S E C U R I T Y   C O N F I G U R A T I O N
****************************************************************************************/
#define BOOT_XCP_SEED_KEY_ENABLE (0)

#endif /* BLT_CONF_H */
/*********************************** end of blt_conf.h *********************************/
