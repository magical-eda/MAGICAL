/**
 * @file   box.hpp
 * @brief  Geometric Data Structure: Box type
 * @author Hao Chen
 * @date   09/05/2019
 *
 **/

#ifndef _GEO_BOX_HPP_
#define _GEO_BOX_HPP_

#include "src/global/global.hpp"
#include "point.hpp"

PROJECT_NAMESPACE_START

template<typename T>
class Box {
public:
  Box(T l = 0, T b = 0, T r = 0, T t = 0)
    : _bl(l, b), _tr(r, t) {
    assert(l <= r && b <= t);
  }
  Box(const Point<T>& p1, const Point<T>& p2) {
    assert(p1.x() <= p2.x() && p1.y() <= p2.y());
    _bl = p1;
    _tr = p2;
  }
  ~Box() {}

  // Basic setting functions
  void  setXL(T l)                      { _bl.setX(l); }
  void  setXH(T r)                      { _tr.setX(r); }
  void  setYL(T b)                      { _bl.setY(b); }
  void  setYH(T t)                      { _tr.setY(t); }
  void  setBounds(T l, T b, T r, T t)   { setXL(l); setYL(b); setXH(r); setYH(t); }
  // Basic access functions
  T          xl()               const { return _bl.x(); }
  T          yl()               const { return _bl.y(); }
  T          xh()               const { return _tr.x(); }
  T          yh()               const { return _tr.y(); }
  T          width()            const { return xh() - xl(); }
  T          height()           const { return yh() - yl(); }
  T          centerX()          const { return (xl() + xh()) / 2; }
  T          centerY()          const { return (yl() + yh()) / 2; }
  T          hpwl()             const { return width() + height(); }
  T          perimeter()        const { return 2 * hpwl(); }
  T          area()             const { return width() * height(); }
  
  // Points
  Point<T>&         bl()               { return _bl; }
  const Point<T>&   bl()         const { return _bl; }
  Point<T>&         tr()               { return _tr; }
  const Point<T>&   tr()         const { return _tr; }
  Point<T>&         min_corner()       { return _bl; }
  const Point<T>&   min_corner() const { return _bl; }
  Point<T>&         max_corner()       { return _tr; }
  const Point<T>&   max_corner() const { return _tr; }
  Point<T>          center()     const { return Point<T>(centerX(), centerY());  }

  // utils
  void shiftX(const T x);
  void shiftY(const T y);
  void shift(const T x, const T y);
  void rotate90(const T x, const T y, const bool bClockWise); // rotate 90 degree with respect to (x, y)
  void rotate180(const T x, const T y);                       // rotate 180 degree with respect to (x, y)
  void flipX(const T x);                                      // flip by line x = x
  void flipY(const T y);                                      // flip by line y = y
  void expand(const T s);
  void expand(const T s, const int dim);
  void expandX(const T s);
  void expandY(const T s);
  void shrink(const T s);
  void shrinkX(const T s);
  void shrinkY(const T s);
  void difference(const Box& r, Vector_t<Box>& result);
  void multi_diff(const Vector_t<Box>& vBox, Vector_t<Box>& result) const;
  void multi_diff(const std::list<const Box*>& vBox, Vector_t<Box>& result) const;
  void multi_diff(const std::list<UInt_t>& ord, const Vector_t<Box>& vBox, Vector_t<Box>& result) const;

  //static functions
  static T     Mdistance(const Box& box1, const Box& box2);
  static T     Mcenterdistance(const Box& box1, const Box& box2);
  static T     Mdistance(const Box& box1, const Point<T>& pt);
  static bool  bOverlap(const Box& box1, const Box& box2);
  static bool  bConnect(const Box& box1, const Box& box2);
  static bool  bCover(const Box& box1, const Box& box2);
  static bool  bInside(const Box& box, const Point<T>& pt);
  static bool  bConnect(const Box& box, const Point<T>& pt);
  static T     overlapArea(const Box& box1, const Box& box2);
  static void  intersection(const Box& box1, const Box& box2, Vector_t<Box>& result);
  static void  intersection2(const Box& box1, const Box& box2, Vector_t<Box>& result);
  static void  difference2(const Box& box1, const Box& box2, Vector_t<Box>& result);

