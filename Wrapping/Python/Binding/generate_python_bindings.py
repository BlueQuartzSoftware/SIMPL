import argparse
import filecmp
import os
import re

from collections import Counter, defaultdict
from contextlib import ExitStack
from pathlib import Path

# typing.Final isn't available until 3.8+
from typing import DefaultDict, Dict, Generator, List, Optional, Tuple

PYB11_BEGIN_BINDINGS: str = 'PYB11_BEGIN_BINDINGS'
PYB11_END_BINDINGS: str = 'PYB11_END_BINDINGS'
PYB11_PROPERTY: str = 'PYB11_PROPERTY'
PYB11_METHOD: str = 'PYB11_METHOD'
PYB11_STATIC_CREATION: str = 'PYB11_STATIC_CREATION'
PYB11_CREATION: str = 'PYB11_CREATION'
PYB11_ENUMERATION: str = 'PYB11_ENUMERATION'
PYB11_STATIC_NEW_MACRO: str = 'PYB11_STATIC_NEW_MACRO'
PYB11_FILTER_NEW_MACRO: str = 'PYB11_FILTER_NEW_MACRO'
PYB11_SHARED_POINTERS: str = 'PYB11_SHARED_POINTERS'
PYB11_FILTER: str = 'PYB11_FILTER'
PYB11_CUSTOM: str = 'PYB11_CUSTOM'
PYB11_FIELD: str = 'PYB11_FIELD'

PYB11_SUPERCLASS: str = 'SUPERCLASS'
PYB11_READ: str = 'READ'
PYB11_WRITE: str = 'WRITE'
PYB11_CONST_GET_OVERLOAD: str = 'CONST_GET_OVERLOAD'
PYB11_CONST_METHOD: str = 'CONST_METHOD'
PYB11_RETURN_VALUE_POLICY: str = 'RETURN_VALUE_POLICY'
PYB11_ARGS: str = 'ARGS'
PYB11_OVERLOAD: str = 'OVERLOAD'

OBSERVER_ARG_NAME: str = 'observer'

class PyField():
  def __init__(self):
    self.cpp_name: str = ''
    self.py_name: str = ''

class PyProperty():
  def __init__(self):
    self.name: str = ''
    self.type: str = ''
    self.read: str = ''
    self.write: str = ''
    self.is_const_overload: bool = False

class PyMethod():
  def __init__(self):
    self.name: str = ''
    self.return_type: str = ''
    self.args: List[str] = []
    self.arg_types: List[str] = []
    self.is_const: bool = False
    self.return_value_policy: str = ''
    self.is_overload: bool = False

class PyStaticCreation():
  def __init__(self):
    self.name: str = ''
    self.args: List[str] = []
    self.is_overload: bool = False

class PyConstructor():
  def __init__(self):
    self.args: List[str] = []

class PyEnum():
  def __init__(self):
    self.name: str = ''
    self.values: List[str] = []

