// ----------------------------------------------------------------------------
//         ATMEL Crypto_Devices Software Support  -  Colorado Springs, CO -
// ----------------------------------------------------------------------------
// DISCLAIMER:  THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
// DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
// OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ----------------------------------------------------------------------------
/** \file
 *  \brief This file contains the utility function for AES132 library example.
 *  \author Atmel Crypto Products
 *  \date June 16, 2015
 */

#include "utility.h"

// Global variables for serial
uint8_t  g_packet_block[PACKET_BLOCK_SIZE_MAX];
uint16_t g_packet_block_index;


/** \brief Converting Nibble to ASCII hex
 *
 * \param data is nibble data to be converted
 *
 * \return data is the ASCII hex value
**/
uint8_t nibble_to_hex(uint8_t data)
{
	data &= 0x0F;
	if (data <= 0x09) {
		// 0x0-0x9
		data += '0';
	} else {
		// 0xA-0xF
		data = data - 10 + 'A';
	}
	return data;
}

/** \brief Converting ASCII hex to nibble
 *
 * \param data is the ASCII hex value to be converted
 *
 * \return data is the nibble value
**/
uint8_t hex_to_nibble(uint8_t data)
{
	if ((data >= '0') && (data <= '9' )) {
		// Numbers (0-9)
		data -= '0';
	} else if ((data >= 'A') && (data <= 'F' )) {
		// Uppercase (A-F)
		data = data -'A' + 10;
	} else if ((data >= 'a') && (data <= 'f' )) {
		// Lowercase (a-f)
		data = data -'a' + 10;
	} else {
		// Illegal
		data = 0;
	}
	
	return data;
}

/** \brief print hexadecimal to UART
 *
 * \param buffer is byte which will be printed to UART
 *
 * \param ucLength is array size
**/
void printf_puthex(uint8_t data_buffer)
{
	printf("%.2X",data_buffer);
}

/** \brief print hexadecimal array to UART
 *
 * \param buffer is array which will be printed to UART
 *
 * \param ucLength is array size
**/
void printf_puthex_array(uint8_t* data_buffer, uint8_t length)
{
	uint8_t i_data;
	
	for (i_data = 0; i_data < length; i_data++)
	{
		printf("%.2X",*data_buffer++);
		printf(" ");
	}
}

/** \brief print 16byte data to UART
 *
 * \param *pBuf is array which will be printed to UART
 *
 * \param ucLength is array size
**/
void printf_puthex_16 (uint16_t data)
{
	printf_puthex((uint8_t) (data >> 8));   //print MSB byte
	printf_puthex((uint8_t) data );         //print LSB byte
	printf(" ");
}




/** \brief Parses a byte from the argument, put the parsed data into a global array.
 *         Uses static variables and global variables.
 *         Set #PACKET_BLOCK_SIZE_MAX define in parser.h to maximum buffer size needed.
 *
 *  \param serial_data Data input
 *  \return 1 on complete parsing, 0 otherwise
 */

 // Example Enter (90) 
uint8_t parse_command(uint8_t serial_data)
{
	static unsigned char parser_state, nibble_value;
	
	switch (parser_state) {
		case 0:
			g_packet_block_index = 0;
			if (serial_data == '(') {
				parser_state = 1;
			} 
			break;
		case 1:
			nibble_value = hex_to_nibble(serial_data);
			parser_state = 2;
			break;
		case 2:
			g_packet_block[g_packet_block_index] = (nibble_value << 4) + hex_to_nibble(serial_data);
			if (g_packet_block_index < PACKET_BLOCK_SIZE_MAX - 1) {
				g_packet_block_index++;
			} else {
				printf("Error buffer overflow\n\r");
			}
			parser_state = 3;
			break;
		case 3:
			if (serial_data == ' ') {
				parser_state = 1;
			} else if (serial_data == ')') {
				parser_state = 0;
				printf("\n\r");
				return 1;
			} else {
				parser_state = 0;
			}
			break;
		default:
			parser_state = 0;
	} // switch (parser_state)
	
	return 0;
}

