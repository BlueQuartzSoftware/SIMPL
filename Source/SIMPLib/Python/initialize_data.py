import numpy as np

from enum import IntEnum
from typing import List, Tuple, Union

from dream3d.Filter import Filter, FilterDelegatePy
from dream3d.simpl import DataContainerArray, FilterDelegateCpp, FilterParameter, IntFilterParameter, DataArrayPath, MultiDataArraySelectionFilterParameter, IGeometry, AttributeMatrix, LinkedChoicesFilterParameter, DoubleFilterParameter, RangeFilterParameter, SeparatorFilterParameter

class InitType(IntEnum):
  MANUAL = 0
  RANDOM = 1
  RANDOM_WITH_RANGE = 2

class InitializeDataPython(Filter):
  def __init__(self) -> None:
    self.cell_attribute_matrix_paths: List[DataArrayPath] = []
    self.x_min: int = 0
    self.y_min: int = 0
    self.z_min: int = 0
    self.x_max: int = 0
    self.y_max: int = 0
    self.z_max: int = 0
    self.init_type: InitType = InitType.MANUAL
    self.init_value: float = 0.0
    self.init_range: Tuple[float, float] = (0.0, 0.0)

  def _set_cell_attribute_matrix_paths(self, value: List[DataArrayPath]) -> None:
    self.cell_attribute_matrix_paths = value

  def _get_cell_attribute_matrix_paths(self) -> List[DataArrayPath]:
    return self.cell_attribute_matrix_paths

  def _set_x_min(self, value: int) -> None:
    self.x_min = value

  def _get_x_min(self) -> int:
    return self.x_min

  def _set_y_min(self, value: int) -> None:
    self.y_min = value

  def _get_y_min(self) -> int:
    return self.y_min

  def _set_z_min(self, value: int) -> None:
    self.z_min = value

  def _get_z_min(self) -> int:
    return self.z_min

  def _set_x_max(self, value: int) -> None:
    self.x_max = value

  def _get_x_max(self) -> int:
    return self.x_max

  def _set_y_max(self, value: int) -> None:
    self.y_max = value

  def _get_y_max(self) -> int:
    return self.y_max

  def _set_z_max(self, value: int) -> None:
    self.z_max = value

  def _get_z_max(self) -> int:
    return self.z_max

  def _set_init_type(self, value: InitType) -> None:
    self.init_type = value

  def _get_init_type(self) -> InitType:
    return self.init_type

  def _set_init_value(self, value: float) -> None:
    self.init_value = value

  def _get_init_value(self) -> float:
    return self.init_value

  def _set_init_range(self, value: Tuple[float, float]) -> None:
    self.init_range = value

  def _get_init_range(self) -> Tuple[float, float]:
    return self.init_range

  @staticmethod
  def name() -> str:
    return 'InitializeDataPython'

  @staticmethod
  def uuid() -> str:
    return '{d386e4ae-0460-4e43-a457-edeb594ccb09}'

  @staticmethod
  def group_name() -> str:
    return 'Example'

  @staticmethod
  def sub_group_name() -> str:
    return 'Sub Example'

  @staticmethod
  def human_label() -> str:
    return 'Initialize Data (Python)'

  @staticmethod
  def version() -> str:
    return '1.0.0'

  @staticmethod
  def compiled_lib_name() -> str:
    return 'Python'

  def setup_parameters(self) -> List[FilterParameter]:
    req = MultiDataArraySelectionFilterParameter.RequirementType([IGeometry.Type.Image], [AttributeMatrix.Type.Cell], [], [])
    return [
      SeparatorFilterParameter('Cell Data', FilterParameter.Category.RequiredArray),
      MultiDataArraySelectionFilterParameter('Cell Arrays', 'cell_attribute_matrix_paths', self.cell_attribute_matrix_paths, FilterParameter.Category.RequiredArray, self._set_cell_attribute_matrix_paths, self._get_cell_attribute_matrix_paths, req, -1),
      IntFilterParameter('X Min (Column)', 'x_min', self.x_min, FilterParameter.Category.Parameter, self._set_x_min, self._get_x_min, -1),
      IntFilterParameter('Y Min (Row)', 'y_min', self.y_min, FilterParameter.Category.Parameter, self._set_y_min, self._get_y_min, -1),
      IntFilterParameter('Z Min (Plane)', 'z_min', self.z_min, FilterParameter.Category.Parameter, self._set_z_min, self._get_z_min, -1),
      IntFilterParameter('X Max (Column)', 'x_max', self.x_max, FilterParameter.Category.Parameter, self._set_x_max, self._get_x_max, -1),
      IntFilterParameter('Y Max (Row)', 'y_max', self.y_max, FilterParameter.Category.Parameter, self._set_y_max, self._get_y_max, -1),
      IntFilterParameter('Z Max (Plane)', 'z_max', self.z_max, FilterParameter.Category.Parameter, self._set_z_max, self._get_z_max, -1),
      LinkedChoicesFilterParameter('Initialization Type', 'init_type', self.init_type, FilterParameter.Category.Parameter, self._set_init_type, self._get_init_type, ['Manual', 'Random', 'Random With Range'], ['init_value', 'init_range'], -1),
      DoubleFilterParameter('Initialization Value', 'init_value', self.init_value, FilterParameter.Category.Parameter, self._set_init_value, self._get_init_value, InitType.MANUAL),
      RangeFilterParameter('Initialization Range', 'init_range', self.init_range, FilterParameter.Category.Parameter, self._set_init_range, self._get_init_range, InitType.RANDOM_WITH_RANGE)
    ]

  def data_check(self, dca: DataContainerArray, delegate: Union[FilterDelegateCpp, FilterDelegatePy] = FilterDelegatePy()) -> Tuple[int, str]:
    if not self.cell_attribute_matrix_paths:
      return (-5550, 'At least one data array must be selected')

    attribute_matrix_path = self.cell_attribute_matrix_paths[0]

    if not dca.doesAttributeMatrixExist(attribute_matrix_path):
      return (-301, 'Failed to find AttributeMatrix')

    dc = dca.getDataContainer(attribute_matrix_path)

    if not dc.Geometry:
      return (-302, 'DataContainer has no geometry')

    if dc.Geometry.getGeometryType() != IGeometry.Type.Image:
      return (-303, 'Wrong geometry type')

    image = dc.Geometry

    if self.x_max < self.x_min:
      return (-5551, f'X Max ({self.x_max}) less than X Min ({self.x_min})')

    if self.y_max < self.y_min:
      return (-5552, f'Y Max ({self.y_max}) less than Y Min ({self.y_min})')

    if self.z_max < self.z_min:
      return (-5553, f'Z Max ({self.z_max}) less than Z Min ({self.z_min})')

    if self.x_min < 0:
      return (-5554, f'X Min ({self.x_min}) less than 0')

    if self.y_min < 0:
      return (-5555, f'Y Min ({self.y_min}) less than 0')

    if self.z_min < 0:
      return (-5556, f'Z Min ({self.z_min}) less than 0')

    if self.x_max > image.getXPoints() - 1:
      return (-5557, f'The X Max you entered of {self.x_max} is greater than your Max X Point of {image.getXPoints() - 1}')

    if self.y_max > image.getYPoints() - 1:
      return (-5558, f'The Y Max you entered of {self.y_max} is greater than your Max Y Point of {image.getYPoints() - 1}')

    if self.z_max > image.getZPoints() - 1:
      return (-5559, f'The Z Max you entered of {self.z_max} is greater than your Max Z Point of {image.getZPoints() - 1}')

    am = dc.getAttributeMatrix(attribute_matrix_path)

    data_arrays = [(array_path, am.getAttributeArray(array_path.DataArrayName)) for array_path in self.cell_attribute_matrix_paths]

    for array_path, data_array in data_arrays:
      if not data_array:
        return (-5560, f'{array_path} does not exist')
      dtype = data_array.dtype
      if dtype == np.float32 or dtype == np.float64:
        min_value = np.finfo(dtype).min
        max_value = np.finfo(dtype).max
      else:
        min_value = np.iinfo(dtype).min
        max_value = np.iinfo(dtype).max
      if self.init_type == InitType.MANUAL:
        if self.init_value < min_value or self.init_value > max_value:
          return (-4000, f'{data_array.name}: The initialization value could not be converted. The valid range is {min_value} to {max_value}')
      elif self.init_type == InitType.RANDOM_WITH_RANGE:
        range_min = self.init_range[0]
        range_max = self.init_range[1]
        if range_min > range_max:
          return (-5550, 'Invalid initialization range. Minimum value is larger than maximum value.')
        if range_min < min_value or range_max > max_value:
          return (-4001, f'{data_array.name}: The initialization range can only be from {min_value} to {max_value}')
        if range_min == range_max:
          return (-4002, 'The initialization range must have differing values')

    return (0, 'Success')

  def _execute_impl(self, dca: DataContainerArray, delegate: Union[FilterDelegateCpp, FilterDelegatePy] = FilterDelegatePy()) -> Tuple[int, str]:
    path = self.cell_attribute_matrix_paths[0]
    dc = dca.getDataContainer(path)
    udims = dc.Geometry.getDimensions()
    am = dca.getAttributeMatrix(path)

    dims = [np.int64(udims[2]), np.int64(udims[1]), np.int64(udims[0])]

    data_arrays = [am.getAttributeArray(array_path.DataArrayName) for array_path in self.cell_attribute_matrix_paths]

    for data_array in data_arrays:
      dtype = data_array.dtype
      data = data_array.npview()
      data_view = data.view([('', dtype)] * np.prod(data_array.cdims)).reshape(dims)
      data_slice = data_view[self.z_min:self.z_max + 1, self.y_min:self.y_max + 1, self.x_min:self.x_max + 1]
      if self.init_type == InitType.MANUAL:
        data_slice[:] = self.init_value
      else:
        if self.init_type == InitType.RANDOM_WITH_RANGE:
          range_min = self.init_range[0]
          range_max = self.init_range[1]
        else:
          if dtype == np.float32 or dtype == np.float64:
            range_min = np.finfo(dtype).min
            range_max = np.finfo(dtype).max
          else:
            range_min = np.iinfo(dtype).min
            range_max = np.iinfo(dtype).max
        rng = np.random.default_rng()
        if dtype == np.float32 or dtype == np.float64:
          data_slice[:] = rng.uniform(range_min, range_max, size=data_slice.size).reshape(data_slice.shape)
        else:
          data_slice[:] = rng.integers(range_min, range_max, size=data_slice.size).reshape(data_slice.shape)

    return (0, 'Success')

filters = [InitializeDataPython]