class PyClass():
  def __init__(self):
    self.name: str = ''
    self.superclass: str = ''
    self.properties: List[PyProperty] = []
    self.fields: List[PyField] = []
    self.methods: List[PyMethod] = []
    self.static_creation_methods: List[PyStaticCreation] = []
    self.constructors: List[PyConstructor] = []
    self.enums: List[PyEnum] = []
    self.has_static_new: bool = False
    self.uses_shared_pointer: bool = False
    self.is_filter: bool = False
    self.is_custom: bool = False

  def __repr__(self):
    return self.name

  def generate_forward_declaration(self) -> str:
    template_param = self.name
    instance = f'instance{self.name}'
    if self.superclass:
      template_param += f', {self.superclass}'
    if self.uses_shared_pointer:
      template_param += f', std::shared_ptr<{self.name}>'
    code = f'py::class_<{template_param}> {instance}(mod, \"{self.name}\");\n'

    for enum in self.enums:
      enum_instance = f'instance{self.name}{enum.name}'
      code += f'py::enum_<{self.name}::{enum.name}> {enum_instance}({instance}, \"{enum.name}\");\n'

    return code

  def generate_bindings(self, foward_declared: bool=False) -> str:
    template_param = self.name
    if self.superclass:
      template_param += f', {self.superclass}'
    if self.uses_shared_pointer:
      template_param += f', std::shared_ptr<{self.name}>'

    needs_var: bool = self.constructors or self.has_static_new or self.static_creation_methods or self.properties or self.methods or self.fields

    instance = f'instance{self.name}'
    code = ''

    if not foward_declared:
      if self.enums or self.is_custom:
        
        code += f'py::class_<{template_param}> {instance}(mod, \"{self.name}\");\n'

        if needs_var:
          code += f'{instance}\n'
      else:
        code = f'py::class_<{template_param}>(mod, \"{self.name}\")'
        if not needs_var:
          code += ';'
        code += '\n'
    else:
      if needs_var:
        code += f'{instance}\n'

    for constructor in self.constructors:
      args = ', '.join(constructor.args)
      code += f'  .def(py::init<{args}>())\n'

    if self.has_static_new:
      code += f'  .def(py::init([]()\n    {{ \n      return {self.name}::New();\n    }}))\n'
      code += f'  .def_static(\"New\", &{self.name}::New)\n'

    for static_new_method in self.static_creation_methods:
      if static_new_method.args:
        args = ', '.join([f'{arg} var_{i}' for i, arg in enumerate(static_new_method.args)])
        values = ', '.join([f'var_{i}' for i, arg in enumerate(static_new_method.args)])

        code += f'  .def(py::init([]({args}) {{\n      return {self.name}::{static_new_method.name}({values});\n    }}))\n'
      else:
        code += f'  .def(py::init(&{self.name}::{static_new_method.name}))\n'
      if not static_new_method.is_overload:
        code += f'  .def_static(\"{static_new_method.name}\", &{self.name}::{static_new_method.name})\n'

    for prop in self.properties:
      if prop.read and prop.write:
        if prop.is_const_overload:
          code += f'  .def_property(\"{prop.name}\", py::overload_cast<>(&{self.name}::{prop.read}, py::const_), &{self.name}::{prop.write})\n'
        else:
          code += f'  .def_property(\"{prop.name}\", &{self.name}::{prop.read}, &{self.name}::{prop.write})\n'
      elif prop.read:
        code += f'  .def_property_readonly(\"{prop.name}\", &{self.name}::{prop.read})\n'
      elif prop.write:
        code += f'  .def(\"set{prop.name}\", &{self.name}::set{prop.name}, \"{prop.name}\"_a)\n'
    
    for field in self.fields:
      code += f'  .def_readwrite(\"{field.py_name}\", &{self.name}::{field.cpp_name})\n'

    for method in self.methods:
      if method.is_overload:
        template_param = ', '.join(method.arg_types)
        code += f'  .def(\"{method.name}\", py::overload_cast<{template_param}>(&{self.name}::{method.name}'
        if method.is_const:
          code += ', py::const_'
        code += ')'
      else:
        code += f'  .def(\"{method.name}\", &{self.name}::{method.name}'
      if method.return_value_policy:
        code += f', {method.return_value_policy}'
      if method.args:
        args = ', '.join([f'\"{arg}\"_a' for arg in method.args])
        code += f', {args}'
      code += ')\n'

    if needs_var:
      code += ';\n\n'
    else:
      code += '\n'

    for enum in self.enums:
      enum_instance = f'instance{self.name}{enum.name}'
      if not foward_declared:
        code += f'py::enum_<{self.name}::{enum.name}>({instance}, \"{enum.name}\")\n'
      else:
        code += f'{enum_instance}\n'
      for value in enum.values:
        code += f'  .value(\"{value}\", {self.name}::{enum.name}::{value})\n'
      code += ';\n\n'

    return code

  def generate_python_interface(self, module_name: str) -> str:
    arg_list = [replace_python_keywords(camel_to_snake(prop.name)) for prop in self.properties]
    if OBSERVER_ARG_NAME in arg_list:
      raise RuntimeError(f'A filter parameter in \"{self.name}\" is called \"{OBSERVER_ARG_NAME}\" which conflicts with the Pythonic interface')
    parameter_list = [f'{prop} = None' for prop in arg_list]
    args_text = ', '.join(parameter_list)
    if args_text:
      args_text = ', ' + args_text
    code = f'def {camel_to_snake(self.name)}(data_container_array{args_text}, observer = None) -> int:\n'
    code += '  \"\"\"\n'
    code += f'  {self.name}\n\n'
    code += '  :param data_container_array | DataContainerArray\n'
    for prop, arg in zip(self.properties, arg_list):
      code += f'  :param {arg} | {prop.type} {prop.name}\n'
    code += '  :param observer | Observer\n'
    code += '  \"\"\"\n\n'
    code += f'  filter = {module_name}.{self.name}()\n'
    code += f'  filter.setDataContainerArray(data_container_array)\n'
    code += '  if observer is not None:\n'
    code += '    filter.connectObserver(observer)\n'

    for prop, arg in zip(self.properties, arg_list):
      code += f'  if {arg} is not None:\n'
      code += f'    filter.{prop.name} = {arg}\n'

    code += '  filter.execute()\n'
    code += '  return filter.ErrorCode\n\n'
    return code

  def generate_unit_test_snippet(self, module_name: str) -> str:
    code = f'  print(\'# --- {self.name}\')\n'
    code += f'  filter = {module_name}.{self.name}()\n'
    code += f'  filter.preflight()\n'
    code += f'  assert filter.NameOfClass == \'{self.name}\', f\'Error: Filter class name is not correct. {{filter.NameOfClass}} != {self.name}\'\n\n'
    return code

