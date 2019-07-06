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

#include <stdint.h>
#include <string.h>
#include <asf.h>
// Macros
#define PACKET_BLOCK_SIZE_MAX    (256)

extern uint8_t  g_packet_block[PACKET_BLOCK_SIZE_MAX];
extern uint16_t g_packet_block_index;


/** \brief Converting Nibble to ASCII hex
 *
 * \param data is nibble data to be converted
 *
 * \return data is the ASCII hex value
**/
uint8_t nibble_to_hex(uint8_t data);

/** \brief Converting ASCII hex to nibble
 *
 * \param data is the ASCII hex value to be converted
 *
 * \return data is the nibble value
**/
uint8_t hex_to_nibble(uint8_t data);

/** \brief print hexadecimal to UART
 *
 * \param buffer is byte which will be printed to UART
 *
 * \param ucLength is array size
**/
void printf_puthex(uint8_t data_buffer);

/** \brief print array to UART
 *
 * \param buffer is array which will be printed to UART
 *
 * \param ucLength is array size
**/
void printf_puthex_array(uint8_t* data_buffer, uint8_t length);

/** \brief print 16byte data to UART
 *
 * \param *pBuf is array which will be printed to UART
 *
 * \param ucLength is array size
**/
void printf_puthex_16 (uint16_t data);


/** \brief Parses a byte from the argument, put the parsed data into a global array.
 *         Uses static variables and global variables.
 *         Set #PACKET_BLOCK_SIZE_MAX define in parser.h to maximum buffer size needed.
 *
 *  \param serial_data Data input
 *  \return 1 on complete parsing, 0 otherwise
 */
uint8_t parse_command(uint8_t serial_data);

