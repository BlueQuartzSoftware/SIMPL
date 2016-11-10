
#include <stdlib.h>

#include <iostream>

#include "SIMPLib/Math/MatrixMath.h"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"

#include "SIMPLTestFileLocations.h"

class MatrixMathTest
{

public:
  MatrixMathTest()
  {
  }

  virtual ~MatrixMathTest()
  {
  }


  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template<typename T>
  void print3x3(T matrix[3][3], const std::string &name)
  {
    std::cout << "Matrix: " << name << std::endl;
    std::cout << "[" << matrix[0][0] << " " << matrix[0][1] << " " << matrix[0][2] << "]" << std::endl;
    std::cout << "[" << matrix[1][0] << " " << matrix[1][1] << " " << matrix[1][2] << "]" << std::endl;
    std::cout << "[" << matrix[2][0] << " " << matrix[2][1] << " " << matrix[2][2] << "]" << std::endl;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void Minors3x3Test()
  {
    float matrix[3][3] = {{3.0f, 0.0f, 2.0f},
                          {2.0f, 0.0f, -2.0f},
                          {0.0f, 1.0f, 1.0f}};
    float minors[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    float correct[3][3] = {{2.0f, 2.0f, 2.0f},
                           {-2.0f, 3.0f, 3.0f},
                           {-0.0f, -10.0f, 0.0f}};

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
    float matrix[3][3] = {{3.0f, 0.0f, 2.0f},
                          {2.0f, 0.0f, -2.0f},
                          {0.0f, 1.0f, 1.0f}};
    float cofactors[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    float correct[3][3] = {{2.0f, -2.0f, 2.0f},
                           {2.0f, 3.0f, -3.0f},
                           {0.0f, 10.0f, 0.0f}};

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
    float matrix[3][3] = {{3.0f, 0.0f, 2.0f},
                          {2.0f, 0.0f, -2.0f},
                          {0.0f, 1.0f, 1.0f}};
    float transpose[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    float correct[3][3] = {{3.0f, 2.0f, 0.0f},
                           {0.0f, 0.0f, 1.0f},
                           {2.0f, -2.0f, 1.0f}};

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
    float matrix[3][3] = {{3.0f, 0.0f, 2.0f},
                          {2.0f, 0.0f, -2.0f},
                          {0.0f, 1.0f, 1.0f}};
    float adjoint[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    float correct[3][3] = {{2.0f, 2.0f, 0.0f},
                           {-2.0f, 3.0f, 10.0f},
                           {2.0f, -3.0f, 0.0f}};

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
    float matrix[3][3] = {{3.0f, 0.0f, 2.0f},
                          {2.0f, 0.0f, -2.0f},
                          {0.0f, 1.0f, 1.0f}};

    float correct = 10.0f;
    float value = MatrixMath::Determinant3x3(matrix);
//    print3x3(matrix, std::string("Input Matrix"));
//    print3x3(adjoint, std::string("Adjoint Matrix"));
    DREAM3D_REQUIRE_EQUAL(value, correct)


  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void Invert3x3Test()
  {
    /*
     * C++ 2D Arrays are A[row][col]
     */
    float matrix[3][3] = {{3.0f, 0.0f, 2.0f},
                          {2.0f, 0.0f, -2.0f},
                          {0.0f, 1.0f, 1.0f}};
    float inverse[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    float correct[3][3] = {{0.2f, 0.2f, 0.0f},
                           {-0.2f, 0.3f, 1.0f},
                           {0.2f, -0.3f, 0.0f}};

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
  int RunTest()
  {

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
  void operator=(const MatrixMathTest&); // Operator '=' Not Implemented
};
