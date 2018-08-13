/*
 * winc1500_wifi.h
 *
 * Created: 5/14/2018 7:16:19 PM
 *  Author: pvallone
 */ 


#ifndef WINC1500_WIFI_H_
#define WINC1500_WIFI_H_
#include <asf.h>
#include <errno.h>
#include "driver/include/m2m_wifi.h"
#include "socket/include/socket.h"
#include "iot/http/http_client.h"

#define DEBUG_UART 

// state modes
enum StateMode {
	
	IDOL = 0,
	WORKING,
	BUSY,
	WIFISETUP,
	DOWNLOAD
};
volatile enum StateMode state;

typedef struct  {
		
	char ssid[20];
	char password[20];	
	bool update_requested;
	float current_firmware_version;
	float new_firmware_version;

}
Packet;

Packet pck;
uint8_t eeprom_buffer[sizeof(pck)]; // array to hold struct
char float_temp_buffer[20];
	/** IP address parsing. */
#define IPV4_BYTE(val, index)               ((val >> (index * 8)) & 0xFF)
#define MAIN_M2M_AP_SEC                      M2M_WIFI_SEC_OPEN
#define MAIN_M2M_AP_WEP_KEY                  "1234567890"
#define MAIN_M2M_AP_SSID_MODE                SSID_MODE_VISIBLE
#define MAIN_HTTP_PROV_SERVER_DOMAIN_NAME    "PhillyNJ.com"
#define MAIN_M2M_DEVICE_NAME                 "PHILLYNJ_00:00"
#define MAIN_MAC_ADDRESS                     {0xf8, 0xf0, 0x05, 0x45, 0xD4, 0x84}
/** Using broadcast address for simplicity. */
#define MAIN_SERVER_PORT                    (80)
/** Send buffer of TCP socket. */
#define MAIN_PREFIX_BUFFER                  "GET /index.php"
#define MAIN_POST_BUFFER                    " HTTP/1.1\r\nHost: 192.168.1.195\r\nAccept: */*\r\n\r\n"	
/** information provider server. */
#define MAIN_SERVER_NAME					"192.168.1.195"		
/** Receive buffer size. */
#define MAIN_WIFI_M2M_BUFFER_SIZE           1400
#define MAIN_HEX2ASCII(x)                   (((x) >= 10) ? (((x) - 10) + 'A') : ((x) + '0'))

#define MAIN_HTTP_FILE_URL "http://192.168.1.195/firmware.bin"
/** Maximum size for packet buffer. */
#define MAIN_BUFFER_MAX_SIZE                 (64)
/** Maximum file name length. */
#define MAIN_MAX_FILE_NAME_LENGTH            (250)
/** Maximum file extension length. */
#define MAIN_MAX_FILE_EXT_LENGTH             (8)
/** Output format with '0'. */
#define MAIN_ZERO_FMT(SZ)                    (SZ == 4) ? "%04d" : (SZ == 3) ? "%03d" : (SZ == 2) ? "%02d" : "%d"
void convertFloatToString (float f, char * buffer);
typedef enum {
	NOT_READY = 0, /*!< Not ready. */
	STORAGE_READY = 0x01, /*!< Storage is ready. */
	WIFI_CONNECTED = 0x02, /*!< Wi-Fi is connected. */
	GET_REQUESTED = 0x04, /*!< GET request is sent. */
	DOWNLOADING = 0x08, /*!< Running to download. */
	COMPLETED = 0x10, /*!< Download completed. */
	CANCELED = 0x20 /*!< Download canceled. */
} download_state;
float request_firmware_version;
struct http_client_module http_client_module_inst;
struct sockaddr_in addr_in;
download_state down_state;
uint32_t http_file_size;
uint32_t received_file_size;
bool is_state_set(download_state mask);
void add_state(download_state mask);
void clear_state(download_state mask);
void start_download(void);
void http_client_callback(struct http_client_module *module_inst, int type, union http_client_data *data);

void winc1500_resolve_cb(uint8_t *hostName, uint32_t hostIp);
void winc1500_socket_cb(SOCKET sock, uint8_t u8Msg, void *pvMsg);
void winc1500_requestClient(void);
void winc1500_wifi_cb(uint8_t u8MsgType, void *pvMsg);
void winc1500_download_socket_cb(SOCKET sock, uint8_t u8Msg, void *pvMsg);

#endif /* WINC1500_WIFI_H_ */