  //operator
  bool operator < (const Box<T>& box) const {
    if (_bl.x() != box.xl()) return _bl.x() < box.xl();
    if (_bl.y() != box.yl()) return _bl.y() < box.yl();
    if (_tr.x() != box.xh()) return _tr.x() < box.xh();
    return _tr.y() < box.yh();
  }
  bool operator == (const Box<T>& box) const {
    return _bl.x() == box.xl() && _bl.y() == box.yl() && _tr.x() == box.xh() && _tr.y() == box.yh();
  }

  friend std::ostream& operator << (std::ostream& os, const Box& r) {
    os << '(' << r._bl.x() << ' ' << r._bl.y() << ' ' << r._tr.x() << ' ' << r._tr.y() << ')';
    return os;
  }

  //Debugging
  void printBoxInfo() const {
    std::cout << "  Bound : " << '(' << _bl.x() << ' ' << _bl.y() << ' ' << _tr.x() << ' ' << _tr.y() << ')';
    std::cout << "  CenterXY : " << ' ' << (_bl.x() + _tr.x()) / 2  << ' ' << (_bl.y() + _tr.y()) / 2 << ')' << std::endl;
  }

private:
  Point<T> _bl; // bottom left
  Point<T> _tr; // top right
};

// member fucntions definition
template<typename T>
void Box<T>::shiftX(const T x) {
  setXL(_bl.x() + x);
  setXH(_tr.x() + x);
}

template<typename T>
void Box<T>::shiftY(const T y) {
  setYL(_bl.y() + y);
  setYH(_tr.y() + y);
}

template<typename T>
void Box<T>::shift(const T x, const T y) {
  shiftX(x);
  shiftY(y);
}

template<typename T>
void Box<T>::rotate90(const T x, const T y, const bool bClockWise) {
  Point<T> p1(_bl);
  Point<T> p2(_tr);
  p1.rotate90(x, y, bClockWise);
  p2.rotate90(x, y, bClockWise);
  setXL(std::min(p1.x(), p2.x()));
  setYL(std::min(p1.y(), p2.y()));
  setXH(std::max(p1.x(), p2.x()));
  setYH(std::max(p1.y(), p2.y()));
}

template<typename T>
void Box<T>::rotate180(const T x, const T y) {
  Point<T> p1(_bl);
  Point<T> p2(_tr);
  p1.rotate180(x, y);
  p2.rotate180(x, y);
  setXL(std::min(p1.x(), p2.x()));
  setXH(std::max(p1.x(), p2.x()));
  setYL(std::min(p1.y(), p2.y()));
  setYH(std::max(p1.y(), p2.y()));
}

template<typename T>
void Box<T>::flipX(const T x) {
  int l = _bl.x();
  int r = _tr.x();
  setXL(x + (x - r));
  setXH(x + (x - l));
}

template<typename T>
void Box<T>::flipY(const T y) {
  int b = _bl.y();
  int t = _tr.y();
  setYL(y + (y - t));
  setYH(y + (y - b));
}

template<typename T>
void Box<T>::expand(const T s) {
  setXL(_bl.x() - s);
  setYL(_bl.y() - s);
  setXH(_tr.x() + s);
  setYH(_tr.y() + s);
}

template<typename T>
void Box<T>::expand(const T s, const int dim) {
  if (dim == 0) {
    setXL(_bl.x() - s);
    setXH(_tr.x() + s);
  }
  else {
    assert(dim == 1);
    setYL(_bl.y() - s);
    setYH(_bl.y() + s);
  }
}

template<typename T>
void Box<T>::expandX(const T s) {
  setXL(_bl.x() - s);
  setXH(_tr.x() + s);
}

template<typename T>
void Box<T>::expandY(const T s) {
  setYL(_bl.y() - s);
  setYH(_bl.y() + s);
}

template<typename T>
void Box<T>::shrink(const T s) {
  setXL(_bl.x() + s);
  setYL(_bl.y() + s);
  setXH(_tr.x() - s);
  setYH(_tr.y() - s);
}

template<typename T>
void Box<T>::shrinkX(const T s) {
  setXL(_bl.x() + s);
  setXH(_tr.x() - s);
}

template<typename T>
void Box<T>::shrinkY(const T s) {
  setYL(_bl.y() + s);
  setYH(_tr.y() - s);
}

