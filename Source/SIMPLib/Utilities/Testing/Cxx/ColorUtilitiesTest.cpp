
#include <iostream>

#include "SIMPLib/Utilities/ColorTable.h"
#include "SIMPLib/Utilities/ColorUtilities.h"

#if 0
  	Black 	(0°,0%,0%) 	#000000 	(0,0,0)
  	White 	(0°,0%,100%) 	#FFFFFF 	(255,255,255)
  	Red 	(0°,100%,100%) 	#FF0000 	(255,0,0)
  	Lime 	(120°,100%,100%) 	#00FF00 	(0,255,0)
  	Blue 	(240°,100%,100%) 	#0000FF 	(0,0,255)
  	Yellow 	(60°,100%,100%) 	#FFFF00 	(255,255,0)
  	Cyan 	(180°,100%,100%) 	#00FFFF 	(0,255,255)
  	Magenta 	(300°,100%,100%) 	#FF00FF 	(255,0,255)
  	Silver 	(0°,0%,75%) 	#C0C0C0 	(192,192,192)
  	Gray 	(0°,0%,50%) 	#808080 	(128,128,128)
  	Maroon 	(0°,100%,50%) 	#800000 	(128,0,0)
  	Olive 	(60°,100%,50%) 	#808000 	(128,128,0)
  	Green 	(120°,100%,50%) 	#008000 	(0,128,0)
  	Purple 	(300°,100%,50%) 	#800080 	(128,0,128)
  	Teal 	(180°,100%,50%) 	#008080 	(0,128,128)
  	Navy 	(240°,100%,50%) 	#000080 	(0,0,128)
#endif

class ColorUtilitiesTest
{
public:
  ColorUtilitiesTest() = default;
  ~ColorUtilitiesTest() = default;

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestHSVConversion()
  {
    SIMPL::Rgb rgb = ColorUtilities::Hsv2Rgb(0.0, 0.0, 0.0);
    SIMPL::Rgb exemplar = RgbColor::dRgb(0, 0, 0, 255);
    bool isSame = RgbColor::compare(rgb, exemplar);
    DREAM3D_REQUIRED(rgb, ==, exemplar)

    rgb = ColorUtilities::Hsv2Rgb(0.0, 0.0, 1.0);
    exemplar = RgbColor::dRgb(255, 255, 255, 255);
    isSame = RgbColor::compare(rgb, exemplar);
    DREAM3D_REQUIRED(rgb, ==, exemplar)

    rgb = ColorUtilities::Hsv2Rgb(0.0, 1.0, 1.0);
    exemplar = RgbColor::dRgb(255, 0, 0, 255);
    isSame = RgbColor::compare(rgb, exemplar);
    DREAM3D_REQUIRED(rgb, ==, exemplar)

    rgb = ColorUtilities::Hsv2Rgb(120.0, 1.0, 1.0);
    exemplar = RgbColor::dRgb(0, 255, 0, 255);
    isSame = RgbColor::compare(rgb, exemplar);
    DREAM3D_REQUIRED(rgb, ==, exemplar)

    rgb = ColorUtilities::Hsv2Rgb(240.0, 1.0, 1.0);
    exemplar = RgbColor::dRgb(0, 0, 255, 255);
    isSame = RgbColor::compare(rgb, exemplar);
    DREAM3D_REQUIRED(rgb, ==, exemplar)

    rgb = ColorUtilities::Hsv2Rgb(60.0, 1.0, 1.0);
    exemplar = RgbColor::dRgb(255, 255, 0, 255);
    isSame = RgbColor::compare(rgb, exemplar);
    DREAM3D_REQUIRED(rgb, ==, exemplar)

    rgb = ColorUtilities::Hsv2Rgb(180.0, 1.0, 1.0);
    exemplar = RgbColor::dRgb(0, 255, 255, 255);
    isSame = RgbColor::compare(rgb, exemplar);
    DREAM3D_REQUIRED(rgb, ==, exemplar)
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### ColorUtilities Starting ####" << std::endl;

    int err = EXIT_SUCCESS;
#if !REMOVE_TEST_FILES
    DREAM3D_REGISTER_TEST(RemoveTestFiles())
#endif

    DREAM3D_REGISTER_TEST(TestHSVConversion())

#if REMOVE_TEST_FILES
    DREAM3D_REGISTER_TEST(RemoveTestFiles())
#endif
  }

public:
  ColorUtilitiesTest(const ColorUtilitiesTest&) = delete;            // Copy Constructor Not Implemented
  ColorUtilitiesTest(ColorUtilitiesTest&&) = delete;                 // Move Constructor
  ColorUtilitiesTest& operator=(const ColorUtilitiesTest&) = delete; // Copy Assignment Not Implemented
  ColorUtilitiesTest& operator=(ColorUtilitiesTest&&) = delete;      // Move Assignment
};
