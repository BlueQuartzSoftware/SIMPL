from typing import List, Tuple, Union
from Filter import Filter, FilterDelegatePy
from simpl import DataContainerArray, FilterDelegateCpp, FilterParameter, IntFilterParameter, DataArraySelectionFilterParameter, DataArrayPath

class ExampleFilter(Filter):
  def __init__(self) -> None:
    self.foo: int = 5
    self.path: DataArrayPath = DataArrayPath('', '', '')

  def _set_foo(self, value: int) -> None:
    self.foo = value

  def _get_foo(self) -> int:
    return self.foo

  def _set_path(self, value: DataArrayPath) -> None:
    self.path = value

  def _get_path(self) -> DataArrayPath:
    return self.path

  @staticmethod
  def name() -> str:
    return 'ExampleFilter'

  @staticmethod
  def uuid() -> str:
    return '{2f2a6f35-d488-59d7-bd8b-8a234e5700b3}'

  @staticmethod
  def group_name() -> str:
    return 'Example'

  @staticmethod
  def sub_group_name() -> str:
    return 'Sub Example'

  @staticmethod
  def human_label() -> str:
    return 'Example Filter'

  @staticmethod
  def version() -> str:
    return '1.0.0'

  @staticmethod
  def compiled_lib_name() -> str:
    return 'Python'

  def setup_parameters(self) -> List[FilterParameter]:
    req = DataArraySelectionFilterParameter.RequirementType()
    return [
      IntFilterParameter('Foo', 'Foo', 0, FilterParameter.Category.Parameter, self._set_foo, self._get_foo, -1),
      DataArraySelectionFilterParameter('Path', 'Path', DataArrayPath('', '', ''), FilterParameter.Category.RequiredArray, self._set_path, self._get_path, req, -1)
    ]

  def data_check(self, dca: DataContainerArray, delegate: Union[FilterDelegateCpp, FilterDelegatePy] = FilterDelegatePy()) -> Tuple[int, str]:
    am = dca.getAttributeMatrix(self.path)

    if am is None:
      return (-1, 'AttributeMatrix is None')

    da = am.getAttributeArray(self.path)
    if da is None:
      return (-2, 'DataArray is None')

    delegate.notifyStatusMessage('data_check finished!')

    return (0, 'Success')

  def _execute_impl(self, dca: DataContainerArray, delegate: Union[FilterDelegateCpp, FilterDelegatePy] = FilterDelegatePy()) -> Tuple[int, str]:
    delegate.notifyStatusMessage(f'foo = {self.foo}')

    da = dca.getAttributeMatrix(self.path).getAttributeArray(self.path)

    data = da.npview()
    delegate.notifyStatusMessage(f'before = {data}')
    data += self.foo
    delegate.notifyStatusMessage(f'after = {data}')

    delegate.notifyStatusMessage('execute finished!')
    return (0, 'Success')

filters = [ExampleFilter]