template<typename T>
void Box<T>::difference(const Box<T>& r, Vector_t<Box<T> >& result) {
  T a = std::min(_bl.x(), r._bl.x());
  T b = std::max(_bl.x(), r._bl.x());
  T c = std::min(_tr.x(), r._tr.x());
  T d = std::max(_tr.x(), r._tr.x());

  T e = std::min(_bl.y(), r._bl.y());
  T f = std::max(_bl.y(), r._bl.y());
  T g = std::min(_tr.y(), r._tr.y());
  T h = std::max(_tr.y(), r._tr.y());

  if (f - e > 0) {
    if (b - a > 0) {
      Box<T> tmp(a, e, b, f);
      if (bOverlap(*this, tmp))
        result.push_back(tmp);
    }
    if (c - b > 0) {
      Box<T> tmp(b, e, c, f);
      if (bOverlap(*this, tmp))
        result.push_back(tmp);
    }
    if (d - c > 0) {
      Box<T> tmp(c, e, d, f);
      if (bOverlap(*this, tmp))
        result.push_back(tmp);
    }
  }
  if (g - f > 0) {
    if (b - a > 0) {
      Box<T> tmp(a, f, b, g);
      if (bOverlap(*this, tmp))
        result.push_back(tmp);
    }
    if (d - c > 0) {
      Box<T> tmp(c, f, d, g);
      if (bOverlap(*this, tmp))
        result.push_back(tmp);
    }
  }
  if (h - g > 0) {
    if (b - a > 0) {
      Box<T> tmp(a, g, b, h);
      if (bOverlap(*this, tmp))
        result.push_back(tmp);
    }
    if (c - b > 0) {
      Box<T> tmp(b, g, c, h);
      if (bOverlap(*this, tmp))
        result.push_back(tmp);
    }
    if (d - c > 0) {
      Box<T> tmp(c, g, d, h);
      if (bOverlap(*this, tmp))
        result.push_back(tmp);
    }
  }
}

template<typename T>
void Box<T>::multi_diff(const Vector_t<Box<T>>& vBox, Vector_t<Box<T>>& result) const {
  Vector_t<T> hor = { _bl.y(), _tr.y() };
  Vector_t<T> ver = { _bl.x(), _tr.x() };
  for (UInt_t i = 0; i < vBox.size(); ++i) {
    Box<T>& box= vBox[i];
    if (box.xl() > _bl.x() && box.xl() < _tr.x())
      ver.push_back(box.xl());
    if (box.xh() > _bl.x() && box.xh() < _tr.x())
      ver.push_back(box.xh());
    if (box.yl() > _bl.y() && box.yl() < _tr.y())
      hor.push_back(box.yl());
    if (box.yh() > _bl.y() && box.yh() < _tr.y())
      hor.push_back(box.yh());
  }
  std::sort(hor.begin(), hor.end());
  std::sort(ver.begin(), ver.end());
  hor.resize(std::unique(hor.begin(), hor.end()) - hor.begin());
  ver.resize(std::unique(ver.begin(), ver.end()) - ver.begin());
  for (UInt_t i = 1; i < ver.size(); ++i)
    for (UInt_t j = 1; j < hor.size(); ++j)
      result.push_back(Box(ver[i - 1], hor[j - 1], ver[i], hor[j]));
}

template<typename T>
void Box<T>::multi_diff(const std::list<UInt_t>& ord, const Vector_t<Box<T>>& vBox, Vector_t<Box<T>>& result) const {
  Vector_t<T> hor = { _bl.y(), _tr.y() };
  Vector_t<T> ver = { _bl.x(), _tr.x() };
  for (UInt_t i : ord) {
    const Box<T>& box = vBox[i];
    if (box.xl() > _bl.x() && box.xl() < _tr.x())
      ver.push_back(box.xl());
    if (box.xh() > _bl.x() && box.xh() < _tr.x())
      ver.push_back(box.xh());
    if (box.yl() > _bl.y() && box.yl() < _tr.y())
      hor.push_back(box.yl());
    if (box.yh() > _bl.y() && box.yh() < _tr.y())
      hor.push_back(box.yh());
  }
  std::sort(hor.begin(), hor.end());
  std::sort(ver.begin(), ver.end());
  hor.resize(std::unique(hor.begin(), hor.end()) - hor.begin());
  ver.resize(std::unique(ver.begin(), ver.end()) - ver.begin());
  for (UInt_t i = 1; i < ver.size(); ++i)
    for (UInt_t j = 1; j < hor.size(); ++j)
      result.push_back(Box(ver[i - 1], hor[j - 1], ver[i], hor[j]));
}

