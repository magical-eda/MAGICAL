/**
 * @file   Polygon2Rectangle.h
 * @brief a generic implementation of polygon-to-rectangle conversion 
 *                                                                         
 * K.D.Gourley and D.M. Green, [A Polygon-to-Rectangle Conversion Algorithm](http://doi.ieeecomputersociety.org/10.1109/MCG.1983.262975), IEEE COMP. GRAPHICS & APPLIC. 3.1 (1983): 31-32.
 * 
 * If a contour polygon has duplicate points, remove all of them and form a polygon with holes 
 *
 * @author Yibo Lin
 * @date   Oct 2014
 */

#ifndef _LIMBO_GEOMETRY_POLYGON2RECTANGLE
#define _LIMBO_GEOMETRY_POLYGON2RECTANGLE

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <limbo/geometry/Geometry.h>
#include <limbo/string/String.h>
#include <limbo/preprocessor/AssertMsg.h>

/// @brief namespace for Limbo
namespace limbo 
{ 
/// @brief namespace for Limbo.Geometry
namespace geometry 
{

using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::string;
using std::vector;
using std::map;

/**
 * @class limbo::geometry::point_compare_type
 * @brief sort helper 
 * if orient == HORIZONTAL, sort by left lower 
 * if orient == VERTICAL, sort by lower left 
 */
struct point_compare_type 
{
	orientation_2d m_orient; ///< orientation, HORIZONTAL or VERTICAL 

    /**
     * constructor
     */
	point_compare_type() {}
    /**
     * constructor
     * @param ori orientation 
     */
	point_compare_type(orientation_2d const& ori) : m_orient(ori) {}

    /**
     * @brief compare two points at specific orientation 
     * @tparam PointType point type 
     * @param p1 a point 
     * @param p2 a point 
     * @return true if (orient == HORIZONTAL and p1 is in the left lower of p2) or (orient == VERTICAL and p1 is in the lower left of p2)
     */
	template <typename PointType>
	inline bool operator()(PointType const& p1, PointType const& p2) const 
	{
		assert(m_orient.valid());
		return point_traits<PointType>::get(p1, m_orient) < point_traits<PointType>::get(p2, m_orient)
			|| (point_traits<PointType>::get(p1, m_orient) == point_traits<PointType>::get(p2, m_orient)
					&& point_traits<PointType>::get(p1, m_orient.get_perpendicular()) < point_traits<PointType>::get(p2, m_orient.get_perpendicular())
				);
	}
};

/**
 * @class limbo::geometry::Polygon2Rectangle
 * @brief a class implement conversion from manhattan polygon to rectangle 
 * @tparam PointSet is the container for internal storing vertices of polygon according to some experiments, vector is much faster than list, and set 
 * @tparam RectSet is the container storing output rectangles 
 */
template <typename PointSet,
		 typename RectSet>
class Polygon2Rectangle
{
	public:
		/// @brief internal rectangle set type 
		typedef RectSet rectangle_set_type; 
		/// @brief internal point set type 
		typedef PointSet point_set_type;
		/// @brief internal point type 
		typedef typename container_traits<point_set_type>::value_type point_type;
		/// @brief point set type for polygon 
		typedef typename point_traits<point_type>::coordinate_type coordinate_type;
		/// @brief internal rectangle type 
		typedef typename container_traits<rectangle_set_type>::value_type rectangle_type;
		/// @brief coordinate distance type 
        typedef typename coordinate_traits<coordinate_type>::coordinate_distance coordinate_distance; 
		/// @brief manhattan area type 
        typedef typename coordinate_traits<coordinate_type>::manhattan_area_type manhattan_area_type; 

