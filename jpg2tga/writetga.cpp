//------------------------------------------------------------------------------
// File: writetga.cpp
// Simple TGA writer -- handles 24-bit truecolor, 8-bit greyscale
// Public domain, Rich Geldreich <richgel99@gmail.com>
//------------------------------------------------------------------------------
#include "writetga.h"
//------------------------------------------------------------------------------
tga_writer::tga_writer()
{
  Pfile = NULL;
  width = height = bytes_per_pixel = bytes_per_line = 0;
  image_type = TGA_IMAGE_TYPE_NULL;
}
//------------------------------------------------------------------------------
tga_writer::~tga_writer()
{
  close();
}
//------------------------------------------------------------------------------
bool tga_writer::close(void)
{
  width = height = bytes_per_pixel = bytes_per_line = 0;
  image_type = TGA_IMAGE_TYPE_NULL;

  if (Pfile)
  {
    bool failed = (fclose(Pfile) == EOF);

    Pfile = NULL;

    return (failed);
  }

  return (false);
}
//------------------------------------------------------------------------------
bool tga_writer::open(
  const char *Pfilename,
  int width, int height,
  tga_image_type_t image_type)
{
  assert(Pfilename);

  close();

#if defined(_MSC_VER)
  Pfile = NULL;
  fopen_s(&Pfile, Pfilename, "wb");
#else
  Pfile = fopen(Pfilename, "wb");
#endif
  if (!Pfile)
    return (true);

  this->width = width;
  this->height = height;
  this->image_type = image_type;

  uint8 tga_header[18];
  memset(tga_header, 0, sizeof(tga_header));

  bool backwards_flag = false;

  tga_header[12] = (uint8)(width & 0xFF);
  tga_header[13] = (uint8)((width >> 8) & 0xFF);
  tga_header[14] = (uint8)(height & 0xFF);
  tga_header[15] = (uint8)((height >> 8) & 0xFF);
  tga_header[17] = backwards_flag ? 0x00 : 0x20;

  switch (image_type)
  {
    case TGA_IMAGE_TYPE_BGR:
    {
      tga_header[2] = 2;
      tga_header[16] = 24;
      bytes_per_pixel = 3;
      break;
    }
    case TGA_IMAGE_TYPE_GREY:
    {
      tga_header[2] = 3;
      tga_header[16] = 8;
      bytes_per_pixel = 1;
      break;
    }
    default:
      assert(false);
  }

  bytes_per_line = width * bytes_per_pixel;

  if (fwrite(tga_header, sizeof(tga_header), 1, Pfile) != 1)
    return (true);

  return (false);
}
//------------------------------------------------------------------------------
bool tga_writer::write_line(const void *Pscan_line)
{
  if (!Pfile)
    return (true);

  if (fwrite(Pscan_line, bytes_per_line, 1, Pfile) != 1)
    return (true);

  return (false);
}
//------------------------------------------------------------------------------

