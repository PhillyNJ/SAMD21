/*
 * winc1500_wifi.c
 *
 * Created: 5/14/2018 7:16:29 PM
 *  Author: pvallone
 */ 
#include "winc1500_wifi.h"

extern void formatFloat(float f, char * buffer);
extern void store_file_packet(char *data, uint32_t length);
extern void update_credentials(void);
/** IP address of host. */
uint32_t gu32HostIp = 0;
/** TCP client socket handlers. */
SOCKET tcp_client_socket = -1;
/** Receive buffer definition. */
uint8_t gau8ReceivedBuffer[MAIN_WIFI_M2M_BUFFER_SIZE] = {0};
/** Wi-Fi status variable. */
bool gbConnectedWifi = false;
/** Get host IP status variable. */
bool gbHostIpByName = false;
/** TCP Connection status variable. */
bool gbTcpConnection = false; 
uint8_t gau8MacAddr[] = MAIN_MAC_ADDRESS;
int8_t gacDeviceName[] = MAIN_M2M_DEVICE_NAME;
tstrM2MAPConfig gstrM2MAPConfig = {
	MAIN_M2M_DEVICE_NAME, 1, 0, WEP_40_KEY_STRING_SIZE, MAIN_M2M_AP_WEP_KEY, (uint8_t)MAIN_M2M_AP_SEC, MAIN_M2M_AP_SSID_MODE
};

CONST char gacHttpProvDomainName[] = MAIN_HTTP_PROV_SERVER_DOMAIN_NAME;

void winc1500_resolve_cb(uint8_t *hostName, uint32_t hostIp)
{
	gu32HostIp = hostIp;
	gbHostIpByName = true;
	#if defined(DEBUG_UART)
	printf("resolve_cb: %s IP address is %d.%d.%d.%d\r\n\r\n", hostName,
			(int)IPV4_BYTE(hostIp, 0), (int)IPV4_BYTE(hostIp, 1),
			(int)IPV4_BYTE(hostIp, 2), (int)IPV4_BYTE(hostIp, 3));
	#endif
	http_client_socket_resolve_handler(hostName, hostIp);
 
}


void winc1500_socket_cb(SOCKET sock, uint8_t u8Msg, void *pvMsg)
{
	/* Check for socket event on TCP socket. */
	if (sock == tcp_client_socket) {
		switch (u8Msg) {
		case SOCKET_MSG_CONNECT:
		{
			if (gbTcpConnection) {
				memset(gau8ReceivedBuffer, 0, sizeof(gau8ReceivedBuffer));
				sprintf((char *)gau8ReceivedBuffer, "%s%s", MAIN_PREFIX_BUFFER, MAIN_POST_BUFFER);
				
				//printf("URL: %s\n\r", gau8ReceivedBuffer);
				tstrSocketConnectMsg *pstrConnect = (tstrSocketConnectMsg *)pvMsg;
				if (pstrConnect && pstrConnect->s8Error >= SOCK_ERR_NO_ERROR) {
					send(tcp_client_socket, gau8ReceivedBuffer, strlen((char *)gau8ReceivedBuffer), 0);

					memset(gau8ReceivedBuffer, 0, MAIN_WIFI_M2M_BUFFER_SIZE);
					recv(tcp_client_socket, &gau8ReceivedBuffer[0], MAIN_WIFI_M2M_BUFFER_SIZE, 0);
				} else {
					#if defined(DEBUG_UART)
					printf("socket_cb: connect error!\r\n");
					#endif
					gbTcpConnection = false;
					close(tcp_client_socket);
					tcp_client_socket = -1;
				}			
			
			}
		}
		break;

		case SOCKET_MSG_RECV:
		{
			char *pcIndxPtr;
			char *pcEndPtr;

			tstrSocketRecvMsg *pstrRecv = (tstrSocketRecvMsg *)pvMsg;
			if (pstrRecv && pstrRecv->s16BufferSize > 0) {
						   
				/* Get city name. */
				pcIndxPtr = strstr((char *)pstrRecv->pu8Buffer, "version");
					
				
				if (NULL != pcIndxPtr) {
					pcIndxPtr = pcIndxPtr + strlen("version") + 1;
					pcEndPtr = strstr(pcIndxPtr, "</");
					if (NULL != pcEndPtr) {
						*pcEndPtr = 0;
					}
					
					char * val = (char *)pcIndxPtr;
					float f =  atof(val);					
					
					#if defined(DEBUG_UART)							
					formatFloat(f, float_temp_buffer);				
					printf("Version %s found on server.\r\n", float_temp_buffer);	
					formatFloat(pck.current_firmware_version, float_temp_buffer);
					printf("Local Version %s.\r\n", float_temp_buffer);
					#endif

					pck.new_firmware_version = f;
					if(pck.new_firmware_version > pck.current_firmware_version){						
						#if defined(DEBUG_UART)
						printf("New Firmware found! Press PA04 to install\n\r");
						#endif
						pck.update_requested = true;
						update_credentials();
					}	
					
					#if defined(DEBUG_UART)
					formatFloat(pck.current_firmware_version, float_temp_buffer);
					printf("Current Version %s. \r\n", float_temp_buffer);
					#endif
					request_firmware_version = f;	
					close(tcp_client_socket);
					tcp_client_socket = -1;
				} else {
					#if defined(DEBUG_UART)
					printf("N/A\r\n");
					#endif
					break;
				}	

				memset(gau8ReceivedBuffer, 0, sizeof(gau8ReceivedBuffer));
				recv(tcp_client_socket, &gau8ReceivedBuffer[0], MAIN_WIFI_M2M_BUFFER_SIZE, 0);
			} else {
				#if defined(DEBUG_UART)
				printf("socket_cb: recv error!\r\n");
				#endif
				close(tcp_client_socket);
				tcp_client_socket = -1;
			}
		}
		break;
        case M2M_WIFI_REQ_DHCP_CONF:
        {
			#if defined(DEBUG_UART)
	        uint8_t *pu8IPAddress = (uint8_t *)pvMsg;			
	        printf("IP address is %u.%u.%u.%u\r\n",			
	        pu8IPAddress[0], pu8IPAddress[1], pu8IPAddress[2], pu8IPAddress[3]);
			#endif
	        break;
        }
		default:
			break;
		}
	}
}

