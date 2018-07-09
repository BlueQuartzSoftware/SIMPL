#pragma once

// Copied from sitkKernel.h (SimpleITK)
namespace itk
{
namespace simple
{

enum KernelEnum
{
  sitkAnnulus = 0,
  sitkBall = 1,
  sitkBox = 2,
  sitkCross = 3
};

enum SeedEnum
{
  /// A sentinel value used for "seed" parameters to indicate it
  /// should be initialized by the wall clock for pseudo-random behavior.
  sitkWallClock = 0
};

} // end namespace simple
} // end namespace itk

// end of Copied from sitkKernel.h (SimpleITK)

