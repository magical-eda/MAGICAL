#ifdef USE_CAIRO
#include "CairoDraw.h"

PROJECT_NAMESPACE_BEGIN

void CairoDraw::openImage(const std::string &imgName, IntType imgW, IntType imgH, IntType mrgW, IntType mrgH, ColorFormat colorFormat,  HexColor bgColor)
{
    destroyImage();

    _imgName = imgName;
    _intlBox.set(0, 0, imgW, imgH);
    _margin = XY<IntType>(mrgW, mrgH);

    if (_imgType == ImageType::PDF)
    {
        _format = ColorFormats::RGB;
    }
    else
    {
        _format = colorFormat;
    }

    switch (_imgType)
    {
        case ImageType::PDF : _surface = cairo_pdf_surface_create(imgName.c_str(), imgW, imgH); break;
        case ImageType::PNG : _surface = cairo_image_surface_create(_format, imgW, imgH); break;
        default             : AssertMsg(false, "Unknown ImageType.\n");
    }
    _cr = cairo_create(_surface);

    // Operator
    setOperator();

/*
    // Draw background color
    auto rgb = hexToRGB(bgColor);
    cairo_set_source_rgba(_cr, rgb.r, rgb.g, rgb.b, _opacity);
    cairo_rectangle(_cr, 0, 0, imgW, imgH);
    cairo_fill(_cr);
    */
}

void CairoDraw::exportImage()
{
    if (_imgType == ImageType::PNG)
    {
        auto res = cairo_surface_write_to_png(_surface, _imgName.c_str());
        AssertMsg(res == CAIRO_STATUS_SUCCESS, "%s", cairo_status_to_string(res));
    }
    destroyImage();
}

void CairoDraw::destroyImage()
{
    if (_surface)
    {
        cairo_surface_destroy(_surface);
        _surface = nullptr;
    }
    if (_cr)
    {
        cairo_destroy(_cr);
        _cr = nullptr;
    }
}

void CairoDraw::drawRectangle(IntType xl, IntType yl, IntType xh, IntType yh, bool fill, IntType lineWidth)
{
    if (xl >= xh || yl >= yh)
    {
        return;
    }

    xl = extlToIntlX(xl);
    yl = extlToIntlY(yl);
    xh = extlToIntlX(xh);
    yh = extlToIntlY(yh);

    cairo_rectangle(_cr, xl, yl, xh - xl, yh - yl);
    if (fill)
    {
        cairo_fill(_cr);
    }
    else
    {
        cairo_set_line_width(_cr, lineWidth);
        cairo_stroke(_cr);
    }
}

PROJECT_NAMESPACE_END
#endif