void winc1500_requestClient(void){

	// if wifi is connected and gbTcpConnection is true, request site
	
	if (gbConnectedWifi && !gbTcpConnection) {
		if (gbHostIpByName) {
			/* Open TCP client socket. */
			if (tcp_client_socket < 0) {
				if ((tcp_client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
					#if defined(DEBUG_UART)
					printf("main: failed to create TCP client socket error!\r\n");
					#endif
					return;
				}
			}

			/* Connect TCP client socket. */
			addr_in.sin_family = AF_INET;
			addr_in.sin_port = _htons(MAIN_SERVER_PORT);
			addr_in.sin_addr.s_addr = gu32HostIp;
			if (connect(tcp_client_socket, (struct sockaddr *)&addr_in, sizeof(struct sockaddr_in)) != SOCK_ERR_NO_ERROR) {
				#if defined(DEBUG_UART)
				printf("main: failed to connect socket error!\r\n");
				#endif
				return;
			}

			gbTcpConnection = true;
		}
	}
}
void winc1500_download_socket_cb(SOCKET sock, uint8_t u8Msg, void *pvMsg){

	http_client_socket_event_handler(sock, u8Msg, pvMsg);
}

void winc1500_wifi_cb(uint8_t u8MsgType, void *pvMsg){
	
	switch (u8MsgType) {
		case M2M_WIFI_RESP_CON_STATE_CHANGED:
		{
			tstrM2mWifiStateChanged *pstrWifiState = (tstrM2mWifiStateChanged *)pvMsg;
			if (pstrWifiState->u8CurrState == M2M_WIFI_CONNECTED) {
				add_state(STORAGE_READY);
				#if defined(DEBUG_UART)
				printf("Wifi Connection Established\r\n");
				#endif
				m2m_wifi_request_dhcp_client();
			} else if (pstrWifiState->u8CurrState == M2M_WIFI_DISCONNECTED) {
				#if defined(DEBUG_UART)
				printf("Wifi Disconnected\r\n");
				#endif
				clear_state(WIFI_CONNECTED);
				gbConnectedWifi = false;				
			}
			
			if (is_state_set(GET_REQUESTED)) {
				clear_state(GET_REQUESTED);
			}
			
			break;
		}

		case M2M_WIFI_REQ_DHCP_CONF:
		{	
			if(state == DOWNLOAD){
				#if defined(DEBUG_UART)
				uint8_t *pu8IPAddress = (uint8_t *)pvMsg;	
				#if defined(DEBUG_UART)			
				printf("wifi_cb: IP address is %u.%u.%u.%u\r\n",			
				#endif
				pu8IPAddress[0], pu8IPAddress[1], pu8IPAddress[2], pu8IPAddress[3]);	
				#endif
				add_state(WIFI_CONNECTED);
				start_download();

			} else {
				#if defined(DEBUG_UART)
				uint8_t *pu8IPAddress = (uint8_t *)pvMsg;				
				printf("*** IP address is %u.%u.%u.%u\r\n",				
				pu8IPAddress[0], pu8IPAddress[1], pu8IPAddress[2], pu8IPAddress[3]);
				#endif
				gbConnectedWifi = true;
					/* Obtain the IP Address by network name */
				gethostbyname((uint8_t *)MAIN_SERVER_NAME);
			}			

			break;
		}

		case M2M_WIFI_RESP_PROVISION_INFO:
		{
			tstrM2MProvisionInfo *pstrProvInfo = (tstrM2MProvisionInfo *)pvMsg;			

			if (pstrProvInfo->u8Status == M2M_SUCCESS) {
			
				m2m_wifi_connect((char *)pstrProvInfo->au8SSID, strlen((char *)pstrProvInfo->au8SSID), pstrProvInfo->u8SecType,
				pstrProvInfo->au8Password, M2M_WIFI_CH_ALL);				
				pck.current_firmware_version = 0;
				pck.new_firmware_version = 0;
				pck.update_requested = true;
				strncpy(pck.password, (const char *)pstrProvInfo->au8Password, sizeof(pck.password));
				strncpy(pck.ssid, (const char *)pstrProvInfo->au8SSID , sizeof(pck.ssid));
				update_credentials();	
				state = IDOL;
				
			} else {
			    #if defined(DEBUG_UART)
				printf("Credentials failed! Please reset the device and try again\r\n");
				#endif
			}
		}
		break;

		default:
		{
			break;
		}
	}
}

void start_download(void)
{
	if (!is_state_set(STORAGE_READY)) {
		#if defined(DEBUG_UART)
		printf("start_download: MMC storage not ready.\r\n");
		#endif
		return;
	}

	if (!is_state_set(WIFI_CONNECTED)) {
		#if defined(DEBUG_UART)
		printf("start_download: Wi-Fi is not connected.\r\n");
		#endif
		return;
	}

	if (is_state_set(GET_REQUESTED)) {
		#if defined(DEBUG_UART)
		printf("start_download: request is sent already.\r\n");
		#endif
		return;
	}

	if (is_state_set(DOWNLOADING)) {
		#if defined(DEBUG_UART)
		printf("start_download: running download already.\r\n");
		#endif
		return;
	}

	/* Send the HTTP request. */
	#if defined(DEBUG_UART)
	printf("start_download: sending HTTP request...\r\n");
	#endif
	http_client_send_request(&http_client_module_inst,MAIN_HTTP_FILE_URL, HTTP_METHOD_GET, NULL, NULL);

}

void http_client_callback(struct http_client_module *module_inst, int type, union http_client_data *data)
{
	switch (type) {
	case HTTP_CLIENT_CALLBACK_SOCK_CONNECTED:
		#if defined(DEBUG_UART)
		printf("http_client_callback: HTTP client socket connected.\r\n");
		#endif
		break;

	case HTTP_CLIENT_CALLBACK_REQUESTED:
		#if defined(DEBUG_UART)
		printf("http_client_callback: request completed.\r\n");
		#endif
		add_state(GET_REQUESTED);
		break;

	case HTTP_CLIENT_CALLBACK_RECV_RESPONSE:
		#if defined(DEBUG_UART)
		printf("http_client_callback: received response %u data size %u\r\n",
				(unsigned int)data->recv_response.response_code,
				(unsigned int)data->recv_response.content_length);
		#endif
		if ((unsigned int)data->recv_response.response_code == 200) {
			http_file_size = data->recv_response.content_length;
			received_file_size = 0;
		} 
		else {
			
			add_state(CANCELED);
			return;
		}
		if (data->recv_response.content_length <= MAIN_BUFFER_MAX_SIZE) {
			store_file_packet(data->recv_response.content, data->recv_response.content_length);
			add_state(COMPLETED);
		
		}
		break;

	case HTTP_CLIENT_CALLBACK_RECV_CHUNKED_DATA:
		store_file_packet(data->recv_chunked_data.data, data->recv_chunked_data.length);
		if (data->recv_chunked_data.is_complete) {			
			
			add_state(COMPLETED);					

		}

		break;

	case HTTP_CLIENT_CALLBACK_DISCONNECTED:
		#if defined(DEBUG_UART)
		printf("http_client_callback: disconnection reason:%d\r\n", data->disconnected.reason);
		#endif
		/* If disconnect reason is equal to -ECONNRESET(-104),
		 * It means the server has closed the connection (timeout).
		 * This is normal operation.
		 */
		if (data->disconnected.reason == -EAGAIN) {
			/* Server has not responded. Retry immediately. */
			if (is_state_set(DOWNLOADING)) {
		
				clear_state(DOWNLOADING);
			}

			if (is_state_set(GET_REQUESTED)) {
				clear_state(GET_REQUESTED);
			}			
			start_download();
		}

		break;
	}
}


void clear_state(download_state mask)
{
	down_state &= ~mask;
}

void add_state(download_state mask)
{
	down_state |= mask;
	
}

bool is_state_set(download_state mask)
{
	return ((down_state & mask) != 0);
}

