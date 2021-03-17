from abc import ABC, abstractmethod
from typing import List, Tuple, Union

from . import simpl

# can use typing.Protocol in 3.8
class FilterDelegatePy:
  def __init__(self) -> None:
      self.warning_code: int = 0
      self.warning_message: str = ''
      self.preflight: bool = False

  def notifyStatusMessage(self, message: str) -> None:
    print(message)

  def notifyProgressMessage(self, progress: int, message: str) -> None:
    print(f'{progress} {message}')

  def setWarningCondition(self, code: int, message: str) -> None:
    self.warning_code = code
    self.warning_message = message

class Filter(ABC):
  @staticmethod
  @abstractmethod
  def name() -> str:
    raise NotImplementedError

  @staticmethod
  @abstractmethod
  def group_name() -> str:
    raise NotImplementedError

  @staticmethod
  @abstractmethod
  def sub_group_name() -> str:
    raise NotImplementedError

  @staticmethod
  @abstractmethod
  def human_label() -> str:
    raise NotImplementedError

  @staticmethod
  @abstractmethod
  def version() -> str:
    raise NotImplementedError

  @staticmethod
  @abstractmethod
  def compiled_lib_name() -> str:
    raise NotImplementedError

  @abstractmethod
  def setup_parameters(self) -> List[simpl.FilterParameter]:
    raise NotImplementedError

  @abstractmethod
  def data_check(self, dca: simpl.DataContainerArray, delegate: Union[simpl.FilterDelegateCpp, FilterDelegatePy] = FilterDelegatePy()) -> Tuple[int, str]:
    raise NotImplementedError

  @abstractmethod
  def _execute_impl(self, dca: simpl.DataContainerArray, delegate: Union[simpl.FilterDelegateCpp, FilterDelegatePy] = FilterDelegatePy()) -> Tuple[int, str]:
    raise NotImplementedError

  def execute(self, dca: simpl.DataContainerArray, delegate: Union[simpl.FilterDelegateCpp, FilterDelegatePy] = FilterDelegatePy()) -> Tuple[int, str]:
    data_check_result = self.data_check(dca, delegate)
    if data_check_result[0] < 0:
      return data_check_result
    return self._execute_impl(dca, delegate)
