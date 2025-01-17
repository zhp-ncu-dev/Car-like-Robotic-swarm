/* Boost check_gmp.cpp test file

 Copyright 2009 Karsten Ahnert
 Copyright 2009 Mario Mulansky

 This file tests the odeint library with the gmp arbitrary precision types

 Distributed under the Boost Software License, Version 1.0.
 (See accompanying file LICENSE_1_0.txt or
 copy at http://www.boost.org/LICENSE_1_0.txt)
*/

#define BOOST_TEST_MODULE odeint_gmp

#include <gmpxx.h>

#include <boost/array.hpp>
#include <boost/test/unit_test.hpp>

#include <boost/mpl/vector.hpp>

#include <boost/numeric/odeint.hpp>
//#include <boost/numeric/odeint/algebra/vector_space_algebra.hpp>

using namespace boost::unit_test;
using namespace boost::numeric::odeint;

namespace mpl = boost::mpl;

const int precision = 1024;

typedef mpf_class value_type;
typedef mpf_class state_type;

// provide min, max and pow functions for mpf types - required for controlled
// steppers
value_type min(const value_type a, const value_type b) {
  if (a < b)
    return a;
  else
    return b;
}
value_type max(const value_type a, const value_type b) {
  if (a > b)
    return a;
  else
    return b;
}
value_type pow(const value_type a, const value_type b) {
  // do the calculation in double precision...
  return value_type(std::pow(a.get_d(), b.get_d()));
}

// provide vector_space reduce:

namespace boost {
namespace numeric {
namespace odeint {

template <>
struct vector_space_reduce<state_type> {
  template <class Op>
  state_type operator()(state_type x, Op op, state_type init) const {
    init = op(init, x);
    return init;
  }
};

}  // namespace odeint
}  // namespace numeric
}  // namespace boost

void constant_system(const state_type &x, state_type &dxdt, value_type t) {
  dxdt = value_type(1.0, precision);
}

/* check runge kutta stepers */
typedef mpl::vector<
    euler<state_type, value_type, state_type, value_type, vector_space_algebra>,
    modified_midpoint<state_type, value_type, state_type, value_type,
                      vector_space_algebra>,
    runge_kutta4<state_type, value_type, state_type, value_type,
                 vector_space_algebra>,
    runge_kutta4_classic<state_type, value_type, state_type, value_type,
                         vector_space_algebra>,
    runge_kutta_cash_karp54_classic<state_type, value_type, state_type,
                                    value_type, vector_space_algebra>,
    runge_kutta_cash_karp54<state_type, value_type, state_type, value_type,
                            vector_space_algebra>,
    runge_kutta_dopri5<state_type, value_type, state_type, value_type,
                       vector_space_algebra>,
    runge_kutta_fehlberg78<state_type, value_type, state_type, value_type,
                           vector_space_algebra> >
    stepper_types;

template <class Stepper>
struct perform_integrate_const_test {
  void operator()(void) {
    /* We have to specify the desired precision in advance! */
    mpf_set_default_prec(precision);

    mpf_t eps_, unity;
    mpf_init(eps_);
    mpf_init(unity);
    mpf_set_d(unity, 1.0);
    mpf_div_2exp(eps_, unity,
                 precision - 1);  // 2^(-precision+1) : smallest number that can
                                  // be represented with used precision
    value_type eps(eps_);

    Stepper stepper;
    state_type x;
    x = 0.0;
    value_type t0(0.0);
    value_type tend(1.0);
    value_type dt(0.1);

    integrate_const(stepper, constant_system, x, t0, tend, dt);

    x = 0.0;
    t0 = 0.0;
    dt = 0.1;
    size_t steps = 10;

    integrate_n_steps(stepper, constant_system, x, t0, dt, steps);

    BOOST_CHECK_MESSAGE(abs(x - 10 * dt) < eps, x);
  }
};

BOOST_AUTO_TEST_CASE_TEMPLATE(integrate_const_test, Stepper, stepper_types) {
  perform_integrate_const_test<Stepper> tester;
  tester();
}

typedef mpl::vector<
    controlled_runge_kutta<runge_kutta_cash_karp54_classic<
        state_type, value_type, state_type, value_type, vector_space_algebra> >,
    controlled_runge_kutta<runge_kutta_dopri5<
        state_type, value_type, state_type, value_type, vector_space_algebra> >,
    controlled_runge_kutta<runge_kutta_fehlberg78<
        state_type, value_type, state_type, value_type, vector_space_algebra> >,
    bulirsch_stoer<state_type, value_type, state_type, value_type,
                   vector_space_algebra> >
    controlled_stepper_types;

template <class Stepper>
struct perform_integrate_adaptive_test {
  void operator()(void) {
    mpf_set_default_prec(precision);

    mpf_t eps_, unity;
    mpf_init(eps_);
    mpf_init(unity);
    mpf_set_d(unity, 1.0);
    mpf_div_2exp(eps_, unity,
                 precision - 1);  // 2^(-precision+1) : smallest number that can
                                  // be represented with used precision
    value_type eps(eps_);

    Stepper stepper;
    state_type x;
    x = 0.0;
    value_type t0(0.0);
    value_type tend(1.0);
    value_type dt(0.1);

    integrate_adaptive(stepper, constant_system, x, t0, tend, dt);

    BOOST_CHECK_MESSAGE(abs(x - tend) < eps, x - 0.1);
  }
};

BOOST_AUTO_TEST_CASE_TEMPLATE(integrate_adaptive__test, Stepper,
                              controlled_stepper_types) {
  perform_integrate_adaptive_test<Stepper> tester;
  tester();
}
