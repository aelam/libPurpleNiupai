/**
 * @file NPutils.h Utility functions
 *
 * purple
 *
 * Purple is the legal property of its developers, whose names are too numerous
 * to list here.  Please refer to the COPYRIGHT file distributed with this
 * source distribution.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
 */
#ifndef NP_UTILS_H__
#define NP_UTILS_H__

/*encode the str to RFC2047 style*/
char *np_encode_mime(const char *str);

/**
 * Generate the Random GUID
 */
char *rand_guid(void);

/**
 * Encodes the spaces in a string
 *
 * @param str The string to be encoded.
 * @param buf The buffer to hold the encoded string.
 * @param len The maximum length (including NUL) to put in @buf.
 *
 * @return Whether @str was able to fit in @buf.
 */
gboolean
np_encode_spaces(const char *str, char *buf, size_t len);

/**
 * Parses the np message formatting into a format compatible with Purple.
 *
 * @param mime     The mime header with the formatting.
 * @param pre_ret  The returned prefix string.
 * @param post_ret The returned postfix string.
 *
 * @return The new message.
 */
void np_parse_format(const char *mime, char **pre_ret, char **post_ret);

/**
 * Parses the Purple message formatting (html) into the np format.
 *
 * @param html			The html message to format.
 * @param attributes	The returned attributes string.
 * @param message		The returned message string.
 *
 * @return The new message.
 */
void np_import_html(const char *html, char **attributes, char **message);

/**
 * Parses a socket string.
 *
 * @param str		A host:port string.
 * @param ret_host 	Return string value of the host.
 * @param ret_port	Return integer value of the port.
 */
void np_parse_socket(const char *str, char **ret_host, int *ret_port);

/**
 * Parses a user name
 *
 * @param str         A network:username string.
 * @param ret_user    Return of the user's passport.
 * @param ret_network Return of the user's network.
 */
void np_parse_user(const char *str, char **ret_user, int *ret_network);

/**
 * Verify if the email is a vaild passport.
 *
 * @param passport 	The email
 *
 * @return True if it is a valid passport, else FALSE
 */
gboolean np_email_is_valid(const char *passport);

/**
 * Handle np Challenge Computation
 * This algorithm references
 * http://imfreedom.org/wiki/index.php/np:NS/Challenges
 *
 * @param input 	Challenge input.
 * @param output 	Callenge output.
 */
void np_handle_chl(char *input, char *output);

/**
 * Read a byte from a buffer
 *
 * @param buf Pointer to buffer.
 *
 * @return 8-bit byte
 */
guint8 np_read8(const char *buf);

/**
 * Read a little-endian short from a buffer
 *
 * @param buf Pointer to buffer.
 *
 * @return 16-bit short
 */
guint16 np_read16le(const char *buf);

/**
 * Read a big-endian short from a buffer
 *
 * @param buf Pointer to buffer.
 *
 * @return 16-bit short
 */
guint16 np_read16be(const char *buf);

/**
 * Read a little-endian int from a buffer
 *
 * @param buf Pointer to buffer.
 *
 * @return 32-bit int
 */
guint32 np_read32le(const char *buf);

/**
 * Read a big-endian int from a buffer
 *
 * @param buf Pointer to buffer.
 *
 * @return 32-bit int
 */
guint32 np_read32be(const char *buf);

/**
 * Read a little-endian long from a buffer
 *
 * @param buf Pointer to buffer.
 *
 * @return 64-bit long
 */
guint64 np_read64le(const char *buf);

/**
 * Read a big-endian long from a buffer
 *
 * @param buf Pointer to buffer.
 *
 * @return 64-bit long
 */
guint64 np_read64be(const char *buf);

/**
 * Write a byte to a buffer
 *
 * @param buf  Pointer to buffer.
 * @param data 8-bit byte.
 */
void np_write8(char *buf, guint8 data);

/**
 * Write a little-endian short to a buffer
 *
 * @param buf  Pointer to buffer.
 * @param data short.
 */
void np_write16le(char *buf, guint16 data);

/**
 * Write a big-endian short to a buffer
 *
 * @param buf  Pointer to buffer.
 * @param data short.
 */
void np_write16be(char *buf, guint16 data);

/**
 * Write a little-endian int to a buffer
 *
 * @param buf  Pointer to buffer.
 * @param data int.
 */
void np_write32le(char *buf, guint32 data);

/**
 * Write a big-endian int to a buffer
 *
 * @param buf  Pointer to buffer.
 * @param data int.
 */
void np_write32be(char *buf, guint32 data);

/**
 * Write a little-endian long to a buffer
 *
 * @param buf  Pointer to buffer.
 * @param data long.
 */
void np_write64le(char *buf, guint64 data);

/**
 * Write a big-endian long to a buffer
 *
 * @param buf  Pointer to buffer.
 * @param data short
 */
void np_write64be(char *buf, guint64 data);

/**
 * Same as above, but these increment the buf pointer.
 */
#define np_pop8(buf)    np_read8((buf+=1)-1)
#define np_pop16le(buf) np_read16le((buf+=2)-2)
#define np_pop16be(buf) np_read16be((buf+=2)-2)
#define np_pop32le(buf) np_read32le((buf+=4)-4)
#define np_pop32be(buf) np_read32be((buf+=4)-4)
#define np_pop64le(buf) np_read64le((buf+=8)-8)
#define np_pop64be(buf) np_read64be((buf+=8)-8)
#define np_push8(buf, data)    np_write8(buf, data),    buf+=1
#define np_push16le(buf, data) np_write16le(buf, data), buf+=2
#define np_push16be(buf, data) np_write16be(buf, data), buf+=2
#define np_push32le(buf, data) np_write32le(buf, data), buf+=4
#define np_push32be(buf, data) np_write32be(buf, data), buf+=4
#define np_push64le(buf, data) np_write64le(buf, data), buf+=8
#define np_push64be(buf, data) np_write64be(buf, data), buf+=8

#endif /* NP_UTILS_H__ */