def replace_python_keywords(text: str) -> str:
  if text == 'lambda':
    return 'lambda_value'
  return text

def camel_to_snake(text: str) -> str:
  result = re.sub(r'(.)([A-Z][a-z]+)', r'\1_\2', text)
  return re.sub(r'([a-z0-9])([A-Z])', r'\1_\2', result).lower()

def is_child_path(child_path: str, parent_path: str) -> bool:
  return os.path.commonpath([os.path.abspath(parent_path)]) == os.path.commonpath([os.path.abspath(parent_path), os.path.abspath(child_path)])

def find_enum_values(file_path: str, name: str) -> List[str]:
  enum_values = []

  with open(file_path, 'r') as file:
    line: str
    for line in file:
      line = line.strip()
      if f'enum class {name}' in line:
        while '{' not in line:
          line = next(file).strip()
        line = next(file).strip()
        while '};' not in line:
          value = line.strip(',')
          enum_values.append(value.split(' ')[0])
          line = next(file).strip()
        break
 
  return enum_values

def find_index(lst: list, value) -> Optional[int]:
  if value in lst:
    return lst.index(value)
  else:
    return None

def extract_macro_args(line: str) -> List[str]:
  match = re.search(r'\((.*?)\)', line)
  if not match:
    raise RuntimeError('No match found')
  return list(filter(bool, match.group(1).split(' ')))

def file_line_generator(file_path: str) -> Generator[str, None, None]:
  with open(file_path, 'r') as file:
    line: str
    for line in file:
      yield line

def parse_class(line: str) -> PyClass:
  tokens = extract_macro_args(line)
  py_class = PyClass()
  py_class.name = tokens.pop(0)
  while tokens:
    token = tokens.pop(0)
    if token == PYB11_SUPERCLASS:
      py_class.superclass = tokens.pop(0)
  return py_class

def parse_property(line: str) -> PyProperty:
  tokens = extract_macro_args(line)
  prop = PyProperty()
  prop.type = tokens.pop(0)
  prop.name = tokens.pop(0)
  while tokens:
    token = tokens.pop(0)
    if token == PYB11_READ:
      prop.read = tokens.pop(0)
    elif token == PYB11_WRITE:
      prop.write = tokens.pop(0)
    elif token == PYB11_CONST_GET_OVERLOAD:
      prop.is_const_overload = True
  return prop

def parse_method_arg(arg: str) -> Tuple[str, str]:
  arg_values = arg.split(',')
  arg_name = arg_values[1]
  arg_type = arg_values[0]
  arg_type = arg_type.replace('.', ' ')
  return (arg_name, arg_type)

