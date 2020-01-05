/**
 * @file   invertaltree.hpp
 * @brief  Geometric Data Structure: Invertal Tree
 * @author Hao Chen
 * @date   09/06/2019
 *
 **/

#ifndef _GEO_INTERVALTREE_HPP_
#define _GEO_INTERVALTREE_HPP_

#include <ext/pb_ds/assoc_container.hpp>

#include "src/global/global.hpp"


PROJECT_NAMESPACE_START

template<typename T, typename U>
struct my_node_update {

  template<typename Node_CItr, typename Node_Itr, typename Cmp_Fn, typename _Alloc>
  class intervals_node_update {
  public:
    typedef T metadata_type;
    void query_all(const Pair_t<T, T> q_interval, std::list<Pair_t<Pair_t<T, T>, U> > &l, bool open=true) const {
      if (q_interval.first == q_interval.second)
        contain_point_all(node_begin(), q_interval.first, l);
      else if (open)
        overlaps_all(node_begin(), q_interval, l); 
      else
        intersects_all(node_begin(), q_interval, l);
    }
    void query_interval(const Pair_t<T, T> q_interval, std::list<Pair_t<T, T> > &l, bool open=true) const {
      if (q_interval.first == q_interval.second)
        contain_point_interval(node_begin(), q_interval.first, l);
      else if (open)
        overlaps_interval(node_begin(), q_interval, l); 
      else
        intersects_interval(node_begin(), q_interval, l);
    }
    void query(const Pair_t<T, T> q_interval, std::list<U> &l, bool open=true) const {
      if (q_interval.first == q_interval.second)
        contain_point(node_begin(), q_interval.first, l);
      else if (open)
        overlaps(node_begin(), q_interval, l); 
      else
        intersects(node_begin(), q_interval, l);
    }
    void query(const T q_pt, std::list<U> &l) const { contain_point(node_begin(), q_pt, l); }

  protected:
    inline void operator() (Node_Itr nd_it, Node_CItr end_nd_it) {
      const T l_max_endpoint = nd_it.get_l_child() == end_nd_it ? 0 : nd_it.get_l_child().get_metadata();
      const T r_max_endpoint = nd_it.get_r_child() == end_nd_it ? 0 : nd_it.get_r_child().get_metadata();
      const_cast<T&>(nd_it.get_metadata()) = max({(*nd_it)->first.second, l_max_endpoint, r_max_endpoint});
    }

    virtual Node_CItr node_begin() const = 0;
    virtual Node_CItr node_end() const = 0;
    virtual ~intervals_node_update() {}

