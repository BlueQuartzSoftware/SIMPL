## Creating a DREAM3D Filter in Python

**1. Setup Instructions for Anaconda:** [https://github.com/BlueQuartzSoftware/dream3d-conda-feedstock/blob/main/dream3d_conda_instructions.md](https://github.com/BlueQuartzSoftware/dream3d-conda-feedstock/blob/main/dream3d_conda_instructions.md)
**2. Imports Needed in Script:**

    from dream3d.Filter import Filter, FilterDelegatePy
    from dream3d.simpl import *

**3. Filter Class General Format:**

    ```
    class FilterName(Filter)
    	def __init__(self) -> None:
        self.var: var_data_type = initial_value
    		#declare and initialize all variables

    	def _set_var(self, value: var_data_type) -> None:
    		self.var = value

    	def _get_var(self) -> var_data_type:
    		return self.var

    	#add more getter/setter functions as needed

      @staticmethod
      def name() -> str:
          return 'Name of Filter'

      @staticmethod
      def uuid() -> str:
          return '{uuid#}'

      @staticmethod
      def group_name() -> str:
          return 'Example'

      @staticmethod
      def sub_group_name() -> str:
          return 'Sub Example'

      @staticmethod
      def human_label() -> str:
        	return 'Label Name for Filter'

      @staticmethod
      def version() -> str:
        	return '1.0.0'

      @staticmethod
      def compiled_lib_name() -> str:
        	return 'Python'

      def setup_parameters(self) -> List[FilterParameter]:
    		#put requirements here
    		return[
    			#list of filter parameters here
    		]

    def data_check(self, dca: DataContainerArray, status_delegate: Union[FilterDelegateCpp, FilterDelegatePy] = FilterDelegatePy()) -> Tuple[int, str]:
    		#tests for correct use of filter here

    def _execute_impl(self, dca: DataContainerArray, status_delegate: Union[FilterDelegateCpp, FilterDelegatePy] = FilterDelegatePy()) -> Tuple[int, str]:
    		#code that does transformation of data here
    		return (0, 'Success')

    filters = [NameOfFilter]

    ```



**4. init**

  - All variables used for the filter parameters are declared and initialized here.
  - An example variable declaration:

      `self.new_string: str = ''`


**5. uuid**
  - uuid() is a unique ID for the filter.
  - To get a uuid(), run the following in Anaconda:

      `import uuid`

      `uuid.uuid4()`

  - Copy the uuid and paste into the return value for the function.

**6. human_label**
  - This is the name of the filter that will show up when running DREAM3D.

**7. setup_parameters**
  - This function returns a list of your filter parameters.
  - There is separate documentation for the filter parameters in the SIMPL/Wrapping/Python/Docs folder.
  - Before the return statement, you can add any requirements that are necessary for the parameters. Requirements are stated in the filter parameter documentation.

**8. data_check**
  - This function runs checks on the user data inputted into the filter. For example, you may want to check if the user has entered a name for a new array, if a data container and/or attribute matrix exists, if the user has selected an input file, etc.
  - An example if statement data check:


        if da is None:
              return (-2, 'DataArray is None')

  - An if statement should always return a negative code along with an error message.

**9. _execute_impl**
  - This is where you write the code that does the actual transformation of the data.

**10. Running Filter in D3D**
  - Open DREAM3D in Anaconda with instructions from step 1
  - Hit CTRL + r to load filters into D3D.

**11. Markdown Documentation of Filter**
  - Follow format from this file: https://github.com/BlueQuartzSoftware/SIMPLView/blob/develop/Source/DevHelper/Template/Documentation/Filter/Documentation.md.in