def parse_method(line: str) -> PyMethod:
  tokens = extract_macro_args(line)
  method = PyMethod()
  method.return_type = tokens.pop(0)
  method.name = tokens.pop(0)

  if tokens:
    if tokens[-1] == PYB11_CONST_METHOD:
      method.is_const = True
      tokens.pop()

    index = find_index(tokens, PYB11_RETURN_VALUE_POLICY)

    if index is not None:
      method.return_value_policy = tokens.pop(index + 1)
      tokens.pop(index)

    token = tokens.pop(0)
    if token == PYB11_ARGS:
      while tokens:
        arg = tokens.pop(0)
        if ',' in arg:
          arg_name, arg_type = parse_method_arg(arg)
          method.args.append(arg_name)
          method.arg_types.append(arg_type)
        else:
          method.args.append(arg)
    elif token == PYB11_OVERLOAD:
      method.is_overload = True
      while tokens:
        overload_arg = tokens.pop(0)
        arg_name, arg_type = parse_method_arg(overload_arg)
        method.args.append(arg_name)
        method.arg_types.append(arg_type)

  return method

def parse_static_creation(line: str) -> PyStaticCreation:
  tokens = extract_macro_args(line)
  py_static_creation = PyStaticCreation()
  py_static_creation.name = tokens.pop(0)
  if tokens:
    token = tokens.pop(0)
    if token == PYB11_ARGS:
      while tokens:
        py_static_creation.args.append(tokens.pop(0))
    elif token == PYB11_OVERLOAD:
      py_static_creation.is_overload = True
      while tokens:
        py_static_creation.args.append(tokens.pop(0))
  return py_static_creation

def parse_creation(line: str) -> PyConstructor:
  tokens = extract_macro_args(line)
  constructor = PyConstructor()
  tokens = [item.replace('.', ' ') for item in tokens]
  constructor.args = tokens
  return constructor

def parse_enumeration(line: str) -> PyEnum:
  tokens = extract_macro_args(line)
  enum = PyEnum()
  enum.name = tokens.pop(0)
  return enum

def parse_static_new_macro(line: str) -> bool:
  return True

def parse_filter_new_macro(line: str) -> bool:
  return True

def parse_shared_pointers(line: str) -> bool:
  return True

def parse_filter(line: str) -> bool:
  return True

def parse_custom(line: str) -> bool:
  return True

def parse_field(line: str) -> PyField:
  tokens = extract_macro_args(line)
  field = PyField()
  size = len(tokens)
  if size == 2:
    field.cpp_name = tokens[0]
    field.py_name = tokens[1]
  elif size == 1:
    field.cpp_name = tokens[0]
    field.py_name = tokens[0]
  else:
    raise RuntimeError('incorrect number of arguments for PyField')
  return field

def parse_bindings(class_line: str, file: Generator[str, None, None]) -> PyClass:
  py_class = parse_class(class_line)
  line: str
  for line in file:
    line = line.strip()
    if line.startswith(PYB11_END_BINDINGS):
      break
    elif line.startswith(PYB11_PROPERTY):
      py_class.properties.append(parse_property(line))
    elif line.startswith(PYB11_METHOD):
      py_class.methods.append(parse_method(line))
    elif line.startswith(PYB11_STATIC_CREATION):
      py_class.static_creation_methods.append(parse_static_creation(line))
    elif line.startswith(PYB11_CREATION):
      py_class.constructors.append(parse_creation(line))
    elif line.startswith(PYB11_ENUMERATION):
      py_class.enums.append(parse_enumeration(line))
    elif line.startswith(PYB11_STATIC_NEW_MACRO):
      py_class.has_static_new = parse_static_new_macro(line)
    elif line.startswith(PYB11_FILTER_NEW_MACRO):
      py_class.has_static_new = parse_filter_new_macro(line)
    elif line.startswith(PYB11_SHARED_POINTERS):
      py_class.uses_shared_pointer = parse_shared_pointers(line)
    elif line.startswith(PYB11_FILTER):
      py_class.is_filter = parse_filter(line)
    elif line.startswith(PYB11_CUSTOM):
      py_class.is_custom = parse_custom(line)
    elif line.startswith(PYB11_FIELD):
      py_class.fields.append(parse_field(line))
  return py_class