        /**
         * @brief constructor 
         *
         * if slicing_orient == HORIZONTAL_SLICING or slicing_orient == VERTICAL_SLICING, only 1 copy of points is stored 
         * if slicing_orient == HOR_VER_SLICING, 2 copies of points sorted by different orientation are stored 
         * btw, slicing orientation is perpendicular to its corresponding sorting orientation 
         *
         * @param vRect reference to the container for output rectangles 
         * @param slicing_orient indicates the orientation of slicing 
         */
		Polygon2Rectangle(rectangle_set_type& vRect, slicing_orientation_2d slicing_orient = HORIZONTAL_SLICING)
            : m_mPoint()
            , m_vRect(vRect) 
            , m_slicing_orient(slicing_orient)
		{
			this->initialize(slicing_orient);
		}
        /**
         * @brief constructor 
         * @tparam InputIterator represents the iterator type of point set container for construction only 
         * @param vRect reference to container for rectangles 
         * @param input_begin begin iterator of points 
         * @param input_end end iterator of points 
         * @param slicing_orient slicing orientation 
         */
		template <typename InputIterator>
		Polygon2Rectangle(rectangle_set_type& vRect, InputIterator input_begin, InputIterator input_end, slicing_orientation_2d slicing_orient)
            : m_mPoint()
            , m_vRect(vRect)
            , m_slicing_orient(slicing_orient)
		{
			this->initialize(slicing_orient);
			this->initialize(input_begin, input_end);
		}
        /**
         * @brief initialize polygon points  
         * @tparam InputIterator represents the iterator type of point set container for construction only 
         * @param input_begin begin iterator of points 
         * @param input_end end iterator of points 
         */
		template <typename InputIterator>
		void initialize(InputIterator input_begin, InputIterator input_end)
		{
			// 1. collecting vertices from input container 
			// it should be ordered, clockwise or counterclockwise  
			// identical vertices are skipped 
			// extra vertices in the same line are skipped  
			std::vector<point_type> vTmpPoint;
			// I'm trying to use only forward_iteartor, which only supports ++ operation 
			// so it may spend more effort on getting the last point 
			InputIterator input_last = input_end;
			for (InputIterator itCur = input_begin; itCur != input_end; ++itCur)
			{
				InputIterator itNext = itCur;
				++itNext;
				if (itNext == input_end)
				{
					input_last = itCur;
					break;
				}
			}
			assert_msg(input_last != input_end, "failed to find input_last, maybe too few points");
			if (is_equal_type()(*input_begin, *input_last)) ++input_begin; // skip identical first and last points 
			// use only operator++ so that just forward_iteartor is enough
			for (InputIterator itPrev = input_begin; itPrev != input_end; ++itPrev)
			{
				InputIterator itCur = itPrev;
				++itCur;
				if (itCur == input_end)
					itCur = input_begin;
				InputIterator itNext = itCur;
				++itNext;
				if (itNext == input_end)
					itNext = input_begin;

				// skip identical vertices 
				if (is_equal_type()(*itCur, *itNext))
					continue;

				// skip extra vertices in the same line 
				if (this->get(*itPrev, HORIZONTAL) == this->get(*itCur, HORIZONTAL) 
						&& this->get(*itCur, HORIZONTAL) == this->get(*itNext, HORIZONTAL)) // vertical line 
				{
#ifdef DEBUG
					assert(std::min(this->get(*itPrev, VERTICAL), this->get(*itNext, VERTICAL)) <= this->get(*itCur, VERTICAL)
							&& std::max(this->get(*itPrev, VERTICAL), this->get(*itNext, VERTICAL)) >= this->get(*itCur, VERTICAL));
#endif 
					continue;
				}
				else if (this->get(*itPrev, VERTICAL) == this->get(*itCur, VERTICAL) 
						&& this->get(*itCur, VERTICAL) == this->get(*itNext, VERTICAL)) // horizontal line 
				{
#ifdef DEBUG
					assert(std::min(this->get(*itPrev, HORIZONTAL), this->get(*itNext, HORIZONTAL)) <= this->get(*itCur, HORIZONTAL)
							&& std::max(this->get(*itPrev, HORIZONTAL), this->get(*itNext, HORIZONTAL)) >= this->get(*itCur, HORIZONTAL));
#endif 
					continue;
				}

				// consider edge from itCur to itNext:
				container_traits<std::vector<point_type> >::insert(
						vTmpPoint, 
						point_traits<point_type>::construct(
							this->get(*itCur, HORIZONTAL), 
							this->get(*itCur, VERTICAL)
							));
			}
#ifdef DEBUG
			// a simple manhattan polygon should have even number of different points
			assert(vTmpPoint.size() % 2 == 0);
#endif 

			// copy from vTmpPoint to m_mPoint
			for (typename map<orientation_2d, point_set_type>::iterator it = m_mPoint.begin(); 
					it != m_mPoint.end(); ++it)
			{
				orientation_2d const& orient = it->first;
				point_set_type& vPoint = it->second;

				std::sort(vTmpPoint.begin(), vTmpPoint.end(), point_compare_type(orient));
				// remove points that appear more than once 
				// in other words, after following operation, contour polygon will become polygon with holes
				vPoint.clear();
				//vPoint.reserve(vTmpPoint.size()); // not defined in containers like set 
				for (typename vector<point_type>::iterator itCur = vTmpPoint.begin(); itCur != vTmpPoint.end(); ++itCur)
				{
					typename vector<point_type>::iterator itNext = itCur;
					++itNext;
					if (itNext == vTmpPoint.end())
						itNext = vTmpPoint.begin();
					if (!is_equal_type()(*itCur, *itNext))
						container_traits<point_set_type>::insert(vPoint, vPoint.end(), *itCur); // provide hint for insertion
					else 
					{
						++itCur;
						if (itCur == vTmpPoint.end()) break;
					}
				}
			}
#ifdef DEBUG
			cout << "reduce from " << vTmpPoint.size() << " to " << m_mPoint.begin()->second.size() << endl;
#endif 
		}