  private:
    void overlaps(Node_CItr nd_it, const Pair_t<T, T> q_interval, std::list<U>& l) const {
      if (nd_it == node_end())
        return;
      if (q_interval.first >= nd_it.get_metadata())
        return;
      overlaps(nd_it.get_l_child(), q_interval, l);
      if ((*nd_it)->first.first < q_interval.second && q_interval.first < (*nd_it)->first.second)
        l.push_front((*nd_it)->second);
      if (q_interval.second <= (*nd_it)->first.first)
        return;
      overlaps(nd_it.get_r_child(), q_interval, l);
    }
    void intersects(Node_CItr nd_it, const Pair_t<T, T> q_interval, std::list<U>& l) const {
      if (nd_it == node_end())
        return;
      if (q_interval.first > nd_it.get_metadata())
        return;
      intersects(nd_it.get_l_child(), q_interval, l);
      if ((*nd_it)->first.first <= q_interval.second && q_interval.first <= (*nd_it)->first.second)
        l.push_front((*nd_it)->second);
      if (q_interval.second < (*nd_it)->first.first)
        return;
      intersects(nd_it.get_r_child(), q_interval, l);
    }
    void contain_point(Node_CItr nd_it, const T q_pt, std::list<U> &l) const {
      if (nd_it == node_end())
        return;
      if (q_pt > nd_it.get_metadata())
        return;
      contain_point(nd_it.get_l_child(), q_pt, l);
      if ((*nd_it)->first.first <= q_pt && q_pt <= (*nd_it)->first.second)
        l.push_front((*nd_it)->second);
      if (q_pt < (*nd_it)->first.first)
        return;
      contain_point(nd_it.get_r_child(), q_pt, l);
    }
    void overlaps_interval(Node_CItr nd_it, const Pair_t<T, T> q_interval, std::list<Pair_t<T, T> >& l) const {
      if (nd_it == node_end())
        return;
      if (q_interval.first >= nd_it.get_metadata())
        return;
      overlaps_interval(nd_it.get_l_child(), q_interval, l);
      if ((*nd_it)->first.first < q_interval.second && q_interval.first < (*nd_it)->first.second)
        l.push_front((*nd_it)->first);
      if (q_interval.second <= (*nd_it)->first.first)
        return;
      overlaps_interval(nd_it.get_r_child(), q_interval, l);
    }
    void intersects_interval(Node_CItr nd_it, const Pair_t<T, T> q_interval, std::list<Pair_t<T, T> >& l) const {
      if (nd_it == node_end())
        return;
      if (q_interval.first > nd_it.get_metadata())
        return;
      intersects_interval(nd_it.get_l_child(), q_interval, l);
      if ((*nd_it)->first.first <= q_interval.second && q_interval.first <= (*nd_it)->first.second)
        l.push_front((*nd_it)->first);
      if (q_interval.second < (*nd_it)->first.first)
        return;
      intersects_interval(nd_it.get_r_child(), q_interval, l);
    }
    void contain_point_interval(Node_CItr nd_it, const T q_pt, std::list<Pair_t<T, T> > &l) const {
      if (nd_it == node_end())
        return;
      if (q_pt > nd_it.get_metadata())
        return;
      contain_point_interval(nd_it.get_l_child(), q_pt, l);
      if ((*nd_it)->first.first <= q_pt && q_pt <= (*nd_it)->first.second)
        l.push_front((*nd_it)->first);
      if (q_pt < (*nd_it)->first.first)
        return;
      contain_point_interval(nd_it.get_r_child(), q_pt, l);
    }
    void overlaps_all(Node_CItr nd_it, const Pair_t<T, T> q_interval, std::list<Pair_t<Pair_t<T, T>, U> >& l) const {
      if (nd_it == node_end())
        return;
      if (q_interval.first >= nd_it.get_metadata())
        return;
      overlaps_all(nd_it.get_l_child(), q_interval, l);
      if ((*nd_it)->first.first < q_interval.second && q_interval.first < (*nd_it)->first.second)
        l.push_front(**nd_it);
      if (q_interval.second <= (*nd_it)->first.first)
        return;
      overlaps_all(nd_it.get_r_child(), q_interval, l);
    }
    void intersects_all(Node_CItr nd_it, const Pair_t<T, T> q_interval, std::list<Pair_t<Pair_t<T, T>, U> >& l) const {
      if (nd_it == node_end())
        return;
      if (q_interval.first > nd_it.get_metadata())
        return;
      intersects_all(nd_it.get_l_child(), q_interval, l);
      if ((*nd_it)->first.first <= q_interval.second && q_interval.first <= (*nd_it)->first.second)
        l.push_front(**nd_it);
      if (q_interval.second < (*nd_it)->first.first)
        return;
      intersects_all(nd_it.get_r_child(), q_interval, l);
    }
    void contain_point_all(Node_CItr nd_it, const T q_pt, std::list<Pair_t<Pair_t<T, T>, U> > &l) const {
      if (nd_it == node_end())
        return;
      if (q_pt > nd_it.get_metadata())
        return;
      contain_point_all(nd_it.get_l_child(), q_pt, l);
      if ((*nd_it)->first.first <= q_pt && q_pt <= (*nd_it)->first.second)
        l.push_front(**nd_it);
      if (q_pt < (*nd_it)->first.first)
        return;
      contain_point_all(nd_it.get_r_child(), q_pt, l);
    }
  }; // Struct intervals_node_update

}; // Struct my_node_update

template<typename T, typename U>
using IntervalTree = typename __gnu_pbds::tree<Pair_t<T, T>, U, std::less<Pair_t<T, T> >, __gnu_pbds::rb_tree_tag, my_node_update<T, U>::template intervals_node_update>;

template<typename T, typename U>
struct my_multi_node_update {

  template<typename Node_CItr, typename Node_Itr, typename Cmp_Fn, typename _Alloc>
  class intervals_node_update {
  public:

    typedef T metadata_type;
    void query_interval(const Pair_t<T, T> q_interval, std::list<Pair_t<T, T> > &l, bool open=true) const {
      if (q_interval.first == q_interval.second)
        contain_point_interval(node_begin(), q_interval.first, l);
      else if (open)
        overlaps_interval(node_begin(), q_interval, l); 
      else
        intersects_interval(node_begin(), q_interval, l);
    }
    void query(const Pair_t<T, T> q_interval, std::list<U> &l, bool open=true) const {
      if (q_interval.first == q_interval.second)
        contain_point(node_begin(), q_interval.first, l);
      else if (open)
        overlaps(node_begin(), q_interval, l); 
      else
        intersects(node_begin(), q_interval, l);
    }
    void query(const T q_pt, std::list<U> &l) const { contain_point(node_begin(), q_pt, l); }

