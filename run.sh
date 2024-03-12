#!/bin/bash

# Terminal 1
lxterminal -e "bash -c 'cd ./RaspberryPi/l_export_host_positioning/app/bluetooth/example_host/bt_aoa_host_locator; ./exe/bt_aoa_host_locator -u /dev/ttyACM0; exec bash'"

# Terminal 2
lxterminal -e "bash -c 'cd ./RaspberryPi/p_export_host_positioning/app/bluetooth/example_host/bt_host_positioning; ./exe/bt_host_positioning -c ./config/positioning_config.json; exec bash'"

# Terminal 3
lxterminal -e "bash -c 'cd ./RaspberryPi/p_export_host_positioning/app/bluetooth/example_host/bt_host_positioning_gui; python3 main.py; exec bash'"