        /**
         * @brief top api for @ref limbo::geometry::Polygon2Rectangle
         * @return true if succeed
         */
		bool operator()()
		{
#ifdef _DEBUG_PTR
			std::cout << "debug| PTR::Polygon2Rectangle(), m_vPoint = " << std::endl;
			print("debug.gp");
#endif

			vector<rectangle_type> vRect (m_mPoint.size());

			while (m_mPoint.begin()->second.size() > 0)
			{
				int i = 0;

				for (typename map<orientation_2d, point_set_type>::iterator it = m_mPoint.begin();
						it != m_mPoint.end(); ++it, ++i)
				{
					orientation_2d const& orient = it->first;
#ifdef DEBUG
					point_set_type const& vPoint = it->second; // just for gdb 
                    assert(vPoint.empty() || !vPoint.empty()); // to remove annoying warning 
#endif 

					point_type Pk, Pl, Pm;

					if (!this->find_Pk_Pl_Pm(Pk, Pl, Pm, orient))
						return false;

					rectangle_type& rect = vRect[i];

					// it is safer to do it with construct because some rectangle classes may not support setting borders one-by-one 
					// for HORIZONTAL_SLICING, sort by VERTICAL, Pm decides TOP, Pl decides RIGHT 
					// for VERTICAL_SLICING, sort by HORIZONTAL, Pl decides TOP, Pm decides RIGHT
					rect = rectangle_traits<rectangle_type>::construct(
							this->get(Pk, HORIZONTAL),
							this->get(Pk, VERTICAL),
							std::max(this->get(Pl, HORIZONTAL), this->get(Pm, HORIZONTAL)),
							std::max(this->get(Pl, VERTICAL), this->get(Pm, VERTICAL)));
#ifdef DEBUG
					// check rectangle
					if (this->get(rect, RIGHT) > std::numeric_limits<coordinate_type>::max()-1000
							|| this->get(rect, TOP) > std::numeric_limits<coordinate_type>::max()-1000)
						return false;
#endif 
				}
				// choose rectangle 
				typename vector<rectangle_type>::iterator itRect = vRect.begin();
				for (typename vector<rectangle_type>::iterator it = ++vRect.begin(); it != vRect.end(); ++it)
				{
					// it is possible to try different strategies here 
					// choose the one with heuristic  
                    coordinate_distance w = this->get(*it, RIGHT)-this->get(*it, LEFT); 
                    coordinate_distance h = this->get(*it, TOP)-this->get(*it, BOTTOM); 
                    coordinate_distance wref = this->get(*itRect, RIGHT)-this->get(*itRect, LEFT); 
                    coordinate_distance href = this->get(*itRect, TOP)-this->get(*itRect, BOTTOM); 
                    switch (m_slicing_orient)
                    {
                        case HOR_VER_SLICING:
                        {
                            // compute area 
                            if (w*h > wref*href) // choose large area 
                                itRect = it;
                            break; 
                        }
                        case HOR_VER_SA_SLICING:
                        {
                            // compute area 
                            if (w*h < wref*href) // choose small area 
                                itRect = it;
                            break; 
                        }
                        case HOR_VER_AR_SLICING:
                        {
                            // compute aspect ratio = maxDelta/minDelta
                            coordinate_distance minDelta = w;
                            coordinate_distance maxDelta = h; 
                            coordinate_distance minDeltaRef = wref;
                            coordinate_distance maxDeltaRef = href; 
                            if (minDelta > maxDelta)
                                std::swap(minDelta, maxDelta); 
                            if (minDeltaRef > maxDeltaRef)
                                std::swap(minDeltaRef, maxDeltaRef); 
                            // I rearrage the aspect ratio computation to avoid division 
                            if (maxDelta*minDeltaRef < minDelta*maxDeltaRef) // avoid rectangle with bad aspect ratio 
                                itRect = it;
                            break; 
                        }
                        default:
                            assert_msg(0, "should not reach here " << m_slicing_orient); 
                    }
				}
				// insert or remove point 
				for (typename map<orientation_2d, point_set_type>::iterator it = m_mPoint.begin();
						it != m_mPoint.end(); ++it)
				{
					orientation_2d const& orient = it->first;
#ifdef DEBUG
					point_set_type const& vPoint = it->second; // just for gdb 
                    assert(vPoint.empty() || !vPoint.empty()); // to remove annoying warning 
#endif 

					F(point_traits<point_type>::construct(this->get(*itRect, LEFT), this->get(*itRect, BOTTOM)), orient);
					F(point_traits<point_type>::construct(this->get(*itRect, LEFT), this->get(*itRect, TOP)), orient);
					F(point_traits<point_type>::construct(this->get(*itRect, RIGHT), this->get(*itRect, BOTTOM)), orient);
					F(point_traits<point_type>::construct(this->get(*itRect, RIGHT), this->get(*itRect, TOP)), orient);
				}
				// collect rectangle 
				container_traits<rectangle_set_type>::insert(m_vRect, *itRect);
			}

			return true;
		}
        /**
         * @brief get rectangles 
         * @return result rectangles 
         */
		rectangle_set_type const& get_rectangles() const 
		{
			return m_vRect;
		}
        /**
         * @brief read polygon from file 
         * try to be compatible to gnuplot format 
         * @return true if succeed
         */
		bool read(string const& filename)
		{
			ifstream in (filename.c_str());
			if (!in.good())
			{
				cout << "failed to open " << filename << endl;
				return false;
			}

			string tag;
			string value_str; 
			int status = 0; // status records line number 
							// if status > 0, it means in a polygon scope 
			std::vector<point_type> vPoint; // temporary point set 

			while (!in.eof())
			{
				in >> tag; 
				if (tag == "polygon" && status == 0)
				{
					status = 1;
				}
				else if (tag == "from" || tag == "to")
				{
					int i = 0; 
					coordinate_type value[2] = {0, 0};
					// read until get two numbers 
					// there may be some delimiters like ',' '\' 
					while (!in.eof() && i < 2)
					{
						in >> value_str;
						boost::trim_if(value_str, boost::is_any_of(", \t\\"));
						if (::limbo::is_number(value_str))
						{
							value[i] = boost::lexical_cast<coordinate_type>(value_str);
							// increment i if value_str represents a number 
							i += 1;
						}
					}
					// return false if reading failed 
					if (i != 2) return false; 
					// add current point to point set 
					vPoint.push_back(point_type());
					this->set(vPoint.back(), HORIZONTAL, value[0]);
					this->set(vPoint.back(), VERTICAL, value[1]);
				}
				else if (status == 1)
				{
					status = 0;
					break; // only read one polygon 
				}
			}
			// after reading, call initialize function 
			this->initialize(vPoint.begin(), vPoint.end());

			return true;
		}
        /**
         * @brief print polygon to file in gnuplot format 
         * @param filename output file name 
         */
		void print(string const& filename) const 
		{
			ofstream out (filename.c_str());
			if (!out.good())
			{
				cout << "failed to open " << filename << endl;
				return;
			}
			int i = 1; // gnuplot requires numbering from 1 
			point_set_type const& vPoint = m_mPoint.begin()->second;
			// print current polygon if exists 
			if (vPoint.size() > 0)
			{
				out << "set obj " << i++ << " ";
				out << "polygon \\" << endl;
				for (typename point_set_type::const_iterator it = vPoint.begin(); it != vPoint.end(); ++it)
				{
					if (it == vPoint.begin())
						out << "from " << this->get(*it, HORIZONTAL) << ", " << this->get(*it, VERTICAL) << " \\" << endl;
					else 
						out << "to " << this->get(*it, HORIZONTAL) << ", " << this->get(*it, VERTICAL) << " \\" << endl;
				}
				out << endl;
			}

			// print rectangles 
			for (typename rectangle_set_type::const_iterator it = m_vRect.begin(); it != m_vRect.end(); ++it)
			{
				// print rectangle 
				out << "set obj " << i << " rectangle ";
				out << "from " << this->get(*it, LEFT) << ", " << this->get(*it, BOTTOM) << " "
					<< "to " << this->get(*it, RIGHT) << ", " << this->get(*it, TOP) << " ";
				out << "fc rgb \"dark-grey\" fs transparent pattern 4 bo"<< endl;
				// print text on the center of rectangle 
				out << "set label \'" << i << "\' at " 
					<< (this->get(*it, LEFT)+this->get(*it, RIGHT))/2.0 << ", " << (this->get(*it, BOTTOM)+this->get(*it, TOP))/2.0 << " "
					<< "front center tc rgb \"black\"" << endl;
				++i;
			}
		}

