/**
 * @file   Polygon2RectangleVec.h
 * @brief  a template specialization for std::vector.  
 * It is more efficient than generic version 
 * @author Yibo Lin
 * @date   May 2016
 */

#ifndef _LIMBO_GEOMETRY_POLYGON2RECTANGLEVEC_H
#define _LIMBO_GEOMETRY_POLYGON2RECTANGLEVEC_H

#include <vector>
#include <limits>

/// @brief namespace for Limbo
namespace limbo 
{ 
/// @brief namespace for Limbo.Geometry
namespace geometry 
{

// forward declaration 
struct point_compare_type; 
template <typename PointSet,
		 typename RectSet>
class Polygon2Rectangle; 

/// @brief a class implement conversion from manhattan polygon to rectangle 
/// @param PointSet is the container for internal storing vertices of polygon 
/// according to some experiments, std::vector is much faster than list, and set 
/// @param RectSet is the container storing output rectangles 
template <typename PointType,
		 typename RectangleType>
class Polygon2Rectangle<std::vector<PointType>, std::vector<RectangleType> >
{
	public:
		/// @brief internal rectangle set type 
		typedef std::vector<RectangleType> rectangle_set_type; 
		/// @brief internal point set type 
		typedef std::vector<PointType> point_set_type;
		/// @brief internal point type 
		//typedef typename polygon_90_traits<polygon_type>::point_type point_type;
		typedef typename container_traits<point_set_type>::value_type point_type;
		/// @brief point set type for polygon 
		//typedef typename polygon_90_traits<polygon_type>::coordinate_type coordinate_type;
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
            : m_mPoint((slicing_orient != HORIZONTAL_SLICING && slicing_orient != VERTICAL_SLICING)? 2 : 1)
            , m_vOrient2Id(2, std::numeric_limits<unsigned char>::max())
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
            : m_mPoint((slicing_orient != HORIZONTAL_SLICING && slicing_orient != VERTICAL_SLICING)? 2 : 1)
            , m_vOrient2Id(2, std::numeric_limits<unsigned char>::max())
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
            assert(input_begin != input_end); 
			// 1. collecting vertices from input container 
			// it should be ordered, clockwise or counterclockwise  
			// identical vertices are skipped 
			// extra vertices in the same line are skipped  
			std::vector<point_type> vTmpPoint;
            InputIterator input_last = input_begin; 
            std::size_t dist = std::distance(input_begin, input_end); 
            std::advance(input_last, dist-1); 
			if (is_equal_type()(*input_begin, *input_last)) // skip identical first and last points 
            {
                ++input_begin; 
                --dist;
            }
            vTmpPoint.reserve(dist); // reserve memory 
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
            std::sort(vTmpPoint.begin(), vTmpPoint.end(), point_compare_type(m_mPoint.front().first));
            point_set_type& vPointFront = m_mPoint.front().second; 
            // remove points that appear more than once 
            // in other words, after following operation, contour polygon will become polygon with holes
            vPointFront.reserve(vTmpPoint.size()); // not defined in containers like set 
            for (typename std::vector<point_type>::iterator itCur = vTmpPoint.begin(), itCure = vTmpPoint.end(); itCur != itCure; ++itCur)
            {
                typename std::vector<point_type>::iterator itNext = itCur;
                ++itNext;
                if (itNext == itCure)
                    itNext = vTmpPoint.begin();
                if (!is_equal_type()(*itCur, *itNext))
                    vPointFront.push_back(*itCur); 
                else 
                {
                    ++itCur;
                    if (itCur == itCure) break;
                }
            }

            // copy the vTmpPoint to the rest entries 
            for (std::size_t i = 1, ie = m_mPoint.size(); i < ie; ++i)
            {
                point_set_type const& vPointPrev = m_mPoint[i-1].second; 
                orientation_2d const& orient = m_mPoint[i].first; 
                point_set_type& vPoint = m_mPoint[i].second; 

                if (i == 1) // make use of the memory already allocated by vTmpPoint 
                    vPoint.swap(vTmpPoint); 
                // copy 
                vPoint.assign(vPointPrev.begin(), vPointPrev.end()); 
                // sort 
				std::sort(vPoint.begin(), vPoint.end(), point_compare_type(orient));
            }
		}

        /**
         * @brief top api for @ref limbo::geometry::Polygon2Rectangle
         * @return true if succeed
         */
		bool operator()()
		{
			std::vector<rectangle_type> vRect (m_mPoint.size());

			while (!m_mPoint.front().second.empty())
			{
				int i = 0;

				for (typename std::vector<std::pair<orientation_2d, point_set_type> >::const_iterator it = m_mPoint.begin();
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
				typename std::vector<rectangle_type>::iterator itRect = vRect.begin();
				for (typename std::vector<rectangle_type>::iterator it = ++vRect.begin(); it != vRect.end(); ++it)
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
				for (typename std::vector<std::pair<orientation_2d, point_set_type> >::iterator it = m_mPoint.begin();
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
				for (typename point_set_type::const_iterator it = vPoint.begin(), ite = vPoint.end(); it != ite; ++it)
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
                // only need to set orient, the initialization of point array is done implicitly
				case HORIZONTAL_SLICING:
					m_mPoint.at(0).first = VERTICAL; 
                    m_vOrient2Id[VERTICAL] = 0;
					break;
				case VERTICAL_SLICING:
					m_mPoint.at(0).first = HORIZONTAL; 
                    m_vOrient2Id[HORIZONTAL] = 0;
					break;
				case HOR_VER_SLICING:
                case HOR_VER_SA_SLICING:
                case HOR_VER_AR_SLICING:
					m_mPoint.at(0).first = HORIZONTAL; // since HORIZONTAL is 0
                    m_vOrient2Id[HORIZONTAL] = 0;
					m_mPoint.at(1).first = VERTICAL; // since VERTICAL is 1
                    m_vOrient2Id[VERTICAL] = 1; 
					break;
				default:
                    std::cout << "unknown slicing orientation" << std::endl;
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
			point_set_type const& vPoint = m_mPoint.at(m_vOrient2Id[orient.to_int()]).second;
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
			for (typename point_set_type::const_iterator it = vPoint.begin(), ite = vPoint.end(); it != ite; ++it)
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
			point_set_type& vPoint = m_mPoint.at(m_vOrient2Id[orient.to_int()]).second;

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
        std::vector<std::pair<orientation_2d, point_set_type> > m_mPoint; ///< save all points of a polygon 
                                                                        ///< 1~2 copies, sort by left lower or by lower left 
                                                                        ///< for HORIZONTAL key, sort by left lower 
                                                                        ///< for VERTICAL key, sort by lower left 
        std::vector<unsigned char>  m_vOrient2Id; ///< orientation_2d to index of m_mPoint
		rectangle_set_type& m_vRect; ///< save all rectangles from conversion 
        slicing_orientation_2d m_slicing_orient; ///< slicing orient
};

} // namespace geometry
} // namespace limbo

#endif
