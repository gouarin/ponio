#pragma once

#include <concepts>
#include <tuple>
#include <array>
#include <type_traits>
#include <cmath>

#include "detail.hpp"
#include "stage.hpp"
#include "generic_butcher_rk.hpp"

namespace ode {

  template <typename state_t>
  auto
  error_estimate( state_t const& un, state_t const& unp1, state_t const& unp1bis )
  {
    return std::abs( (unp1-unp1bis)/(1.0 + std::max(un,unp1)) );
  }
  template <typename state_t>
  requires ::detail::is_const_iterable<state_t>
  auto
  error_estimate( state_t const& un, state_t const& unp1, state_t const& unp1bis )
  {
    auto it_unp1    = std::cbegin(unp1);
    auto it_unp1bis = std::cbegin(unp1bis);
    auto last = std::cend(un);

    using value_t = std::remove_cvref_t<decltype(*it_unp1)>;
    auto r = static_cast<value_t>(0.);

    for ( auto it_un=std::cbegin(un) ; it_un != last ; ++it_un, ++it_unp1, ++it_unp1bis )
    {
      r += ::detail::power<2>(
        ( *it_unp1 - *it_unp1bis )/( 1.0 + std::max(*it_un,*it_unp1) )
      );
    }
    return std::sqrt(r);

    /*
    return std::sqrt(
      std::accumulate(
        std::cbegin(un), std::cend(un),
        [&it_unp1,&it_unp1bis]( auto r , auto uni ) mutable {
          return std::pow(
              (*it_unp1 - *it_unp1bis++)/(1.0 + std::max(uni,*it_unp1++))
            , 2u );
        }
      )
    );
    */
  }

  template <typename Algorithm_t, typename state_t>
  struct method
  {
    static constexpr bool is_embedded = Algorithm_t::is_embedded;
    using step_storage_t = std::conditional< is_embedded,
                              std::array<state_t,Algorithm_t::N_stages+2>,
                              std::array<state_t,Algorithm_t::N_stages+1>
                            >::type;

    Algorithm_t alg;
    step_storage_t kis;

    method( Algorithm_t const& alg_ , state_t const& shadow_of_u0 )
    : alg(alg_) , kis(::detail::init_fill_array<Algorithm_t::N_stages+1>(shadow_of_u0))
    {}

    template < std::size_t I=0 , typename Problem_t , typename value_t >
    typename std::enable_if< (I == Algorithm_t::N_stages+1) , void >::type
    _call_stage ( Problem_t & f , value_t tn , state_t const& un , value_t dt )
    {
      
    }

    template < std::size_t I=0 , typename Problem_t , typename value_t >
    typename std::enable_if< (I < Algorithm_t::N_stages+1) , void >::type
    _call_stage ( Problem_t & f , value_t tn , state_t const& un , value_t dt )
    {
      kis[I] = alg.stage(Stage<I>{}, f, tn, un, kis, dt);
      _call_stage<I+1>(f,tn,un,dt);
    }

    template < typename value_t , typename Algo_t=Algorithm_t >
    std::tuple<value_t,state_t,value_t>
    _return ( value_t tn, state_t const& un, value_t dt )
    {
      return std::make_tuple(
        tn+dt,
        kis.back(),
        dt
      );
    }

    template < typename value_t , typename Algo_t=Algorithm_t >
    requires std::same_as<Algo_t,Algorithm_t> && Algorithm_t::is_embedded
    std::tuple<value_t,state_t,value_t>
    _return ( value_t tn, state_t const& un, value_t dt )
    {
      auto error = error_estimate(un,kis[Algorithm_t::N_stages],kis[Algorithm_t::N_stages+1]);

      value_t new_dt = 0.9*std::pow(alg.tol/error, 1./static_cast<value_t>(Algorithm_t::order))*dt;

      if (error > alg.tol) {
        return std::make_tuple(
          tn+dt,
          kis[Algorithm_t::N_stages],
          new_dt
        );
      } else {
        return std::make_tuple(
          tn,
          un,
          new_dt
        );
      }
    }

    template < typename Problem_t , typename value_t >
    inline std::tuple<value_t,state_t,value_t>
    operator () ( Problem_t & f , value_t tn , state_t const& un , value_t dt )
    {
      _call_stage(f,tn,un,dt);

      return _return(tn,un,dt);
    }
  };

  /**
   *  generic factory to build a method from an algoritm, it only reuses `method`
   *  constructor
   *  @param algo         a `Algorithm_t` objet with predifined stages of the method
   *  @param shadow_of_u0 an object with the same size of computed value for allocation
   */
  template < typename Algorithm_t , typename state_t >
  auto
  make_method ( Algorithm_t const& algo , state_t const& shadow_of_u0 )
  {
    return method<Algorithm_t,state_t>(algo,shadow_of_u0);
  }

} // namespace ode