/**
 * @file CairoDraw.h
 * @brief Wrapper for Cairo drawing functions
 * @author Keren Zhu
 * @date 01/28/2019
 */

#ifndef GDSROUTE_CAIRODRAW_H_
#define GDSROUTE_CAIRODRAW_H_

#include <cairo/cairo.h>
#include <cairo/cairo-pdf.h>
#include <string>
#include <array>
#include "global/global.h"
#include "util/XY.h"
#include "util/Box.h"

PROJECT_NAMESPACE_BEGIN

/// @class PROJECT_NAMESPACE::CairoDraw
/// @brief A wrapper for Cairo library API
class CairoDraw
{
    public:
        using ColorFormat = cairo_format_t;
        using OperatorType = cairo_operator_t;
        struct ColorFormats
        {
            const static ColorFormat BLACK = cairo_format_t::CAIRO_FORMAT_A1;
            const static ColorFormat GRAY = cairo_format_t::CAIRO_FORMAT_A8;
            const static ColorFormat RGB = cairo_format_t::CAIRO_FORMAT_ARGB32;
        };
        using HexColor = int32_t;

        const static IntType GRAY_SCALE = 255; ///< The scale limit of GRAY

        /// @brief the type of image
        enum class ImageType : Byte
        {
            PNG = 0,
            PDF = 1
        };
        /// @brief Predefined colors
        enum ColorType
        {
            // Basic colors
            BLACK          = 0x000000,
            BLUE           = 0x0000FF,
            BROWN          = 0x996633,
            CYAN           = 0x00FFFF,
            GREEN          = 0x008000,
            MAGENTA        = 0xFF00FF,
            ORANGE         = 0xFF7F00,
            PURPLE         = 0x7F007F,
            RED            = 0xFF0000, 
            YELLOW         = 0xFFFF00, 
            WHITE          = 0xFFFFFF,
            GRAY           = 0x808080,

            // Some other color
            LIME           = 0x00FF00,
            DARKCYAN       = 0x008B8B,
            SKYBLUE        = 0x87CEEB,
            SLATEBLUE      = 0x6A5ACD,
            TOMATO         = 0xFF6347,
            YELLOWGREEN    = 0x9ACD32,
            VIOLET         = 0xEE82EE,
            SILVER         = 0xC0C0C0,
            ORANGERED      = 0xFF4500,
            OLIVE          = 0x808000,
            NAVY           = 0x000080,
            GOLD           = 0xFFD700,
            LIGHTSEAGREEN  = 0x20B2AA,
            DARKSEAGREEN   = 0x8FBC8F,
        };

        /// @brief RGB to represent a color
        struct RGB
        {
            explicit RGB() = default;
            explicit RGB(RealType ri, RealType gi, RealType bi) : r(ri), g(gi), b(bi) {}

            RealType r = 0;
            RealType g = 0;
            RealType b = 0;
        };

    public:
        /// @brief default constructor
        explicit CairoDraw() = default;
        /// @brief constructor
        /// @param the type of image, eg. png
        explicit CairoDraw(ImageType it) : _imgType(it) {}
        /// @brief destructor
        virtual ~CairoDraw() { destroyImage(); }
        /*------------------------------*/ 
        /* Interface for drawing        */
        /*------------------------------*/ 
        void          drawRectangle(IntType xl, IntType yl, IntType xh, IntType yh, bool fill = true, IntType lineWidth = 1);
        /*------------------------------*/ 
        /* Init interface               */
        /*------------------------------*/ 
        void          openImage(const std::string &imgName, IntType imgW = 400, IntType imgH = 600, IntType mrgW = 10, IntType mrgH = 10, ColorFormat colorFormat = ColorFormats::GRAY,  HexColor bgColor = WHITE);
        void          exportImage();
        void          setCanvasBox(IntType xl, IntType yl, IntType xh, IntType yh) { _extlBox.set(xl, yl, xh, yh); }
        void          setCanvasBox(const Box<IntType> &extlBox) { _extlBox = extlBox;  }
        /// @brief set if inverting y
        /// @param whether inverting y
        void          setIfInvertY(bool invertY) { _invertY = invertY; }
        /*------------------------------*/ 
        /* Interface for drawing setting*/
        /*------------------------------*/ 
        /// @brief Set color with hexColor for drawing
        /// @param first: the color
        /// @param second: alpha channel
        void          setColor(HexColor hexColor, RealType opacity); 
        /// @brief Set the gray scale for drawing
        /// @param integer, 0-255
        void          setGrayScale(IntType gray);
        /// @brief set the Cairo operator for overlapping resolving. List of operator see: https://www.cairographics.org/operators/
        /// @param type of operator
        void          setOperator(OperatorType op = OperatorType::CAIRO_OPERATOR_SOURCE) { cairo_set_operator(_cr, op); }
    protected:
        void          destroyImage();

