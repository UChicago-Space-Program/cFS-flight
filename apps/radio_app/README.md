![Static Analysis](https://github.com/nasa/sample_app/workflows/Static%20Analysis/badge.svg)
![Format Check](https://github.com/nasa/sample_app/workflows/Format%20Check/badge.svg)

# Radio Manager App

Radio Manager is responsible for managing the state and configuration of the radio. However, data that pass through the radio is entirely controlled by the Comms App. In fact, this Radio Manager only sends its configurations to the Comms App.

## Use

## Architecture

## Known issues

It looks like we can use CSP to communicate with the radio directly, in which case it might be easier to alter our structure. Here are the following nodes:
1. Ground Station
2. OBC
3. Payload Computer
4. Radio
5. ...
There is a single COMMs app which is responsible for ALL communication through CSP. The radio manager app will be responsible for configurations and their updates, but not the actual, general data itself.