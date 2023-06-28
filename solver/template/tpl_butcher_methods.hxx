// Copyright 2022 PONIO TEAM. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#pragma once
// autogenerated file

#include <cmath>
#include <string_view>
#include <tuple>

#include "butcher_tableau.hpp"
#include "generic_butcher_rk.hpp"
#include "ponio_config.hpp"

namespace ode::butcher
{
    // clang-format off

// explicit Runge-Kutta methods
{% for rk in list_erk %}
/**
 * @brief Butcher tableau of {{ rk.label }} method
 * @tparam value_t type of coefficient (``double``by default)
 */
template <typename value_t=double>
struct butcher_{{ rk.id }} : public {{ "adaptive_" if 'b2' in rk else "" }}butcher_tableau<{{ rk.A|length }}, value_t>
{
  using base_t = {{ "adaptive_" if 'b2' in rk else "" }}butcher_tableau<{{ rk.A|length }}, value_t>;
  static constexpr std::size_t N_stages = base_t::N_stages;
  static constexpr std::size_t order = {{ rk.order }};
  static constexpr std::string_view id = "{{ rk.id }}";

  using base_t::A;
  using base_t::b;
  using base_t::c;

  butcher_{{ rk.id }}()
  : base_t(
    {{ '{{' }}
    {% for ai in rk.A -%}
      { {{ ai }} }{{ ",\n      " if not loop.last else "" }}
    {%- endfor %}
    {{ '}}' }}, // A
    { {{ rk.b }} }, // b
    {% if 'b2' in rk -%}{ {{ rk.b2 }} }, // b2 {%- endif %}
    { {{ rk.c }} }  // c
  )
  {}
};

/**
 * @brief {{ rk.label }} method
 * @tparam value_t type of coefficient (``double``by default)
 *
 * @details see more on [ponio](https://josselin.massot.gitlab.labos.polytechnique.fr/ponio/viewer.html#{{ rk.id }})
 */
template <typename value_t>
using {{ rk.id }}_t = runge_kutta::explicit_rk_butcher<butcher_{{ rk.id }}<value_t>>;

using {{ rk.id }} = runge_kutta::explicit_rk_butcher<butcher_{{ rk.id }}<double>>;

/**
 * @brief l{{ rk.label }} method
 * @tparam value_t type of coefficient (``double``by default)
 *
 * @details see more on [ponio](https://josselin.massot.gitlab.labos.polytechnique.fr/ponio/viewer.html#{{ rk.id }})
 */
template <typename value_t, typename Exp_t>
constexpr auto l{{ rk.id }}_t = []( Exp_t exp_ , double tol=ponio::default_config::tol )
{
  return lawson::make_lawson<butcher_{{ rk.id }}<value_t>,Exp_t>(exp_,tol);
};

template <typename Exp_t>
auto
l{{ rk.id }}( Exp_t exp_ , double tol=ponio::default_config::tol )
{
  return lawson::make_lawson<butcher_{{ rk.id }}<double>,Exp_t>(exp_,tol);
}

{% endfor %}

/**
 * @brief Type of tuple that contains all classical explicit Runge-Kutta methods of ponio
 */
template <typename value_t>
using erk_tuple = std::tuple< {{ list_erk | sformat("{}_t<value_t>", attribute="id") | join(", ") }} >;

/**
 * @brief Type of tuple that contains all Lawson methods of ponio
*/
template <typename value_t, typename Exp_t>
using lrk_tuple = std::tuple< {{ list_erk | sformat("decltype(l{}_t<value_t, Exp_t>)", attribute="id") | join(", ") }} >;

// exponential Runge-Kutta methods
{% for rk in list_exprk %}
/**
 * @brief Butcher tableau of {{ rk.label }} method
 * @tparam _value_t type of coefficient (``double`` by default)
 * @tparam _linear_t type of linear part (``double`` by default)
 */
template <typename _value_t=double, typename _linear_t=double>
struct butcher_{{ rk.id }}
{
  using value_t  = _value_t;
  using linear_t = _linear_t;
  using func_t   = std::function<linear_t(linear_t &&)>;
  static constexpr std::size_t N_stages = {{ rk.b.type|length }};
  static constexpr std::size_t order = 1;
  static constexpr const char* id = "{{ rk.id }}";

  std::tuple<{% for t in rk.A.type -%}{{ t }}{{ " , " if not loop.last else "" }}{%- endfor %}> a;
  std::tuple<{% for t in rk.b.type -%}{{ t }}{{ " , " if not loop.last else "" }}{%- endfor %}> b;
  std::array<value_t, N_stages> c;

  butcher_{{ rk.id }}()
  : a( {% for aij in rk.A.code %}{{ aij }}{{ " , " if not loop.last else "" }}{%- endfor %} )
  , b( {% for bi  in rk.b.code %}{{ bi  }}{{ " , " if not loop.last else "" }}{%- endfor %} )
  , c({ {{ rk.c }} })
  {}
};

/**
 * @brief {{ rk.label }} method
 * @tparam value_t type of coefficient (``double``by default)
 * @tparam linear_t type of coefficient (``double``by default)
 */
template <typename value_t, typename linear_t>
using {{ rk.id }}_t = exp_runge_kutta::explicit_exp_rk_butcher<butcher_{{ rk.id }}<value_t, linear_t>>;

using {{ rk.id }} = exp_runge_kutta::explicit_exp_rk_butcher<butcher_{{ rk.id }}<double, double>>;

{% endfor %}

/**
 * @brief Type of tuple that contains all exponential Runge-Kutta methods of ponio
*/
template <typename value_t, typename linear_t>
using exprk_tuple = std::tuple< {{ list_exprk | sformat("{}_t<value_t, linear_t>", attribute="id") | join(", ") }} >;

// diagonal implicit Runge-Kutta methods
{% for rk in list_dirk %}
/**
 * @brief Butcher tableau of {{ rk.label }} method
 * @tparam value_t type of coefficient (``double``by default)
 */
template <typename value_t=double>
struct butcher_{{ rk.id }} : public {{ "adaptive_" if 'b2' in rk else "" }}butcher_tableau<{{ rk.A|length }},value_t>
{
  using base_t = {{ "adaptive_" if 'b2' in rk else "" }}butcher_tableau<{{ rk.A|length }},value_t>;
  static constexpr std::size_t N_stages = base_t::N_stages;
  static constexpr std::size_t order = {{ rk.order }};
  static constexpr const char* id = "{{ rk.id }}";

  using base_t::A;
  using base_t::b;
  using base_t::c;

  butcher_{{ rk.id }}()
  : base_t(
    {{ '{{' }}
    {% for ai in rk.A -%}
      { {{ ai }} }{{ ",\n      " if not loop.last else "" }}
    {%- endfor %}
    {{ '}}' }}, // A
    { {{ rk.b }} }, // b
    {% if 'b2' in rk -%}{ {{ rk.b2 }} }, // b2 {%- endif %}
    { {{ rk.c }} }  // c
  )
  {}
};

template <typename value_t, typename linear_algebra_t=void, typename ... Args>
auto
{{ rk.id }}_t ( Args ... args )
{
  return runge_kutta::make_dirk<butcher_{{ rk.id }}<value_t>, linear_algebra_t>(args...);
}

template <typename linear_algebra_t=void, typename ... Args>
auto
{{ rk.id }} ( Args ... args )
{
  return runge_kutta::make_dirk<butcher_{{ rk.id }}<double>, linear_algebra_t>(args...);
}

{% endfor %}

    // clang-format on

} // namespace ode::butcher
