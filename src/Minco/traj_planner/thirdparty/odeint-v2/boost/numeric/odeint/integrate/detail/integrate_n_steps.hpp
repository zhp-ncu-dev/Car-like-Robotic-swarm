/*
 [auto_generated]
 boost/numeric/odeint/integrate/detail/integrate_n_steps.hpp

 [begin_description]
 integrate steps implementation
 [end_description]

 Copyright 2009-2012 Karsten Ahnert
 Copyright 2009-2012 Mario Mulansky

 Distributed under the Boost Software License, Version 1.0.
 (See accompanying file LICENSE_1_0.txt or
 copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_NUMERIC_ODEINT_INTEGRATE_DETAIL_INTEGRATE_N_STEPS_HPP_INCLUDED
#define BOOST_NUMERIC_ODEINT_INTEGRATE_DETAIL_INTEGRATE_N_STEPS_HPP_INCLUDED

#include <boost/numeric/odeint/integrate/detail/integrate_adaptive.hpp>
#include <boost/numeric/odeint/stepper/stepper_categories.hpp>
#include <boost/numeric/odeint/util/unit_helper.hpp>
#include <boost/numeric/odeint/util/unwrap_reference.hpp>

#include <boost/numeric/odeint/util/detail/less_with_sign.hpp>

namespace boost {
namespace numeric {
namespace odeint {
namespace detail {

// forward declaration
template <class Stepper, class System, class State, class Time, class Observer>
size_t integrate_adaptive(Stepper stepper, System system, State &start_state,
                          Time &start_time, Time end_time, Time &dt,
                          Observer observer, controlled_stepper_tag);

/* basic version */
template <class Stepper, class System, class State, class Time, class Observer>
Time integrate_n_steps(Stepper stepper, System system, State &start_state,
                       Time start_time, Time dt, size_t num_of_steps,
                       Observer observer, stepper_tag) {
  typename odeint::unwrap_reference<Observer>::type &obs = observer;

  Time time = start_time;

  for (size_t step = 0; step < num_of_steps; ++step) {
    obs(start_state, time);
    stepper.do_step(system, start_state, time, dt);
    // direct computation of the time avoids error propagation happening when
    // using time += dt we need clumsy type analysis to get boost units working
    // here
    time = start_time +
           static_cast<typename unit_value_type<Time>::type>(step + 1) * dt;
  }
  obs(start_state, time);

  return time;
}

/* controlled version */
template <class Stepper, class System, class State, class Time, class Observer>
Time integrate_n_steps(Stepper stepper, System system, State &start_state,
                       Time start_time, Time dt, size_t num_of_steps,
                       Observer observer, controlled_stepper_tag) {
  typename odeint::unwrap_reference<Observer>::type &obs = observer;

  Time time = start_time;
  Time time_step = dt;

  for (size_t step = 0; step < num_of_steps; ++step) {
    obs(start_state, time);
    detail::integrate_adaptive(stepper, system, start_state, time,
                               time + time_step, dt, null_observer(),
                               controlled_stepper_tag());
    // direct computation of the time avoids error propagation happening when
    // using time += dt we need clumsy type analysis to get boost units working
    // here
    time =
        start_time +
        static_cast<typename unit_value_type<Time>::type>(step + 1) * time_step;
  }
  obs(start_state, time);

  return time;
}

/* dense output version */
template <class Stepper, class System, class State, class Time, class Observer>
Time integrate_n_steps(Stepper stepper, System system, State &start_state,
                       Time start_time, Time dt, size_t num_of_steps,
                       Observer observer, dense_output_stepper_tag) {
  typename odeint::unwrap_reference<Observer>::type &obs = observer;

  Time time = start_time;
  const Time end_time =
      start_time +
      static_cast<typename unit_value_type<Time>::type>(num_of_steps) * dt;

  stepper.initialize(start_state, time, dt);

  size_t step = 0;

  while (step < num_of_steps) {
    while (less_with_sign(time, stepper.current_time(),
                          stepper.current_time_step())) {
      stepper.calc_state(time, start_state);
      obs(start_state, time);
      ++step;
      // direct computation of the time avoids error propagation happening when
      // using time += dt we need clumsy type analysis to get boost units
      // working here
      time = start_time +
             static_cast<typename unit_value_type<Time>::type>(step) * dt;
    }

    // we have not reached the end, do another real step
    if (less_with_sign(stepper.current_time() + stepper.current_time_step(),
                       end_time, stepper.current_time_step())) {
      stepper.do_step(system);
    } else if (less_with_sign(
                   stepper.current_time(), end_time,
                   stepper.current_time_step())) {  // do the last step ending
                                                    // exactly on the end point
      stepper.initialize(stepper.current_state(), stepper.current_time(),
                         end_time - stepper.current_time());
      stepper.do_step(system);
    }
  }

  while (stepper.current_time() < end_time) {
    if (less_with_sign(end_time,
                       stepper.current_time() + stepper.current_time_step(),
                       stepper.current_time_step()))
      stepper.initialize(stepper.current_state(), stepper.current_time(),
                         end_time - stepper.current_time());
    stepper.do_step(system);
  }

  // observation at end point, only if we ended exactly on the end-point (or
  // above due to finite precision)
  obs(stepper.current_state(), end_time);

  return time;
}

}  // namespace detail
}  // namespace odeint
}  // namespace numeric
}  // namespace boost

#endif /* BOOST_NUMERIC_ODEINT_INTEGRATE_DETAIL_INTEGRATE_N_STEPS_HPP_INCLUDED \
        */