template<typename T>
void Box<T>::multi_diff(const std::list<const Box<T>*>& vBox, Vector_t<Box<T>>& result) const {
  Vector_t<T> hor = { _bl.y(), _tr.y() };
  Vector_t<T> ver = { _bl.x(), _tr.x() };
  for (const Box<T>* box: vBox) {
    if (box->xl() > _bl.x() && box->xl() < _tr.x())
      ver.push_back(box->xl());
    if (box->xh() > _bl.x() && box->xh() < _tr.x())
      ver.push_back(box->xh());
    if (box->yl() > _bl.y() && box->yl() < _tr.y())
      hor.push_back(box->yl());
    if (box->yh() > _bl.y() && box->yh() < _tr.y())
      hor.push_back(box->yh());
  }
  std::sort(hor.begin(), hor.end());
  std::sort(ver.begin(), ver.end());
  hor.resize(std::unique(hor.begin(), hor.end()) - hor.begin());
  ver.resize(std::unique(ver.begin(), ver.end()) - ver.begin());
  for (UInt_t i = 1; i < ver.size(); ++i)
    for (UInt_t j = 1; j < hor.size(); ++j)
      result.push_back(Box<T>(ver[i - 1], hor[j - 1], ver[i], hor[j]));
}

// static functions
template<typename T>
T Box<T>::Mdistance(const Box<T>& box1, const Box<T>& box2) {
  T d1 = max({box1._bl.x() - box2._tr.x(), box2._bl.x() - box1._tr.x(), (T)0});
  T d2 = max({box1._bl.y() - box2._tr.y(), box2._bl.y() - box1._tr.y(), (T)0});
  return d1 + d2;
}

template<typename T>
T Box<T>::Mcenterdistance(const Box<T>& box1, const Box<T>& box2) {
  return abs((box1.xl() + box1.xh()) / 2 - (box2.xl() + box2.xh()) / 2)
       + abs((box1.yl() + box1.yh()) / 2 - (box2.yl() + box2.yh()) / 2);
}

template<typename T>
T Box<T>::Mdistance(const Box<T>& box, const Point<T>& pt) {
  T d1 = 0, d2 = 0;
  if (pt.x() < box.xl())
    d1 = box.xl() - pt.x();
  else if (pt.x() > box.xh())
    d1 = pt.x() - box.xh();
  if (pt.y() < box.yl())
    d2 = box.yl() - pt.y();
  else if
    (pt.y() > box.yh()) d2 = pt.y() - box.yh();
  return d1 + d2;
}

template<typename T>
bool Box<T>::bOverlap(const Box<T>& box1, const Box<T>& box2) {
  if (box1._bl.y() >= box2._tr.y()) return false;
  if (box1._tr.y() <= box2._bl.y()) return false;
  if (box1._bl.x() >= box2._tr.x()) return false;
  if (box1._tr.x() <= box2._bl.x()) return false;
  return true;
}

template<typename T>
bool Box<T>::bConnect(const Box<T>& box1, const Box<T>& box2) {
  if (box1._bl.y() > box2._tr.y()) return false;
  if (box1._tr.y() < box2._bl.y()) return false;
  if (box1._bl.x() > box2._tr.x()) return false;
  if (box1._tr.x() < box2._bl.x()) return false;
  return true;
}

template<typename T>
bool Box<T>::bCover(const Box& box1, const Box& box2) {
  return bConnect(box1, box2.min_corner()) &&
         bConnect(box1, box2.max_corner());
}

template<typename T>
bool Box<T>::bInside(const Box<T>& box, const Point<T>& pt) {
  return (pt.x() > box.xl() && pt.x() < box.xh() &&
          pt.y() > box.yl() && pt.y() < box.yh());
}

template<typename T>
bool Box<T>::bConnect(const Box<T>& box, const Point<T>& pt) {
  return (pt.x() >= box.xl() && pt.x() <= box.xh() &&
          pt.y() >= box.yl() && pt.y() <= box.yh());
}

