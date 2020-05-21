#ifndef STAN_MATH_PRIM_META_REF_TYPE_HPP
#define STAN_MATH_PRIM_META_REF_TYPE_HPP

namespace stan {

/**
 * Determines appropriate type for assigning expression of given type to,
 * to evaluate expensive expressions, but not make a copy if T involves no
 * calculations. This works similarly as `Eigen::Ref`. It also handles
 * rvalue references, so it can be used with perfect forwarding.
 * @tparam T type to determine reference for
 * @tparam Ref_stride Stride type (see the documentation for `Eigen::Ref`)
 */
template <typename T,
          typename Ref_stride
          = std::conditional_t<is_eigen_vector<T>::value, Eigen::InnerStride<1>,
                               Eigen::OuterStride<>>,
          typename = void>
struct ref_type {
  using T_plain = plain_type_t<T>;
  using T_optionally_ref
      = std::conditional_t<std::is_rvalue_reference<T>::value,
                           std::remove_reference_t<T>, const T&>;
  using type = std::conditional_t<
      Eigen::internal::traits<
          Eigen::Ref<std::decay_t<T_plain>, 0, Ref_stride>>::
          template match<std::decay_t<T>>::MatchAtCompileTime,
      T_optionally_ref, T_plain>;
};

template <typename T, typename Ref_stride>
struct ref_type<T, Ref_stride, require_not_eigen_t<T>> {
  using type
      = std::conditional_t<std::is_rvalue_reference<T>::value, T, const T&>;
};

template <typename T>
using ref_type_t = typename ref_type<T>::type;

}  // namespace stan

#endif
