Copy Object 
=============

## Group (Subgroup) ##

DREAM3D Review (Registration)

## Description ##

This **Filter** copies a **Data Container**, **Attribute Matrix**, or **Attribute Array**.  **Attribute Matrix** copies will be located in the same **Data Container** as the original, while **Attribute Array** copies will be located in the same **Attribute Matrix** as the original.  

If a **Data Container** is copied, then the new **Data Container** will contain copies of the original **Data Container's** **Geometry**, **Attribute Matrices**, and **Attribute Arrays**. Similarly, if an **Attribute Matrix** is copied, then the new **Attribute Matrix** will contain copies of the the original **Attribute Matrix's** **Attribute Arrays**.  Thus, the copy flows down the data structure hierarchy.

## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| Object Type to Copy | Enumeration | Whether to copy a **Data Container**, **Attribute Matrix**, or **Attribute Array** |

## Required Geometry ###

None

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Data Container** | None | N/A | N/A | **Data Container** to copy, if _Data Container_ is selected for _Object Type to Copy_ |
| Any **Attribute Matrix** | None | Any | N/A | **Attribute Matrix** to copy, if _Attribute Matrix_ is selected for _Object Type to Copy_ |
| Any **Attribute Array** | None | Any | Any | **Attribute Array** to copy, if _Attribute Array_ is selected for _Object Type to Copy_ |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Data Container** | None | N/A | N/A | Copied **Data Container** name, if _Data Container_ is selected for _Object Type to Copy_ |
| Any **Attribute Matrix** | None | Any | N/A | Copied **Attribute Matrix** name, if _Attribute Matrix_ is selected for _Object Type to Copy_ |
| Any **Attribute Array** | None | Any | Any | Copied **Attribute Array** name, if _Attribute Array_ is selected for _Object Type to Copy_ |

## Example Pipelines ##



## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