template<typename T>
T Box<T>::overlapArea(const Box<T>& box1, const Box<T>& box2) {
  T overlapH = std::min(box1._tr.x(), box2._tr.x()) - std::max(box1._bl.x(), box2._bl.x());
  T overlapV = std::min(box1._tr.y(), box2._tr.y()) - std::max(box1._bl.y(), box2._bl.y());
  if (overlapH <= 0 || overlapV <= 0)
    return 0;
  return overlapH * overlapV;
}

template<typename T>
void Box<T>::intersection(const Box<T>& box1, const Box<T>& box2, Vector_t<Box<T> >& result) {
  if (!bOverlap(box1, box2))
    return;
  Box<T> box;
  box.setXL(std::max(box1._bl.x(), box2._bl.x()));
  box.setXH(std::min(box1._tr.x(), box2._tr.x()));
  box.setYL(std::max(box1._bl.y(), box2._bl.y()));
  box.setYH(std::min(box1._tr.y(), box2._tr.y()));
  result.push_back(box);
}

template<typename T>
void Box<T>::intersection2(const Box<T>& box1, const Box<T>& box2, Vector_t<Box<T> >& result) {
  if (!bConnect(box1, box2))
    return;
  Box<T> box;
  box.setXL(std::max(box1._bl.x(), box2._bl.x()));
  box.setXH(std::min(box1._tr.x(), box2._tr.x()));
  box.setYL(std::max(box1._bl.y(), box2._bl.y()));
  box.setYH(std::min(box1._tr.y(), box2._tr.y()));
  result.push_back(box);
}

template<typename T>
void Box<T>::difference2(const Box<T>& box1, const Box<T>& box2, Vector_t<Box<T> >& result) {
  // X = intersection, 0-7 = possible difference areas
  // h +-+-+-+
  // . |5|6|7|
  // g +-+-+-+
  // . |3|X|4|
  // f +-+-+-+
  // . |0|1|2|
  // e +-+-+-+
  // . a b c d
  T a = std::min(box1._bl.x(), box2._bl.x());
  T b = std::max(box1._bl.x(), box2._bl.x());
  T c = std::min(box1._tr.x(), box2._tr.x());
  T d = std::max(box1._tr.x(), box2._tr.x());

  T e = std::min(box1._bl.y(), box2._bl.y());
  T f = std::max(box1._bl.y(), box2._bl.y());
  T g = std::min(box1._tr.y(), box2._tr.y());
  T h = std::max(box1._tr.y(), box2._tr.y());

  if (f - e > 0) {
    if (b - a > 0) {
      Box<T> tmp(a, e, b, f);
      if (bOverlap(box1, tmp) || bOverlap(box2, tmp))
        result.push_back(tmp);
    }
    if (c - b > 0) {
      Box<T> tmp(b, e, c, f);
      if (bOverlap(box1, tmp) || bOverlap(box2, tmp))
        result.push_back(tmp);
    }
    if (d - c > 0) {
      Box<T> tmp(c, e, d, f);
      if (bOverlap(box1, tmp) || bOverlap(box2, tmp))
        result.push_back(tmp);
    }
  }
  if (g - f > 0) {
    if (b - a > 0) {
      Box<T> tmp(a, f, b, g);
      if (bOverlap(box1, tmp) || bOverlap(box2, tmp))
        result.push_back(tmp);
    }
    if (d - c > 0) {
      Box<T> tmp(c, f, d, g);
      if (bOverlap(box1, tmp) || bOverlap(box2, tmp))
        result.push_back(tmp);
    }
  }
  if (h - g > 0) {
    if (b - a > 0) {
      Box<T> tmp(a, g, b, h);
      if (bOverlap(box1, tmp) || bOverlap(box2, tmp))
        result.push_back(tmp);
    }
    if (c - b > 0) {
      Box<T> tmp(b, g, c, h);
      if (bOverlap(box1, tmp) || bOverlap(box2, tmp))
        result.push_back(tmp);
    }
    if (d - c > 0) {
      Box<T> tmp(c, g, d, h);
      if (bOverlap(box1, tmp) || bOverlap(box2, tmp))
        result.push_back(tmp);
    }
  }
}

PROJECT_NAMESPACE_END

#endif /// _GEO_BOX_HPP_
