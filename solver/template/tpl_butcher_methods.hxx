#pragma once
// autogenerated file

#include "butcher_tableau.hpp"
#include "generic_butcher_rk.hpp"

namespace ode::butcher {

{% for rk in list_meth %}
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
/**
 * @brief {{ rk.label }} method
 * @tparam value_t type of coefficient (``double``by default)
 * 
 * @details see more on [ponio](https://josselin.massot.gitlab.labos.polytechnique.fr/ponio/viewer.html#{{ rk.id }})
 */
template <typename value_t=double>
using {{ rk.id }} = runge_kutta::explicit_rk_butcher<butcher_{{ rk.id }}<value_t>>;

{% endfor %}

} // namespace ode::butcher