def parse_file(file_path: str) -> List[PyClass]:
  file_gen = file_line_generator(file_path)
  classes = []
  line: str
  for line in file_gen:
    line = line.strip()
    if line.startswith(PYB11_BEGIN_BINDINGS):
      py_class = parse_bindings(line, file_gen)
      for enum in py_class.enums:
        enum.values = find_enum_values(file_path, enum.name)
      classes.append(py_class)
  return classes

def dfs(graph: DefaultDict[str, List[str]], vertex: str) -> List[str]:
  visited = {key : False for key in graph.keys()}
  def dfs_impl(graph: DefaultDict[str, List[str]], vertex: str, visited: Dict[str, bool]) -> List[str]:
    visited[vertex] = True
    path = [vertex]

    for item in graph[vertex]:
      if item not in graph:
        path.append(item)
      elif not visited[item]:
        path.extend(dfs_impl(graph, item, visited))
    return path
  return dfs_impl(graph, vertex, visited)

def is_sublist(parent: list, child: list) -> bool:
  it = iter(parent)
  return all(any(parent_item == child_item for parent_item in it) for child_item in child)

def remove_sublists(lists: list) -> list:
  lists.sort(key=lambda s: len(s))
  out = []
  n = len(lists)
  for i, item in enumerate(lists):
    is_sub = False
    for j in range(i + 1, n - 1):
      is_sub = is_sublist(lists[j], item)
      if is_sub:
        break
    if not is_sub:
      out.append(item)
  return out

def sort_inherited_classes(classes: List[PyClass]) -> List[PyClass]:
  # assumes single inheritance
  class_lookup = {item.name : item for item in classes}
  superclasses = {item.superclass for item in classes}
  class_names = {item.name for item in classes}

  intersection = superclasses.intersection(class_names)

  base_classes = []

  for item in classes:
    if item.superclass not in intersection:
      base_classes.append(item.name)

  graph: DefaultDict[str, List[str]] = defaultdict(list)

  for py_class in classes:
    graph[py_class.superclass].append(py_class.name)

  sorted_classes = []

  for name in base_classes:
    sorted_classes.append(dfs(graph, name))

  sorted_classes = remove_sublists(sorted_classes)

  sorted_classes = [item for items in sorted_classes for item in items]

  result = []

  for name in sorted_classes:
    if name in class_lookup:
      result.append(class_lookup[name])

  if Counter(classes) != Counter(result):
    raise RuntimeError('sorted results must contain same elements as input')

  return result

def replace_file_if_different(temp_file: str, target_file: str) -> None:
  target_file_path = Path(target_file)
  
  if target_file_path.exists():
    if filecmp.cmp(temp_file, target_file, shallow=False):
      os.remove(temp_file)
    else:
      os.remove(target_file)
      os.rename(temp_file, target_file)
  else:
    os.rename(temp_file, target_file)

def generate_simpl_bindings(output_dir: str, header_path: str, body_path: str, files: List[str], source_dir: str, python_output_dir: str, plugin_name: str, body_top_path: str = '', post_types_path: str = '', no_tests: bool = False, relative_imports: bool = False) -> None:
  module_target_file_path = f'{output_dir}/py_simpl.cpp'
  module_temp_file_path = f'{module_target_file_path}.temp'
  with open(module_temp_file_path, 'w') as module_file, open(f'{python_output_dir}/simplpy.py', 'w') as python_file, ExitStack() as exit_stack:
    if not no_tests:
      test_file = exit_stack.enter_context(open(f'{python_output_dir}/simpl_UnitTest.py', 'w'))
    with open(header_path, 'r') as header_file:
      header_code = header_file.read()
      module_file.write(f'// This file is automatically generated during build time by {__file__}\n\n')
      module_file.write(header_code)

    module_file.write('\n')

    for file_path in files:
      file_path = Path(file_path).relative_to(source_dir).as_posix()
      module_file.write(f'#include \"{file_path}\"\n')

    module_file.write('\nPYBIND11_MODULE(simpl, mod)\n{\n')

    if body_top_path:
      with open(body_top_path, 'r') as body_top_file:
        custom_body_top_code = body_top_file.read()
        module_file.write(f'{custom_body_top_code}\n')

    module_classes = []

    for file_path in files:
      classes = parse_file(file_path)
      module_classes.extend(classes)

    module_classes = sort_inherited_classes(module_classes)

    if not relative_imports:
      python_file.write(f'import simpl\n\n')
    else:
      python_file.write(f'from . import simpl\n\n')

    if not no_tests:
      test_file.write(f'import simpl\n\n')
      test_file.write(f'def simplUnitTest():\n')

    for py_class in module_classes:
      code = py_class.generate_forward_declaration()
      module_file.write(code)

    if post_types_path:
      with open(post_types_path, 'r') as post_types_file:
        custom_post_types_code = post_types_file.read()
        module_file.write(f'{custom_post_types_code}\n')

    for py_class in module_classes:
      code = py_class.generate_bindings(foward_declared=True)
      module_file.write(code)
      if py_class.is_filter:
        python_code = py_class.generate_python_interface('simpl')
        python_file.write(python_code)
        if not no_tests:
          test_code = py_class.generate_unit_test_snippet('simpl')
          test_file.write(test_code)

    if not no_tests:
      test_file.write('\n')
      test_file.write('if __name__ == \'__main__\':\n')
      test_file.write('  print(\'simpl UnitTest Starting\')\n')
      test_file.write('  simplUnitTest()\n')
      test_file.write('  print(\'simpl UnitTest Complete\')\n')

    with open(body_path, 'r') as body_file:
      body_code = body_file.read()
      module_file.write(body_code)

    module_file.write('}\n')
  replace_file_if_different(module_temp_file_path, module_target_file_path)

