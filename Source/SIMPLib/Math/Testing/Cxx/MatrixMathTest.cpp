
#include <stdlib.h>

#include <cmath>
#include <iostream>

#include "SIMPLib/Math/MatrixMath.h"

#include "SIMPLib/Testing/SIMPLTestFileLocations.h"
#include "SIMPLib/Testing/UnitTestSupport.hpp"

class MatrixMathTest
{

public:
  MatrixMathTest() = default;

  virtual ~MatrixMathTest() = default;

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T> void print3x3(T matrix[3][3], const std::string& name)
  {
    std::cout << "Matrix: " << name << std::endl;
    std::cout << "[" << matrix[0][0] << " " << matrix[0][1] << " " << matrix[0][2] << "]" << std::endl;
    std::cout << "[" << matrix[1][0] << " " << matrix[1][1] << " " << matrix[1][2] << "]" << std::endl;
    std::cout << "[" << matrix[2][0] << " " << matrix[2][1] << " " << matrix[2][2] << "]" << std::endl;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void Multiply3x3With3x3Test()
  {
    float matrix1[3][3] = {{-1, 4, -6}, {8, 5, 16}, {2, 8, 5}};

    float matrix2[3][3] = {{12, 7, 6}, {8, 0, 5}, {3, 2, 4}};

    float correct[3][3] = {{2, -19, -10}, {184, 88, 137}, {103, 24, 72}};
    float multiple[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};

    MatrixMath::Multiply3x3with3x3(matrix1, matrix2, multiple);

    for(int r = 0; r < 3; r++)
    {
      for(int c = 0; c < 3; c++)
      {
        DREAM3D_REQUIRE_EQUAL(multiple[r][c], correct[r][c])
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void Multiply3x3With3x1FloatTest()
  {
    float matrix1[3][3] = {{3, 5, 1}, {2, 6, 8}, {9, 4, 7}};
    float matrix2[3] = {3, 2, 1};
    float correct[3] = {20, 26, 42};

    float multiple[3];

    MatrixMath::Multiply3x3with3x1(matrix1, matrix2, multiple);

    for(int r = 0; r < 3; r++)
    {
      DREAM3D_REQUIRE_EQUAL(multiple[r], correct[r])
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void Multiply3x3With3x1DoubleTest()
  {
    double matrix1[3][3] = {{3, 5, 1}, {2, 6, 8}, {9, 4, 7}};
    double matrix2[3] = {3, 2, 1};
    double correct[3] = {20, 26, 42};

    double multiple[3];

    MatrixMath::Multiply3x3with3x1(matrix1, matrix2, multiple);

    for(int r = 0; r < 3; r++)
    {
      DREAM3D_REQUIRE_EQUAL(multiple[r], correct[r])
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void Add3x1sTest()
  {
    float matrix1[3] = {1, 2, 3};
    float matrix2[3] = {4, 5, 6};

    float correct[3] = {5, 7, 9};

    float sum[3];

    MatrixMath::Add3x1s(matrix1, matrix2, sum);

    for(int r = 0; r < 3; r++)
    {
      DREAM3D_REQUIRE_EQUAL(sum[r], correct[r])
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void Subtract3x1sTest()
  {
    float matrix1[3] = {1, 2, 3};
    float matrix2[3] = {4, 5, 6};

    float correct[3] = {-3, -3, -3};

    float difference[3];

    MatrixMath::Subtract3x1s(matrix1, matrix2, difference);

    for(int r = 0; r < 3; r++)
    {
      DREAM3D_REQUIRE_EQUAL(difference[r], correct[r])
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void Add3x3sTest()
  {
    float matrix1[3][3] = {{1, 2, 3}, {2, 3, 4}, {3, 4, 5}};
    float matrix2[3][3] = {{4, 5, 6}, {4, 2, 5}, {7, 1, 6}};

    float correct[3][3] = {{5, 7, 9}, {6, 5, 9}, {10, 5, 11}};

    float sum[3][3];

    MatrixMath::Add3x3s(matrix1, matrix2, sum);

    for(int r = 0; r < 3; r++)
    {
      for(int c = 0; c < 3; c++)
      {
        DREAM3D_REQUIRE_EQUAL(sum[r][c], correct[r][c])
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void Subtract3x3sTest()
  {
    float matrix1[3][3] = {{1, 2, 3}, {2, 3, 4}, {3, 4, 5}};
    float matrix2[3][3] = {{4, 5, 6}, {4, 2, 5}, {7, 1, 6}};

    float correct[3][3] = {{-3, -3, -3}, {-2, 1, -1}, {-4, 3, -1}};

    float difference[3][3];

    MatrixMath::Subtract3x3s(matrix1, matrix2, difference);

    for(int r = 0; r < 3; r++)
    {
      for(int c = 0; c < 3; c++)
      {
        DREAM3D_REQUIRE_EQUAL(difference[r][c], correct[r][c])
      }
    }
  }

  // ---------------------------------------------------------------------------- -
  //
  // -----------------------------------------------------------------------------
  void Multiply3x1WithConstantTest()
  {
    float matrix[3] = {5, 2, 7};
    float constant = 3;
    float correct[3] = {15, 6, 21};

    MatrixMath::Multiply3x1withConstant(matrix, constant);

    for(int r = 0; r < 3; r++)
    {
      DREAM3D_REQUIRE_EQUAL(matrix[r], correct[r])
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void Divide4x1WithConstantTest()
  {
    float matrix[4] = {2, 12, 6};
    float constant = 4;
    float correct[4] = {0.5f, 3, 1.5f};

    MatrixMath::Divide4x1withConstant(matrix, constant);

    for(int r = 0; r < 3; r++)
    {
      DREAM3D_REQUIRE_EQUAL(matrix[r], correct[r])
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void Multiply3x3WithConstantTest()
  {
    float matrix[3][3] = {{1, 4, 3}, {2, 7, 8}, {9, 6, 0}};
    float constant = 6;
    float correct[3][3] = {{6, 24, 18}, {12, 42, 48}, {54, 36, 0}};

    MatrixMath::Multiply3x3withConstant(matrix, constant);

    for(int r = 0; r < 3; r++)
    {
      for(int c = 0; c < 3; c++)
      {
        DREAM3D_REQUIRE_EQUAL(matrix[r][c], correct[r][c])
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void Minors3x3Test()
  {
    float matrix[3][3] = {{3.0f, 0.0f, 2.0f}, {2.0f, 0.0f, -2.0f}, {0.0f, 1.0f, 1.0f}};
    float minors[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    float correct[3][3] = {{2.0f, 2.0f, 2.0f}, {-2.0f, 3.0f, 3.0f}, {-0.0f, -10.0f, 0.0f}};

    MatrixMath::Minors3x3(matrix, minors);
    //    print3x3(matrix, std::string("Input Matrix"));
    //    print3x3(minors, std::string("Minors Matrix"));

    for(int r = 0; r < 3; r++)
    {
      for(int c = 0; c < 3; c++)
      {
        DREAM3D_REQUIRE_EQUAL(minors[r][c], correct[r][c])
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void Cofactor3x3Test()
  {
    float matrix[3][3] = {{3.0f, 0.0f, 2.0f}, {2.0f, 0.0f, -2.0f}, {0.0f, 1.0f, 1.0f}};
    float cofactors[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    float correct[3][3] = {{2.0f, -2.0f, 2.0f}, {2.0f, 3.0f, -3.0f}, {0.0f, 10.0f, 0.0f}};

    MatrixMath::Cofactor3x3(matrix, cofactors);
    //    print3x3(matrix, std::string("Input Matrix"));
    //    print3x3(cofactors, std::string("Cofactors Matrix"));

    for(int r = 0; r < 3; r++)
    {
      for(int c = 0; c < 3; c++)
      {
        DREAM3D_REQUIRE_EQUAL(cofactors[r][c], correct[r][c])
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void Transpose3x3Test()
  {
    float matrix[3][3] = {{3.0f, 0.0f, 2.0f}, {2.0f, 0.0f, -2.0f}, {0.0f, 1.0f, 1.0f}};
    float transpose[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    float correct[3][3] = {{3.0f, 2.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {2.0f, -2.0f, 1.0f}};

    MatrixMath::Transpose3x3(matrix, transpose);
    //    print3x3(matrix, std::string("Input Matrix"));
    //    print3x3(transpose, std::string("Transpose Matrix"));

    for(int r = 0; r < 3; r++)
    {
      for(int c = 0; c < 3; c++)
      {
        DREAM3D_REQUIRE_EQUAL(transpose[r][c], correct[r][c])
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void Adjoint3x3Test()
  {
    float matrix[3][3] = {{3.0f, 0.0f, 2.0f}, {2.0f, 0.0f, -2.0f}, {0.0f, 1.0f, 1.0f}};
    float adjoint[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    float correct[3][3] = {{2.0f, 2.0f, 0.0f}, {-2.0f, 3.0f, 10.0f}, {2.0f, -3.0f, 0.0f}};

    MatrixMath::Adjoint3x3(matrix, adjoint);
    //    print3x3(matrix, std::string("Input Matrix"));
    //    print3x3(adjoint, std::string("Adjoint Matrix"));

    for(int r = 0; r < 3; r++)
    {
      for(int c = 0; c < 3; c++)
      {
        DREAM3D_REQUIRE_EQUAL(adjoint[r][c], correct[r][c])
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void Determinant3x3Test()
  {
    float matrix[3][3] = {{3.0f, 0.0f, 2.0f}, {2.0f, 0.0f, -2.0f}, {0.0f, 1.0f, 1.0f}};

    float correct = 10.0f;
    float value = MatrixMath::Determinant3x3(matrix);
    //    print3x3(matrix, std::string("Input Matrix"));
    //    print3x3(adjoint, std::string("Adjoint Matrix"));
    DREAM3D_REQUIRE_EQUAL(value, correct)
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void Copy3x3Test()
  {
    float matrix[3][3] = {{1, 3, 4}, {2, 5, 3}, {0, 9, 7}};
    float copy[3][3];

    MatrixMath::Copy3x3(matrix, copy);

    for(int r = 0; r < 3; r++)
    {
      for(int c = 0; c < 3; c++)
      {
        DREAM3D_REQUIRE_EQUAL(matrix[r][c], copy[r][c])
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void Copy3x1Test()
  {
    float matrix[3] = {5, 7, 2};
    float copy[3];

    MatrixMath::Copy3x1(matrix, copy);

    for(int r = 0; r < 3; r++)
    {
      DREAM3D_REQUIRE_EQUAL(matrix[r], copy[r])
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void Invert3x3Test()
  {
    /*
     * C++ 2D Arrays are A[row][col]
     */
    float matrix[3][3] = {{3.0f, 0.0f, 2.0f}, {2.0f, 0.0f, -2.0f}, {0.0f, 1.0f, 1.0f}};
    float inverse[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    float correct[3][3] = {{0.2f, 0.2f, 0.0f}, {-0.2f, 0.3f, 1.0f}, {0.2f, -0.3f, 0.0f}};

    MatrixMath::Invert3x3(matrix, inverse);
    //    print3x3(matrix, std::string("Input Matrix"));
    //    print3x3(inverse, std::string("Inverse Matrix"));

    for(int r = 0; r < 3; r++)
    {
      for(int c = 0; c < 3; c++)
      {
        DREAM3D_REQUIRE_EQUAL(inverse[r][c], correct[r][c])
      }
    }

    float identity[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    MatrixMath::Identity3x3(correct);
    MatrixMath::Multiply3x3with3x3(matrix, inverse, identity);

    //    print3x3(identity, std::string("Identity Matrix"));
    for(int r = 0; r < 3; r++)
    {
      for(int c = 0; c < 3; c++)
      {
        DREAM3D_REQUIRE_EQUAL(identity[r][c], correct[r][c])
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void Identity3x3Test()
  {
    float correct[3][3] = {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};
    float matrix[3][3];

    MatrixMath::Identity3x3(matrix);

    for(int r = 0; r < 3; r++)
    {
      for(int c = 0; c < 3; c++)
      {
        DREAM3D_REQUIRE_EQUAL(matrix[r][c], correct[r][c])
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void Normalize3x3Test()
  {
    float matrix[3][3] = {{3, 4, 1}, {2, 8, 6}, {4, 2, 10}};
    float correct[3][3] = {{0.557086f, 0.4364357f, 0.0854357f}, {0.371390f, 0.8728716f, 0.512614f}, {0.742781f, 0.2182179f, 0.854357f}};

    float threshold = 0.001f;

    MatrixMath::Normalize3x3(matrix);

    for(int r = 0; r < 3; r++)
    {
      for(int c = 0; c < 3; c++)
      {
        DREAM3D_REQUIRE(std::abs(matrix[r][c] - correct[r][c]) < threshold)
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void Normalize3x1FloatTest()
  {
    float matrix[3] = {2, 4, 8};
    float correct[3] = {0.218217f, 0.436435f, 0.8728715f};

    MatrixMath::Normalize3x1(matrix);

    float threshold = 0.001f;

    for(int r = 0; r < 3; r++)
    {
      DREAM3D_REQUIRE(std::abs(matrix[r] - correct[r]) < threshold)
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void Normalize3x1DoubleTest()
  {
    double matrix[3] = {2, 4, 8};
    double correct[3] = {0.218217, 0.436435, 0.8728715};

    MatrixMath::Normalize3x1(matrix);

    double threshold = 0.001;

    for(int r = 0; r < 3; r++)
    {
      DREAM3D_REQUIRE(std::abs(matrix[r] - correct[r]) < threshold)
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void Normalize3x1DoubleReferenceTest()
  {
    double matrix[3] = {2, 4, 8};
    double correct[3] = {0.218217, 0.436435, 0.8728715};

    MatrixMath::Normalize3x1(matrix[0], matrix[1], matrix[2]);

    double threshold = 0.001;

    for(int r = 0; r < 3; r++)
    {
      DREAM3D_REQUIRE(std::abs(matrix[r] - correct[r]) < threshold)
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void Sort3x1AscendingTest()
  {
    float matrix[3] = {3, 6, 1};
    float correct[3] = {1, 3, 6};

    MatrixMath::Sort3x1Ascending(matrix);

    for(int r = 0; r < 3; r++)
    {
      DREAM3D_REQUIRE_EQUAL(matrix[r], correct[r])
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void FindIndexOfMaxVal3x1Test()
  {
    float matrix[3] = {2, 9, 7};
    float correct = 1;
    float index = MatrixMath::FindIndexOfMaxVal3x1(matrix);

    DREAM3D_REQUIRE_EQUAL(index, correct)
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void Magnitude3x1Test()
  {
    float matrix[3] = {1, 7, 4};
    float correct = 8.124038f;

    float magnitude = MatrixMath::Magnitude3x1(matrix);

    float threshold = 0.001f;

    DREAM3D_REQUIRE(std::abs(magnitude - correct) < threshold)
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void Magnitude4x1Test()
  {
    float matrix[4] = {2, 6, 9, 1};
    float correct = 11.04536f;

    float magnitude = MatrixMath::Magnitude4x1(matrix);

    float threshold = 0.001f;

    DREAM3D_REQUIRE(std::abs(magnitude - correct) < threshold)
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void DotProduct3x1FloatTest()
  {
    float matrix1[3] = {3, 8, 6};
    float matrix2[3] = {2, 4, 5};
    float correct = 68;

    float dotProduct = MatrixMath::DotProduct3x1(matrix1, matrix2);

    DREAM3D_REQUIRE_EQUAL(dotProduct, correct)
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void DotProduct3x1DoubleTest()
  {
    double matrix1[3] = {0.5, 0.25, 6};
    double matrix2[3] = {2, 4, 5};
    double correct = 32;

    double dotProduct = MatrixMath::DotProduct3x1(matrix1, matrix2);

    DREAM3D_REQUIRE_EQUAL(dotProduct, correct)
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void CrossProductFloatTest()
  {
    float matrix1[3] = {1, 2, 3};
    float matrix2[3] = {4, 5, 6};
    float correct[3] = {-3, 6, -3};

    float crossProduct[3];

    MatrixMath::CrossProduct(matrix1, matrix2, crossProduct);

    float threshold = 0.001f;

    for(int r = 0; r < 3; r++)
    {
      DREAM3D_REQUIRE(std::abs(crossProduct[r] - correct[r]) < threshold)
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void CrossProductDoubleTest()
  {
    double matrix1[3] = {1, 2, 3};
    double matrix2[3] = {4, 5, 6};
    double correct[3] = {-3, 6, -3};

    double crossProduct[3];

    MatrixMath::CrossProduct(matrix1, matrix2, crossProduct);

    double threshold = 0.001;

    for(int r = 0; r < 3; r++)
    {
      DREAM3D_REQUIRE(std::abs(crossProduct[r] - correct[r]) < threshold)
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int RunTest()
  {
    Multiply3x3With3x3Test();
    Multiply3x3With3x1DoubleTest();
    Multiply3x3With3x1FloatTest();

    Add3x1sTest();
    Subtract3x1sTest();
    Add3x3sTest();
    Subtract3x3sTest();

    Multiply3x1WithConstantTest();
    Divide4x1WithConstantTest();
    Multiply3x3WithConstantTest();

    Transpose3x3Test();
    Invert3x3Test();
    Adjoint3x3Test();
    Cofactor3x3Test();
    Minors3x3Test();
    Determinant3x3Test();

    Copy3x3Test();
    Copy3x1Test();

    Identity3x3Test();
    Normalize3x3Test();
    Normalize3x1FloatTest();
    Normalize3x1DoubleTest();
    Normalize3x1DoubleReferenceTest();

    Sort3x1AscendingTest();
    FindIndexOfMaxVal3x1Test();

    Magnitude3x1Test();
    Magnitude4x1Test();

    DotProduct3x1DoubleTest();
    DotProduct3x1FloatTest();

    CrossProductDoubleTest();
    CrossProductFloatTest();

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### MatrixMathTest Starting ####" << std::endl;

    int err = EXIT_SUCCESS;
    DREAM3D_REGISTER_TEST(RunTest())
    DREAM3D_REGISTER_TEST(Minors3x3Test())
    DREAM3D_REGISTER_TEST(Cofactor3x3Test())
    DREAM3D_REGISTER_TEST(Transpose3x3Test())
    DREAM3D_REGISTER_TEST(Adjoint3x3Test())
    DREAM3D_REGISTER_TEST(Determinant3x3Test())

    DREAM3D_REGISTER_TEST(Invert3x3Test())
  }

private:
  MatrixMathTest(const MatrixMathTest&); // Copy Constructor Not Implemented
  void operator=(const MatrixMathTest&); // Move assignment Not Implemented
};
