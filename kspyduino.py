#
# Copyright 2017 Sam Lewis
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import krpc

from argparse import ArgumentParser
from serial import Serial


def handle_args():
    parser = ArgumentParser(description='kspyduino')

    parser.add_argument('-p', '--port', type=str, dest='port', help='Arduino COM port', required=True)

    return parser.parse_args()


if __name__ == '__main__':
    args = handle_args()

    serial = Serial(
        port=args.port,
        baudrate=9600,
        timeout=0.01
    )

    conn = krpc.connect(name='kspyduino')

    while True:
        try:
            vessel = conn.space_center.active_vessel
            serial.write(b'get throttle\n')
            while True:
                line = str(serial.readline(), encoding='utf-8').strip()
                if line != '':
                    print(line)
                    command, control, value = line.split()
                    if (command == 'set'):
                        if (control == 'throttle'):
                            vessel.control.throttle = float(value)
                        elif (control == 'stage' and value == 'true'):
                            vessel.control.activate_next_stage()
                        elif (control == 'lights'):
                            vessel.control.lights = value == 'true'

        except Exception as e:
            print(e)