def generate_plugin_bindings(output_dir: str, module_name: str, files: List[str], include_dir: Optional[str], python_output_dir: str, plugin_name: str, header_path: str = '', body_path: str = '', body_top_path: str = '', post_types_path: str = '', no_tests: bool = False, relative_imports: bool = False) -> None:
  module_target_file_path = f'{output_dir}/py_{module_name}.cpp'
  module_temp_file_path = f'{module_target_file_path}.temp'
  with open(module_temp_file_path, 'w') as module_file, open(f'{python_output_dir}/{module_name}py.py', 'w') as python_file, ExitStack() as exit_stack:
    if not no_tests:
      test_file = exit_stack.enter_context(open(f'{python_output_dir}/{module_name}_UnitTest.py', 'w'))
    includes = files
    if include_dir:
      includes = []
      for file_path in files:
        path = Path(file_path)
        include_path = Path(include_dir)
        path = path.relative_to(include_path)
        path = include_path.parts[-1] / path
        includes.append(path.as_posix())

    include_code = '\n'.join([f'#include \"{file_path}\"' for file_path in includes])

    custom_header_code = ''
    if header_path:
      with open(header_path, 'r') as header_file:
        custom_header_code = header_file.read()

    custom_body_code = ''
    if body_path:
      with open(body_path, 'r') as body_file:
        custom_body_code = body_file.read()

    relative_import_code = 'simpl'
    if relative_imports:
      relative_import_code = 'dream3d.simpl'

    header_code = (
      f'// This file is automatically generated during build time by {__file__}\n\n'
      '#include <memory>\n\n'
      '#include <pybind11/pybind11.h>\n\n'
      '#include \"SIMPLib/Filtering/FilterManager.h\"\n\n'
      f'#include \"{plugin_name}/{plugin_name}Plugin.h\"\n\n'
      '#include \"Binding/Pybind11CustomTypeCasts.h\"\n\n'
      f'{custom_header_code}\n'
      f'{include_code}\n\n'
      'namespace py = pybind11;\n'
      'using namespace py::literals;\n\n'
      f'PYBIND11_MODULE({module_name}, mod)\n'
      '{\n'
      f'py::module::import("{relative_import_code}");\n\n'
    )
    module_file.write(header_code)

    if body_top_path:
      with open(body_top_path, 'r') as body_top_file:
        custom_body_top_code = body_top_file.read()
        module_file.write(f'{custom_body_top_code}\n')

    module_classes = []

    for file_path in files:
      classes = parse_file(file_path)
      module_classes.extend(classes)

    module_classes = sort_inherited_classes(module_classes)

    if not relative_imports:
      python_file.write(f'import {module_name}\n\n')
    else:
      python_file.write(f'from . import {module_name}\n\n')

    if not no_tests:
      test_file.write(f'import {module_name}\n\n')
      test_file.write(f'def {module_name}UnitTest():\n')

    for py_class in module_classes:
      code = py_class.generate_forward_declaration()
      module_file.write(code)

    if post_types_path:
      with open(post_types_path, 'r') as post_types_file:
        custom_post_types_code = post_types_file.read()
        module_file.write(f'{custom_post_types_code}\n')

    for py_class in module_classes:
      code = py_class.generate_bindings(foward_declared=True)
      module_file.write(code)
      if py_class.is_filter:
        python_code = py_class.generate_python_interface(module_name)
        python_file.write(python_code)
        if not no_tests:
          test_code = py_class.generate_unit_test_snippet(module_name)
          test_file.write(test_code)

    if not no_tests:
      test_file.write('\n')
      test_file.write('if __name__ == \'__main__\':\n')
      test_file.write(f'  print(\'{module_name} UnitTest Starting\')\n')
      test_file.write(f'  {module_name}UnitTest()\n')
      test_file.write(f'  print(\'{module_name} UnitTest Complete\')\n')

    module_file.write(f'{custom_body_code}\n')
    
    module_file.write(f'  //Ensure all the filters are loaded when this module loads\n')
    module_file.write('  py::module_ os = py::module_::import("os");\n')
    module_file.write('  auto env_value = os.attr(\"getenv\")(\"DREAM3D_PLUGINS_LOADED\");\n')
    module_file.write('  if(env_value.is_none())\n')
    module_file.write('  {\n')
    module_file.write(f'    {plugin_name}Plugin {module_name}Plugin;\n')
    module_file.write(f'    {module_name}Plugin.registerFilters(FilterManager::Instance());\n')
    module_file.write('  }\n')
    module_file.write('\n}\n')
  replace_file_if_different(module_temp_file_path, module_target_file_path)