  protected:
    inline void operator() (Node_Itr nd_it, Node_CItr end_nd_it) {
      const T l_max_endpoint = nd_it.get_l_child() == end_nd_it ? 0 : nd_it.get_l_child().get_metadata();
      const T r_max_endpoint = nd_it.get_r_child() == end_nd_it ? 0 : nd_it.get_r_child().get_metadata();
      const_cast<T&>(nd_it.get_metadata()) = max({(*nd_it)->first.second, l_max_endpoint, r_max_endpoint});
    }

    virtual Node_CItr node_begin() const = 0;
    virtual Node_CItr node_end() const = 0;
    virtual ~intervals_node_update() {}
    
  private:
    void overlaps(Node_CItr nd_it, const Pair_t<T, T> q_interval, std::list<U>& l) const {
      if (nd_it == node_end())
        return;
      if (q_interval.first >= nd_it.get_metadata())
        return;
      overlaps(nd_it.get_l_child(), q_interval, l);
      if ((*nd_it)->first.first < q_interval.second && q_interval.first < (*nd_it)->first.second)
        l.insert(l.begin(), (*nd_it)->second.begin(), (*nd_it)->second.end());
      if (q_interval.second <= (*nd_it)->first.first)
        return;
      overlaps(nd_it.get_r_child(), q_interval, l);
    }
    void intersects(Node_CItr nd_it, const Pair_t<T, T> q_interval, std::list<U>& l) const {
      if (nd_it == node_end())
        return;
      if (q_interval.first > nd_it.get_metadata())
        return;
      intersects(nd_it.get_l_child(), q_interval, l);
      if ((*nd_it)->first.first <= q_interval.second && q_interval.first <= (*nd_it)->first.second)
        l.insert(l.begin(), (*nd_it)->second.begin(), (*nd_it)->second.end());
      if (q_interval.second < (*nd_it)->first.first)
        return;
      intersects(nd_it.get_r_child(), q_interval, l);
    }
    void contain_point(Node_CItr nd_it, const T q_pt, std::list<U> &l) const {
      if (nd_it == node_end())
        return;
      if (q_pt > nd_it.get_metadata())
        return;
      contain_point(nd_it.get_l_child(), q_pt, l);
      if ((*nd_it)->first.first <= q_pt && q_pt <= (*nd_it)->first.second)
        l.insert(l.begin(), (*nd_it)->second.begin(), (*nd_it)->second.end());
      if (q_pt < (*nd_it)->first.first)
        return;
      contain_point(nd_it.get_r_child(), q_pt, l);
    }
    void overlaps_interval(Node_CItr nd_it, const Pair_t<T, T> q_interval, std::list<Pair_t<T, T> >& l) const {
      if (nd_it == node_end())
        return;
      if (q_interval.first >= nd_it.get_metadata())
        return;
      overlaps_interval(nd_it.get_l_child(), q_interval, l);
      if ((*nd_it)->first.first < q_interval.second && q_interval.first < (*nd_it)->first.second)
        l.push_front((*nd_it)->first);
      if (q_interval.second <= (*nd_it)->first.first)
        return;
      overlaps_interval(nd_it.get_r_child(), q_interval, l);
    }
    void intersects_interval(Node_CItr nd_it, const Pair_t<T, T> q_interval, std::list<Pair_t<T, T> >& l) const {
      if (nd_it == node_end())
        return;
      if (q_interval.first > nd_it.get_metadata())
        return;
      intersects_interval(nd_it.get_l_child(), q_interval, l);
      if ((*nd_it)->first.first <= q_interval.second && q_interval.first <= (*nd_it)->first.second)
        l.push_front((*nd_it)->first);
      if (q_interval.second < (*nd_it)->first.first)
        return;
      intersects_interval(nd_it.get_r_child(), q_interval, l);
    }
    void contain_point_interval(Node_CItr nd_it, const T q_pt, std::list<Pair_t<T, T> > &l) const {
      if (nd_it == node_end())
        return;
      if (q_pt > nd_it.get_metadata())
        return;
      contain_point_interval(nd_it.get_l_child(), q_pt, l);
      if ((*nd_it)->first.first <= q_pt && q_pt <= (*nd_it)->first.second)
        l.push_front((*nd_it)->first);
      if (q_pt < (*nd_it)->first.first)
        return;
      contain_point_interval(nd_it.get_r_child(), q_pt, l);
    }
  }; // Struct intervals_node_update  

}; // Struct my_multi_node_update 

template<typename T, typename U>
using MultiIntervalTree = typename __gnu_pbds::tree<Pair_t<T, T>, __gnu_pbds::list_update<U, __gnu_pbds::null_type>, std::less<Pair_t<T, T> >, __gnu_pbds::rb_tree_tag, my_multi_node_update<T, U>::template intervals_node_update>;




PROJECT_NAMESPACE_END


#endif /// _GEO_INTERVALTREE_HPP_
