/*
 [auto_generated]
 boost/numeric/odeint/util/is_resizeable.hpp

 [begin_description]
 Metafunction to determine if a state type can resized. For usage in the
 steppers. [end_description]

 Copyright 2009-2011 Karsten Ahnert
 Copyright 2009-2011 Mario Mulansky

 Distributed under the Boost Software License, Version 1.0.
 (See accompanying file LICENSE_1_0.txt or
 copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_NUMERIC_ODEINT_UTIL_IS_RESIZEABLE_HPP_INCLUDED
#define BOOST_NUMERIC_ODEINT_UTIL_IS_RESIZEABLE_HPP_INCLUDED

#include <vector>

#include <boost/fusion/include/front.hpp>
#include <boost/fusion/include/is_sequence.hpp>
#include <boost/type_traits/integral_constant.hpp>
#include <boost/type_traits/remove_reference.hpp>

#include <boost/mpl/end.hpp>
#include <boost/mpl/find_if.hpp>
#include <boost/mpl/placeholders.hpp>
// #include <boost/mpl/not.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_same.hpp>

namespace boost {
namespace numeric {
namespace odeint {

/*
 * by default any type is not resizable
 */
template <class Container, class Enabler = void>
struct is_resizeable {
  // struct type : public boost::false_type { };
  typedef boost::false_type type;
  const static bool value = type::value;
};

/*
 * specialization for std::vector
 */
template <class V, class A>
struct is_resizeable<std::vector<V, A> > {
  // struct type : public boost::true_type { };
  typedef boost::true_type type;
  const static bool value = type::value;
};

/*
 * specialization for fusion sequences
 */
template <class FusionSequence>
struct is_resizeable<
    FusionSequence,
    typename boost::enable_if<typename boost::fusion::traits::is_sequence<
        FusionSequence>::type>::type> {
  typedef
      typename boost::mpl::find_if<FusionSequence,
                                   is_resizeable<boost::mpl::_1> >::type iter;
  typedef typename boost::mpl::end<FusionSequence>::type last;

  typedef
      typename boost::mpl::if_<boost::is_same<iter, last>, boost::false_type,
                               boost::true_type>::type type;
  const static bool value = type::value;
};

}  // namespace odeint
}  // namespace numeric
}  // namespace boost

#endif  // BOOST_NUMERIC_ODEINT_UTIL_IS_RESIZEABLE_HPP_INCLUDED