def read_plugin_file_list(file_list_path: str, source_dir: str) -> List[str]:
  files = []
  with open(file_list_path, 'r') as file:
    lines = file.read().splitlines()
    for line in lines:
      if line.startswith('#'):
        continue
      path = line.strip()
      path = f'{source_dir}/{line}'
      files.append(path)
  return files

def read_simpl_file_list(file_list_path: str, source_dir: str) -> List[str]:
  files = []
  with open(file_list_path, 'r') as file:
    lines = file.read().split(';')
    for line in lines:
      path = line.strip()
      if '.h' in path and is_child_path(path, source_dir):
        with open(path, 'r') as file:
          data = file.read()
          if re.search(PYB11_BEGIN_BINDINGS, data) and not re.search(f'#define {PYB11_BEGIN_BINDINGS}', data):
            files.append(path)
  files = list(dict.fromkeys(files))
  return files

if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  parser.add_argument('output_dir')
  parser.add_argument('file_list_path')
  parser.add_argument('source_dir')
  parser.add_argument('python_output_dir')
  parser.add_argument('--module_name')
  parser.add_argument('--header_path')
  parser.add_argument('--body_path')
  parser.add_argument('--body_top_path')
  parser.add_argument('--post_types_path')
  parser.add_argument('--include_dir')
  parser.add_argument('--plugin_name')
  parser.add_argument('--plugin', action='store_true')
  parser.add_argument('--no_tests', action='store_true')
  parser.add_argument('--relative_imports', action='store_true')

  args = parser.parse_args()

  if args.plugin:
    if args.module_name is None:
      parser.error('--plugin requires --module_name')
    files = read_plugin_file_list(args.file_list_path, args.source_dir)
    generate_plugin_bindings(args.output_dir, args.module_name, files, args.include_dir, args.python_output_dir, args.plugin_name, args.header_path, args.body_path, args.body_top_path, args.post_types_path, args.no_tests, args.relative_imports)
  else:
    if args.header_path is None:
      parser.error('requires --header_path')
    if args.body_path is None:
      parser.error('requires --body_path')
    files = read_simpl_file_list(args.file_list_path, args.source_dir)
    generate_simpl_bindings(args.output_dir, args.header_path, args.body_path, files, args.source_dir, args.python_output_dir, args.plugin_name, args.body_top_path, args.post_types_path, args.no_tests, args.relative_imports)
