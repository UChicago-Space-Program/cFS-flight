# Radio Manager App

Radio Manager is responsible for managing the state and configuration of the radio. The app communicates with the CSP CAN App via Software Bus to configure the radio hardware. Radio Manager maintains dual counters (cached and radio hardware) to detect communication issues between the OBC and radio.

## Architecture

Radio Manager App manages radio state and configuration. It communicates with the CSP CAN App (not directly with CSP) via Software Bus messages to configure the radio hardware.

**Data Flow:**
- Ground/Other Apps → Radio Manager App (SB Commands)
- Radio Manager App → CSP CAN App (SB Messages with radio config)
- CSP CAN App → Radio Hardware (CSP/CAN)

## Features

- Radio state management with dual counter system (cached + radio hardware)
- Counter discrepancy detection to identify OBC-radio communication issues
- Software Bus command interface for configuration and control
- Telemetry publishing with radio state information

