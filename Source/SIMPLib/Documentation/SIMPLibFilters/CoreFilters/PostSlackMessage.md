Post Slack Message 
=============

## Group (Subgroup) ##

Core Filters, Misc

## Description ##

This filter will post a message to the Slack.com channel of their choice. The user will need to know the web address for their particular channel. Start by adding a [Slack WebHook](https://slack.com/apps/A0F7XDUAZ-incoming-webhooks) to your Slack domain. Once the https web address is generated, that web address can be used in the filter. The message can be formatted using basic MarkDown syntax. 

**Slack attachments are not yet supported.**

## Parameters ##

| Name             | Type | Description |
|------------------|------| ------------|
| Slack UserName | String | The user the message will be posted as. |
| Slack URL | String | The Slack WebHook web address |
| Slack Message | String | The message that will be sent |
| Warnings as Error | bool | Treat warnings from the Slack server as pipeline errors |

## Required Geometry ##

None

## Required Objects ##

N/A

## Created Objects ##

N/A


## Example Pipelines ##



## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)

