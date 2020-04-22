import numpy as np

import simpl
import simplpy
import simpl_helpers as sc
import simpl_test_dirs as sd

def GeometryTest():
  imageGeom = simpl.ImageGeom.CreateGeometry('ImageGeometry')
  assert imageGeom.Name == 'ImageGeometry'

  imageGeom.setDimensions(100, 1001, 200)
  dims = imageGeom.getDimensions()
  print('dims= %s' % dims)
  assert dims[0] == 100
  assert dims[1] == 1001
  assert dims[2] == 200

  imageGeom.setSpacing(1.0, 2.0, 3.0)
  imageRes = imageGeom.getSpacing()
  assert imageRes[0] == 1.0
  assert imageRes[1] == 2.0
  assert imageRes[2] == 3.0
  print('imageRes= %s' % imageRes)

  imageGeom.setOrigin(20, 45, 70)
  origin = imageGeom.getOrigin()
  assert origin[0] == 20
  assert origin[1] == 45
  assert origin[2] == 70
  print('origin= %s' % origin)

  infoString = imageGeom.getInfoString(simpl.HtmlFormat)
  print('%s' % infoString)

  boundingBox = imageGeom.getBoundingBox()
  assert boundingBox[0] == 20
  assert boundingBox[1] == 120
  assert boundingBox[2] == 45
  assert boundingBox[3] == 2047
  assert boundingBox[4] == 70
  assert boundingBox[5] == 670
  print('boundingBox= %s' % boundingBox)

if __name__ == '__main__':
  GeometryTest()
  print('GeometryTest Complete')
