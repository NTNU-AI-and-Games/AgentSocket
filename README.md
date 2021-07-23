# AgentSocket

AgentSocket is a plugin for Unreal Engine, enabling remote input and observation for AI agents.
The plugin features remote input with actions names (corresponding to input action mappings), keys and mouse. 
Being made with Reinforcement Learning support in mind, the plugin will perform the actions and return a step response consisting of viewport screenshot and json payload of states and reward.

The plugin is under development, and may not function properly.

Supports Unreal version 4.26 (may also support other versions)

## Getting Started
### Install
1. Clone this repo into `./Plugins/` of your Unreal project
2. In Unreal Editor, go to `Edit > Plugins` find `AgentSocket` in the list, and enable the plugin

### Use the AgentSocketComponent
1. Create a custom PlayerController blueprint class
2. Open the PlayController and `Add Component > AgentSocket`
3. Customizations can be done in the Details panel for the AgentSocket.
4. Create a custom GameModeBase blueprint class, Assign the custom PlayerController to custom GameModeBase
5. In world settings of the current map, override the GameMode with our custom GameModeBase

### Connecting from remote agent
 - The AgentSocket plugin will host a server at **tcp port 11111** by default.
 - Send a Json message with the following format (this make the character walk backwards and jump):
   ```
   { 
     "type": "action",
     "actions": [
        { "type": "key", "key": "s" },
        { "type": "action", "name", "jump" }
     ]
   }
   ```
 - Receive the step responses (there are __json response__ and __png image response__) from the same port.
 - If the response has `\x89PNG` in the first 4 bytes, then it must be an image


## JSON Step Response
The Json responses are formatted as follows:
```
{
  "Type": "STEP",
  "State": {
    "GameOver": <true | false>,
    "GameReset" <true | false>,
    "Other": [
      <string key>: <string value>, ...
    ]
  },
  "Reward": <number>
}
```



## PNG Response
The PNG responses are bytes messages sent through the tcp protocol. Formatted according to the standards.
The responses can be considered as the raw bytes of a PNG file.


## Feature list
 - Add rewards using blueprints.
 - Set game over or game reset state using blueprints
 - Image stream settings can be set in the details panel of AgentSocket. For best performance, use grayscale setting.
 - Batch actions
 - Video capture of the unreal engine client window


---
## Known Issues
 - JSON step responses return corrupted messages in the beginning 
 - The AgentSocketComponent must be attached to a PlayerController
 - Image scaling is currently NOT supported
 - Resizing window will not resize the image stream
 - Image stream is of raw images. Further optimizations may be implemented later
 - Does not support Split Screen of Unreal Engine properly
 
