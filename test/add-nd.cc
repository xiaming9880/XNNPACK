// Copyright 2019 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <gtest/gtest.h>

#include "binary-elementwise-operator-tester.h"


TEST(ADD_ND_F32, vector_x_vector) {
  BinaryElementwiseOperatorTester()
    .operation_type(BinaryElementwiseOperatorTester::OperationType::Add)
    .input1_shape({2})
    .input2_shape({2})
    .iterations(1)
    .TestF32();
}

TEST(ADD_ND_F32, vector_x_scalar) {
  BinaryElementwiseOperatorTester()
    .operation_type(BinaryElementwiseOperatorTester::OperationType::Add)
    .input1_shape({2})
    .iterations(1)
    .TestF32();
  BinaryElementwiseOperatorTester()
    .operation_type(BinaryElementwiseOperatorTester::OperationType::Add)
    .input1_shape({2})
    .input2_shape({1})
    .iterations(1)
    .TestF32();
}

TEST(ADD_ND_F32, scalar_x_vector) {
  BinaryElementwiseOperatorTester()
    .operation_type(BinaryElementwiseOperatorTester::OperationType::Add)
    .input2_shape({2})
    .iterations(1)
    .TestF32();
  BinaryElementwiseOperatorTester()
    .operation_type(BinaryElementwiseOperatorTester::OperationType::Add)
    .input1_shape({1})
    .input2_shape({2})
    .iterations(1)
    .TestF32();
}

TEST(ADD_ND_F32, matrix_x_matrix) {
  BinaryElementwiseOperatorTester()
    .operation_type(BinaryElementwiseOperatorTester::OperationType::Add)
    .input1_shape({2, 3})
    .input2_shape({2, 3})
    .iterations(1)
    .TestF32();
}

TEST(ADD_ND_F32, matrix_x_row) {
  BinaryElementwiseOperatorTester()
    .operation_type(BinaryElementwiseOperatorTester::OperationType::Add)
    .input1_shape({2, 3})
    .input2_shape({1, 3})
    .iterations(1)
    .TestF32();
  BinaryElementwiseOperatorTester()
    .operation_type(BinaryElementwiseOperatorTester::OperationType::Add)
    .input1_shape({2, 3})
    .input2_shape({3})
    .iterations(1)
    .TestF32();
}

TEST(ADD_ND_F32, matrix_x_column) {
  BinaryElementwiseOperatorTester()
    .operation_type(BinaryElementwiseOperatorTester::OperationType::Add)
    .input1_shape({2, 3})
    .input2_shape({2, 1})
    .iterations(1)
    .TestF32();
}

TEST(ADD_ND_F32, matrix_x_scalar) {
  BinaryElementwiseOperatorTester()
    .operation_type(BinaryElementwiseOperatorTester::OperationType::Add)
    .input1_shape({2, 3})
    .input2_shape({1, 1})
    .iterations(1)
    .TestF32();
  BinaryElementwiseOperatorTester()
    .operation_type(BinaryElementwiseOperatorTester::OperationType::Add)
    .input1_shape({2, 3})
    .input2_shape({1})
    .iterations(1)
    .TestF32();
  BinaryElementwiseOperatorTester()
    .operation_type(BinaryElementwiseOperatorTester::OperationType::Add)
    .input1_shape({2, 3})
    .iterations(1)
    .TestF32();
}

TEST(ADD_ND_F32, row_x_matrix) {
  BinaryElementwiseOperatorTester()
    .operation_type(BinaryElementwiseOperatorTester::OperationType::Add)
    .input1_shape({1, 3})
    .input2_shape({2, 3})
    .iterations(1)
    .TestF32();
  BinaryElementwiseOperatorTester()
    .operation_type(BinaryElementwiseOperatorTester::OperationType::Add)
    .input1_shape({3})
    .input2_shape({2, 3})
    .iterations(1)
    .TestF32();
}

TEST(ADD_ND_F32, column_x_matrix) {
  BinaryElementwiseOperatorTester()
    .operation_type(BinaryElementwiseOperatorTester::OperationType::Add)
    .input1_shape({2, 1})
    .input2_shape({2, 3})
    .iterations(1)
    .TestF32();
}

TEST(ADD_ND_F32, scalar_x_matrix) {
  BinaryElementwiseOperatorTester()
    .operation_type(BinaryElementwiseOperatorTester::OperationType::Add)
    .input1_shape({1, 1})
    .input2_shape({2, 3})
    .iterations(1)
    .TestF32();
  BinaryElementwiseOperatorTester()
    .operation_type(BinaryElementwiseOperatorTester::OperationType::Add)
    .input1_shape({1})
    .input2_shape({2, 3})
    .iterations(1)
    .TestF32();
  BinaryElementwiseOperatorTester()
    .operation_type(BinaryElementwiseOperatorTester::OperationType::Add)
    .input2_shape({2, 3})
    .iterations(1)
    .TestF32();
}