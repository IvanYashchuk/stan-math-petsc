#include <stan/math/prim.hpp>
#include <test/unit/util.hpp>
#include <gtest/gtest.h>

#ifdef STAN_OPENCL
#include <stan/math/opencl/opencl.hpp>
#include <boost/random/mersenne_twister.hpp>
#endif

TEST(MathMatrixPrim, mdivide_left_tri_val) {
  using stan::math::mdivide_left_tri;
  stan::math::matrix_d I = Eigen::MatrixXd::Identity(2, 2);

  stan::math::matrix_d Ad(2, 2);
  Ad << 2.0, 0.0, 5.0, 7.0;
  EXPECT_MATRIX_FLOAT_EQ(I, mdivide_left_tri<Eigen::Lower>(Ad, Ad));

  stan::math::matrix_d A_Ainv = Ad * mdivide_left_tri<Eigen::Lower>(Ad);
  EXPECT_MATRIX_NEAR(I, A_Ainv, 1e-15);

  Ad << 2.0, 3.0, 0.0, 7.0;
  EXPECT_MATRIX_FLOAT_EQ(I, mdivide_left_tri<Eigen::Upper>(Ad, Ad));
}

TEST(MathMatrixPrim, mdivide_left_tri_size_zero) {
  using stan::math::mdivide_left_tri;
  stan::math::matrix_d Ad(0, 0);
  stan::math::matrix_d b0(0, 2);
  stan::math::matrix_d I;

  I = mdivide_left_tri<Eigen::Lower>(Ad, Ad);
  EXPECT_EQ(0, I.rows());
  EXPECT_EQ(0, I.cols());

  I = mdivide_left_tri<Eigen::Upper>(Ad, Ad);
  EXPECT_EQ(0, I.rows());
  EXPECT_EQ(0, I.cols());

  I = mdivide_left_tri<Eigen::Lower>(Ad);
  EXPECT_EQ(0, I.rows());
  EXPECT_EQ(0, I.cols());

  I = mdivide_left_tri<Eigen::Upper>(Ad);
  EXPECT_EQ(0, I.rows());
  EXPECT_EQ(0, I.cols());

  I = mdivide_left_tri<Eigen::Lower>(Ad, b0);
  EXPECT_EQ(0, I.rows());
  EXPECT_EQ(b0.cols(), I.cols());

  I = mdivide_left_tri<Eigen::Upper>(Ad, b0);
  EXPECT_EQ(0, I.rows());
  EXPECT_EQ(b0.cols(), I.cols());
}

#ifdef STAN_OPENCL
void mdivide_left_tri_lower_cl_test(int size) {
  boost::random::mt19937 rng;
  stan::math::matrix_d m1(size, size);
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < i; j++) {
      m1(i, j) = stan::math::uniform_rng(-5, 5, rng);
    }
    m1(i, i) = 20.0;
    for (int j = i + 1; j < size; j++) {
      m1(i, j) = 0.0;
    }
  }

  stan::math::opencl_context.tuning_opts().tri_inverse_size_worth_transfer
      = size * 2;

  stan::math::matrix_d m1_cpu = stan::math::mdivide_left_tri<Eigen::Lower>(m1);

  stan::math::opencl_context.tuning_opts().tri_inverse_size_worth_transfer = 0;

  stan::math::matrix_d m1_cl = stan::math::mdivide_left_tri<Eigen::Lower>(m1);

  EXPECT_MATRIX_NEAR(m1_cpu, m1_cl, 1E-8);
}
TEST(MathMatrixCL, mdivide_left_tri_lower_cl_small) {
  mdivide_left_tri_lower_cl_test(3);
}
TEST(MathMatrixCL, mdivide_left_tri_lower_cl_mid) {
  mdivide_left_tri_lower_cl_test(100);
}
TEST(MathMatrixCL, mdivide_left_tri_lower_cl_big) {
  mdivide_left_tri_lower_cl_test(500);
}

void mdivide_left_tri_upper_cl_test(int size) {
  boost::random::mt19937 rng;
  stan::math::matrix_d m1(size, size);
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < i; j++) {
      m1(i, j) = 0.0;
    }
    m1(i, i) = 20.0;
    for (int j = i + 1; j < size; j++) {
      m1(i, j) = stan::math::uniform_rng(-5, 5, rng);
    }
  }

  stan::math::opencl_context.tuning_opts().tri_inverse_size_worth_transfer
      = size * 2;

  stan::math::matrix_d m1_cpu = stan::math::mdivide_left_tri<Eigen::Upper>(m1);

  stan::math::opencl_context.tuning_opts().tri_inverse_size_worth_transfer = 0;

  stan::math::matrix_d m1_cl = stan::math::mdivide_left_tri<Eigen::Upper>(m1);

  EXPECT_MATRIX_NEAR(m1_cpu, m1_cl, 1E-8);
}
TEST(MathMatrixCL, mdivide_left_tri_upper_cl_small) {
  mdivide_left_tri_upper_cl_test(3);
}
TEST(MathMatrixCL, mdivide_left_tri_upper_cl_mid) {
  mdivide_left_tri_upper_cl_test(100);
}
TEST(MathMatrixCL, mdivide_left_tri_upper_cl_big) {
  mdivide_left_tri_upper_cl_test(500);
}

void mdivide_left_tri_cl_test(int size) {
  boost::random::mt19937 rng;
  stan::math::matrix_d m1(size, size);
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < i; j++) {
      m1(i, j) = stan::math::uniform_rng(-5, 5, rng);
    }
    m1(i, i) = 20.0;
    for (int j = i + 1; j < size; j++) {
      m1(i, j) = 0.0;
    }
  }

  stan::math::opencl_context.tuning_opts().tri_inverse_size_worth_transfer
      = size * 2;

  stan::math::matrix_d m1_cpu
      = stan::math::mdivide_left_tri<Eigen::Lower>(m1, m1);

  stan::math::opencl_context.tuning_opts().tri_inverse_size_worth_transfer = 0;

  stan::math::matrix_d m1_cl
      = stan::math::mdivide_left_tri<Eigen::Lower>(m1, m1);

  EXPECT_MATRIX_NEAR(m1_cpu, m1_cl, 1E-8);
}
TEST(MathMatrixCL, mdivide_left_tri_cl_small) { mdivide_left_tri_cl_test(3); }
TEST(MathMatrixCL, mdivide_left_tri_cl_mid) { mdivide_left_tri_cl_test(100); }
TEST(MathMatrixCL, mdivide_left_tri_cl_big) { mdivide_left_tri_cl_test(500); }
#endif
