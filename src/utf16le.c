#include "utf.h"
#include "debug.h"
#include "wrappers.h"
#include <unistd.h>


extern const char *STR_UTF16BE;
extern char *const STR_UTF16LE;
extern char const *STR_UTF8;

int
from_utf16le_to_utf16be(int infile, int outfile)
{
  int bom;
  utf16_glyph_t buf;
  ssize_t bytes_read;
  size_t bytes_to_write;
  int ret = -1;

  bom = UTF16BE;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  reverse_bytes(&bom, 2);
#endif
  write_to_bigendian(outfile, &bom, 2);

  while ((bytes_read = read_to_bigendian(infile, &(buf.upper_bytes), 2)) > 0) {
    bytes_to_write = 2;
    if(is_upper_surrogate_pair(buf)) {

      if((bytes_read = read_to_bigendian(infile, &(buf.lower_bytes), 2)) < 0) {
        break;
      }
      reverse_bytes(&(buf.lower_bytes), 2);
      bytes_to_write += 2;
    }
    reverse_bytes(&(buf.upper_bytes), 2);
    write_to_bigendian(outfile, &buf, bytes_to_write);
  }
  ret = bytes_read;
  return ret;
}

int
from_utf16le_to_utf8(int infile, int outfile)
{

  int bom;
  utf8_glyph_t utf8_buf;
  code_point_t code_point;
  utf16_glyph_t buf;
  ssize_t bytes_read;
  size_t bytes_to_write;
  int ret = -1;

  bom = UTF8;
  #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  reverse_bytes(&bom, 3);
  #endif
  write_to_bigendian(outfile, &bom, 3);

  while ((bytes_read = read_to_bigendian(infile, &(buf.upper_bytes), 2)) > 0) {
    if(is_upper_surrogate_pair(buf)) {
      if((bytes_read = read_to_bigendian(infile, &(buf.lower_bytes), 2)) < 0) {
        break;
      }
    }
    code_point = utf16_glyph_to_code_point(&buf);
    utf8_buf = code_point_to_utf8_glyph(code_point, &bytes_to_write);

    for(unsigned int i = 0; i < bytes_to_write ; i++)
    {
      write_to_bigendian(outfile, &utf8_buf.bytes[i], 1);
    }
    buf.upper_bytes = 0x00;
    buf.lower_bytes = 0x00;
    utf8_buf.bytes[0].byte = 0x00;
    utf8_buf.bytes[1].byte = 0x00;
    utf8_buf.bytes[2].byte = 0x00;
    utf8_buf.bytes[3].byte = 0x00;
  }
  ret = bytes_read;
  return ret;
}

utf16_glyph_t
code_point_to_utf16le_glyph(code_point_t code_point, size_t *size_of_glyph)
{
  utf16_glyph_t ret;

  memeset(&ret, 0, sizeof ret);
  if(is_code_point_surrogate(code_point)) {
    code_point -= 0x10000;
    ret.upper_bytes = (code_point >> 10) + 0xD800;
    ret.lower_bytes = (code_point & 0x3FF) + 0xDC00;
  #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    reverse_bytes(&ret.upper_bytes, 2);
    reverse_bytes(&ret.lower_bytes, 2);
  #endif
    *size_of_glyph = 4;
  }
  else {
    ret.upper_bytes |= code_point;
  #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    reverse_bytes(&ret.upper_bytes, 2);
  #endif
    *size_of_glyph = 2;
  }
  return ret;
}
