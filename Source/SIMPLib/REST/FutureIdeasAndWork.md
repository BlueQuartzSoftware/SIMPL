# Future Ideas and Work Additions #

+ Allow user to upload files to the server to be processed.
    + How do accomplish this? 
    + Only accept .zip or .tar.gz archives? 
    + Allow folder from client side and upload individually?
+ User Login and user Session tracking
    + Keep files (input and output) for a user rather than session?
+ Adjust ExecutePipeline EndPoint/API to allow user to archive a directory
  + Based on work to add the ArchveToolbox plugin possibly
+ Allow users to name their log files

## DREAM.3D Additions ##

+ Add a menu command in DREAM.3D to send the current pipeline to the server
+ Add a plugin for zipping files and directories (zlib + minizip)?


## Expanding the API ##

+ Thread the execution of the pipeline to return immediately
+ Allow polling of a running pipeline
+ **Really Advanced**  Use a WebSocket to send the Standard Output back to the client so the user knows real time how their pipeline is proceeding.




## User Interface ##

+ Create a user interface (Qt5) that incorporates all possible commands and requests
+ Create a web front end to run pipelines
    + Use meta data from FilterParameter classes to autogenerate the web front end codes
         + HTML and/or JavaScript

## Use Cases ##

Allow a user to build up a pipeline from within a client (Web based)

  + Requires holding state on the server side
  + New APIs
    + CreatePipeline
    + AddFilter
    + RemoveFilter
  + Or just do all that on the Client side, then ship the pipeline to the server for preflight and execution

## Globus Integration ##

+ Could serve the Doc Root from a Globus shared instance allowing users to transfer their data through Globus.
+ Create a Plugin/Toolbox that integrates Globus authentication and transfer.