	protected:
        /**
         * @brief get coordinate from point 
         * @param p point 
         * @param o orientation 
         * @return coordinate
         */
		inline coordinate_type get(point_type const& p, orientation_2d o) const {return point_traits<point_type>::get(p, o);}
        /**
         * @brief get coordinate from rectangle  
         * @param r rectangle  
         * @param d direction 
         * @return coordinate
         */
		inline coordinate_type get(rectangle_type const& r, direction_2d d) const {return rectangle_traits<rectangle_type>::get(r, d);}
        /**
         * @brief set coordinate of point 
         * @param p point 
         * @param o orientation 
         * @param v coordinate value
         */
		inline void set(point_type& p, orientation_2d o, coordinate_type v) const {point_traits<point_type>::set(p, o, v);}
        /**
         * @brief set coordinate of rectangle  
         * @param r rectangle 
         * @param d direction
         * @param v coordinate value
         */
		inline void set(rectangle_type& r, direction_2d d, coordinate_type v) const {rectangle_traits<rectangle_type>::set(r, d, v);}

        /**
         * @brief is equal helper 
         */
		struct is_equal_type
		{
            /**
             * @brief is equal helper 
             * @param p1 point 
             * @param p2 point
             * @return true if two points are equal 
             */
			inline bool operator() (point_type const& p1, point_type const& p2) const 
			{
				return point_traits<point_type>::get(p1, HORIZONTAL) == point_traits<point_type>::get(p2, HORIZONTAL) 
					&& point_traits<point_type>::get(p1, VERTICAL) == point_traits<point_type>::get(p2, VERTICAL);
			}
		};
        /**
         * @brief initialize with slicing orientation 
         * it must be called before initializing other data  
         * @param slicing_orient slicing orientation
         */
		void initialize(slicing_orientation_2d slicing_orient)
		{
			switch (slicing_orient)
			{
				case HORIZONTAL_SLICING:
					assert(m_mPoint.insert(make_pair(
									VERTICAL, 
									container_traits<point_set_type>::construct(point_compare_type(VERTICAL))
									)).second);
					break;
				case VERTICAL_SLICING:
					assert(m_mPoint.insert(make_pair(
									HORIZONTAL, 
									container_traits<point_set_type>::construct(point_compare_type(HORIZONTAL))
									)).second);
					break;
				case HOR_VER_SLICING:
                case HOR_VER_SA_SLICING:
                case HOR_VER_AR_SLICING:
					assert(m_mPoint.insert(make_pair(
									VERTICAL, 
									container_traits<point_set_type>::construct(point_compare_type(VERTICAL))
									)).second);
					assert(m_mPoint.insert(make_pair(
									HORIZONTAL, 
									container_traits<point_set_type>::construct(point_compare_type(HORIZONTAL))
									)).second);
					break;
				default:
					cout << "unknown slicing orientation" << endl;
					assert(0);
			}
		}
        /**
         * @brief find Pk, Pl, Pm, please refer to the paper for definition 
         * Given points, find Pk, Pl and Pm
         * @param Pk the leftmost of the lowest points
         * @param Pl the next leftmost of the lowest points
         * @param Pm 1) Xk <= Xm < Xl
         *           2) Ym is lowest but Ym > Yk (Yk == Yl)
         * @param orient orientation 
         */
		bool find_Pk_Pl_Pm(point_type& Pk, point_type& Pl, point_type& Pm, orientation_2d const& orient) 
		{
			point_set_type const& vPoint = m_mPoint[orient];
			if (vPoint.size() < 4)
				return false;

			// always keep it sorted 
			// copy first point to Pk
			this->set(Pk, HORIZONTAL, this->get(*vPoint.begin(), HORIZONTAL));
			this->set(Pk, VERTICAL, this->get(*vPoint.begin(), VERTICAL));
			// copy second point to Pl
			this->set(Pl, HORIZONTAL, this->get(*(++(vPoint.begin())), HORIZONTAL));
			this->set(Pl, VERTICAL, this->get(*(++(vPoint.begin())), VERTICAL));
			// determine Pm
			this->set(Pm, HORIZONTAL, std::numeric_limits<coordinate_type>::max());
			this->set(Pm, VERTICAL, std::numeric_limits<coordinate_type>::max());
			for (typename point_set_type::const_iterator it = vPoint.begin(); it != vPoint.end(); ++it)
			{
				if (this->get(Pk, orient) == this->get(*it, orient))
					continue;
				else if (this->get(Pk, orient.get_perpendicular()) <= this->get(*it, orient.get_perpendicular())
						&& this->get(*it, orient.get_perpendicular()) <= this->get(Pl, orient.get_perpendicular()))
				{
					assert(this->get(*it, orient) > this->get(Pk, orient));
					this->set(Pm, HORIZONTAL, this->get(*it, HORIZONTAL));
					this->set(Pm, VERTICAL, this->get(*it, VERTICAL));
					break;
				}
			}
			if (this->get(Pm, HORIZONTAL) == std::numeric_limits<coordinate_type>::max()
					|| this->get(Pm, VERTICAL) == std::numeric_limits<coordinate_type>::max())
				return false;

			return true;
		}
        /**
         * @brief F function in the original paper 
         * remove point if found, otherwise insert 
         * @param point a point to insert or remove 
         * @param orient orientation 
         */
		void F(point_type const& point, orientation_2d const& orient)
		{
			point_set_type& vPoint = m_mPoint[orient];

			// vPoint is always in order 
			typename point_set_type::iterator itr = std::lower_bound(vPoint.begin(), vPoint.end(), point, point_compare_type(orient));

			if (itr == vPoint.end() || !is_equal_type()(*itr, point)) // not found, insert point 
			{
				container_traits<point_set_type>::insert(vPoint, itr, point);
			}
			else // found, remove point 
			{
				container_traits<point_set_type>::erase(vPoint, itr);
			}
		}
#if 0
		/// @brief maybe useful in get the length of slide line 
		coordinate_type getHorRangeNext(coordinate_type const& x1, coordinate_type const& x2, coordinate_type const& min_y) const
		{
			coordinate_type next_y = std::numeric_limits<coordinate_type>::max() / 2;

			for (typename point_set_type::const_iterator it = m_vPoint.begin(); it != m_vPoint.end(); ++it)
			{
				coordinate_type x = this->get(*it, HORIZONTAL), y = this->get(*it, VERTICAL);
				if (x<x1 || x>x2) continue;
				if (y <= min_y)   continue;
				if (y >= next_y)  continue;  

				next_y = y;
			}

			return next_y;
		}

