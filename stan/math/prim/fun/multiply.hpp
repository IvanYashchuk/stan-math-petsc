#ifndef STAN_MATH_PRIM_FUN_MULTIPLY_HPP
#define STAN_MATH_PRIM_FUN_MULTIPLY_HPP

#include <stan/math/prim/meta.hpp>
#include <stan/math/prim/err.hpp>
#include <stan/math/prim/fun/Eigen.hpp>
#include <stan/math/prim/fun/dot_product.hpp>
#ifdef STAN_OPENCL
#include <stan/math/opencl/opencl.hpp>
#endif
#include <type_traits>

namespace stan {
namespace math {

/**
 * Return specified matrix multiplied by specified scalar.
 *
 * @tparam Mat type of the matrix or expression
 * @tparam Scal type of the scalar
 *
 * @param m matrix
 * @param c scalar
 * @return product of matrix and scalar
 */
template <typename Mat, typename Scal, require_eigen_t<Mat>* = nullptr,
          require_stan_scalar_t<Scal>* = nullptr>
inline auto multiply(const Mat& m, Scal c) {
  return (c * m).eval();
}

/**
 * Return specified scalar multiplied by specified matrix.
 *
 * @tparam Scal type of the scalar
 * @tparam Mat type of the matrix or expression
 *
 * @param c scalar
 * @param m matrix
 * @return product of scalar and matrix
 */
template <typename Scal, typename Mat, require_stan_scalar_t<Scal>* = nullptr,
          require_eigen_t<Mat>* = nullptr>
inline auto multiply(Scal c, const Mat& m) {
  return (c * m).eval();
}

/**
 * Return the product of the specified matrices. The number of
 * columns in the first matrix must be the same as the number of rows
 * in the second matrix.
 *
 * @tparam Mat1 type of the first matrix or expression
 * @tparam Mat2 type of the second matrix or expression
 *
 * @param m1 first matrix or expression
 * @param m2 second matrix or expression
 * @return the product of the first and second matrices
 * @throw <code>std::invalid_argument</code> if the number of columns of m1 does
 * not match the number of rows of m2.
 */
template <typename Mat1, typename Mat2,
          require_all_eigen_vt<std::is_arithmetic, Mat1, Mat2>* = nullptr,
          require_any_not_same_t<double, value_type_t<Mat1>,
                                 value_type_t<Mat2>>* = nullptr,
          require_not_eigen_row_and_col_t<Mat1, Mat2>* = nullptr>
inline auto multiply(const Mat1& m1, const Mat2& m2) {
  check_size_match("multiply", "Columns of m1", m1.cols(), "Rows of m2",
                   m2.rows());
  return (m1 * m2).eval();
}

/**
 * Return the product of the specified matrices. The number of
 * columns in the first matrix must be the same as the number of rows
 * in the second matrix. If scalar of matrices is \c double OpenCL
 * implementation can be used.
 *
 * @tparam Mat1 type of the first matrix or expression
 * @tparam Mat2 type of the second matrix or expression
 *
 * @param m1 first matrix or expression
 * @param m2 second matrix or expression
 * @return the product of the first and second matrices
 * @throw <code>std::invalid_argument</code> if the number of columns of m1 does
 * not match the number of rows of m2.
 */
template <typename Mat1, typename Mat2,
          require_all_eigen_t<Mat1, Mat2>* = nullptr,
          require_all_same_t<double, value_type_t<Mat1>,
                             value_type_t<Mat2>>* = nullptr,
          require_not_eigen_row_and_col_t<Mat1, Mat2>* = nullptr>
inline auto multiply(const Mat1& m1, const Mat2& m2)
    -> decltype((m1 * m2).eval()) {
  check_multiplicable("multiply", "m1", m1, "m2", m2);

#ifdef STAN_OPENCL
  if (m1.rows() * m1.cols() * m2.cols()
      > opencl_context.tuning_opts().multiply_dim_prod_worth_transfer) {
    matrix_cl<double> m1_cl(m1);
    matrix_cl<double> m2_cl(m2);
    matrix_cl<double> m3_cl = m1_cl * m2_cl;
    return from_matrix_cl<Mat1::RowsAtCompileTime, Mat2::ColsAtCompileTime>(
        m3_cl);
  } else {
    return (m1 * m2).eval();
  }
#else
  return (m1 * m2).eval();
#endif
}

/**
 * Return the scalar product of the specified row vector and
 * specified column vector.  The return is the same as the dot
 * product.  The two vectors must be the same size.
 *
 * @tparam RowVec type of the row vector
 * @tparam ColVec type of the column vector
 *
 * @param rv row vector
 * @param v column vector
 * @return scalar result of multiplying row vector by column vector
 * @throw <code>std::invalid_argument</code> if rv and v are not the same size
 */
template <typename RowVec, typename ColVec,
          require_all_not_var_t<scalar_type_t<RowVec>,
                                scalar_type_t<ColVec>>* = nullptr,
          require_eigen_row_and_col_t<RowVec, ColVec>* = nullptr>
inline auto multiply(const RowVec& rv, const ColVec& v) {
  check_multiplicable("multiply", "rv", rv, "v", v);
  return dot_product(rv, v);
}

/**
 * Return product of scalars.
 *
 * @tparam Scalar1 type of first scalar
 * @tparam Scalar2 type of second scalar
 * @param m scalar
 * @param c scalar
 * @return product
 */
template <typename Scalar1, typename Scalar2,
          require_all_stan_scalar_t<Scalar1, Scalar2>* = nullptr>
inline return_type_t<Scalar1, Scalar2> multiply(Scalar1 m, Scalar2 c) {
  return c * m;
}

}  // namespace math
}  // namespace stan

#endif