        /// Convert a HEX color to RGB
        RGB           hexToRGB(HexColor hex) const               { return RGB((hex >> 16) / 255.0, ((hex >> 8) % 256) / 255.0, (hex % 256) / 255.0); }
        // Functions that convert X/Y in external coordinate system to X'/Y' in internal coordinate system
        IntType      extlToIntlX(IntType x) const              { return x * (_intlBox.xLen() - 2 * _margin.x()) / _extlBox.xLen() + _margin.x(); }
        IntType      extlToIntlY(IntType y) const 
        { 
            if (_invertY)
            {
                return _intlBox.yHi() + _intlBox.yLo() -  (y * (_intlBox.yLen() - 2 * _margin.y()) / _extlBox.yLen() + _margin.y()); 
            }
            else
            {
                return y * (_intlBox.yLen() - 2 * _margin.y()) / _extlBox.yLen() + _margin.y(); 
            }
        }
        XY<IntType>  extlToIntlXY(IntType x, IntType y) const { return XY<IntType>(extlToIntlX(x), extlToIntlY(y)); }
        XY<IntType>  extlToIntlXY(const XY<IntType> &xy) const { return XY<IntType>(extlToIntlX(xy.x()), extlToIntlY(xy.y())); }

    protected:
        // The output image name and type
        std::string                             _imgName;
        ImageType                               _imgType  = ImageType::PDF;
        ColorFormat                             _format; ///< The color format of the image
        bool                                    _invertY = false; ///< Whether inverting y
        // Canvas size
        Box<IntType>                           _intlBox;            // The box region in internal coordinate system, which defines the actual output image size
        Box<IntType>                           _extlBox;            // The box region in external coordinate system, in which all the drawing APIs will be called
        XY<IntType>                            _margin;             // The X/Y margin in extBox
        // Data members for CAIRO drawing
        cairo_surface_t *                       _surface  = nullptr; ///< Cairo surface 
        cairo_t *                               _cr       = nullptr; ///< Cairo canvas

};

inline void CairoDraw::setColor(HexColor hexColor, RealType opacity)
{
    if (_format != ColorFormats::RGB)
    {
        AssertMsg(false, "%s: Try setting color with unsupport Cairo format !\n", __FUNCTION__);
    }
    auto rgb = hexToRGB(hexColor);
    cairo_set_source_rgba(_cr, rgb.r, rgb.g, rgb.b, opacity);
}

inline void CairoDraw::setGrayScale(IntType gray)
{
    AssertMsg(gray >= 0, "%s: gray scale should be in 0-%d: %d\n", __FUNCTION__, GRAY_SCALE, gray);
    AssertMsg(gray <= GRAY_SCALE, "%s: gray scale should be in 0-%d: %d\n", __FUNCTION__, GRAY_SCALE, gray);
    if (_format != ColorFormats::GRAY)
    {
        AssertMsg(false, "%s: Try setting gray scale with unsupport Cairo format !\n", __FUNCTION__);
    }
    RealType alpha = static_cast<RealType>(gray * PERCENT_ACC / GRAY_SCALE) / PERCENT_ACC;
    cairo_set_source_rgba(_cr, 0, 0, 0, alpha);
}

PROJECT_NAMESPACE_END

#endif //GDSROUTE_CAIRODRAW_H_