		coordinate_type getVerRangeNext(coordinate_type const& y1, coordinate_type const& y2, coordinate_type const& min_x) const
		{
			coordinate_type next_x = std::numeric_limits<coordinate_type>::max() / 2;

			for (typename point_set_type::const_iterator it = m_vPoint.begin(); it != m_vPoint.end(); ++it)
			{
				coordinate_type x = this->get(*it, HORIZONTAL), y = this->get(*it, VERTICAL);
				if (y<y1 || y>y2) continue;
				if (x <= min_x)   continue;
				if (x >= next_x)  continue;

				next_x = x;
			}

			return next_x;
		}
#endif 
		map<orientation_2d, point_set_type> m_mPoint; ///< save all points of a polygon 
													///< 1~2 copies, sort by left lower or by lower left 
													///< for HORIZONTAL key, sort by left lower 
													///< for VERTICAL key, sort by lower left 
		rectangle_set_type& m_vRect; ///< save all rectangles from conversion 
        slicing_orientation_2d m_slicing_orient; ///< slicing orient
};

} // namespace geometry
} // namespace limbo

// a specialization for vectors 
#include <limbo/geometry/Polygon2RectangleVec.h>

namespace limbo 
{ 
namespace geometry 
{

/// @brief standby function for polygon-to-rectangle conversion 
/// @tparam InputIterator represents the input iterators for points of polygon 
/// @tparam PointSet represents the internal container for points of polygon, user needs to pass a hint for type deduction 
/// @tparam RectSet represents the container for rectangles 
/// @param input_begin begin iterator of input 
/// @param input_end end iterator of input 
/// @param r reference to container for rectangles 
/// @param slicing_orient slicing orientations 
/// @return true if succeed 
template <typename InputIterator, typename PointSet, typename RectSet>
inline bool polygon2rectangle(InputIterator input_begin, InputIterator input_end, 
		PointSet const&, RectSet& r, slicing_orientation_2d slicing_orient = HORIZONTAL_SLICING)
{
	Polygon2Rectangle<PointSet, RectSet> p2r (r, input_begin, input_end, slicing_orient);
	if (!p2r()) return false;
	return true;
}

} // namespace geometry
} // namespace limbo

#endif 
