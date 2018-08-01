#ifdef STAN_OPENCL

#include <stan/math/prim/mat.hpp>
#include <CL/cl.hpp>
#include <stan/math/gpu/opencl_context.hpp>
#include <stan/math/gpu/kernel_cl.hpp>
#include <stan/math/gpu/copy.hpp>
#include <stan/math/gpu/matrix_gpu.hpp>
#include <stan/math/gpu/transpose.hpp>
#include <gtest/gtest.h>
#include <algorithm>

TEST(MathGpu, nonexistent_kernel_compile) {
  EXPECT_THROW(stan::math::kernel_cl kernel("noname"), std::domain_error);
}
TEST(MathGpu, make_kernel) {
  stan::math::matrix_d m0(3, 3);
  stan::math::matrix_d m0_dst(3, 3);
  m0 << 1, 2, 3, 4, 5, 6, 7, 8, 9;
  stan::math::matrix_gpu m00(m0);

  stan::math::matrix_gpu m00_dst(m0.cols(), m0.rows());
  cl::CommandQueue cmdQueue = stan::math::opencl_context.queue();
  stan::math::kernel_cl kernel("transpose");
  kernel.set_args(m00_dst.buffer(), m00.buffer(), m00.rows(), m00.cols());
  cmdQueue.enqueueNDRangeKernel(kernel.compiled_, cl::NullRange,
                                  cl::NDRange(m00.rows(), m00.cols()),
                                  cl::NullRange, NULL, NULL);
  copy(m0_dst, m00_dst);
}
#endif