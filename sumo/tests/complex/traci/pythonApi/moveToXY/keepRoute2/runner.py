#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    runner.py
@author  Jakob Erdmann
@date    2017-01-23
@version $Id: runner.py 24854 2017-06-22 13:21:09Z behrisch $


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from __future__ import print_function
from __future__ import absolute_import
import os
import subprocess
import sys
import random
sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import traci
import sumolib  # noqa

sumoBinary = os.environ["SUMO_BINARY"]
PORT = sumolib.miscutils.getFreeSocketPort()
sumoProcess = subprocess.Popen([sumoBinary,
                                '-n', 'input_net.net.xml',
                                '--no-step-log',
                                '-S', '-Q',
                                '--remote-port', str(PORT)], stdout=sys.stdout)

ANGLE_UNDEF = traci.constants.INVALID_DOUBLE_VALUE
INVALID = traci.constants.INVALID_DOUBLE_VALUE

vehID = "v0"
def check(x, y, angle, exLane, exPos, exPosLat, comment):
    traci.vehicle.moveToXY(vehID, "", angle, x, y, keepRoute=2)
    traci.simulationStep()
    x2, y2 = traci.vehicle.getPosition(vehID)
    lane2 = traci.vehicle.getLaneID(vehID)
    pos2 = traci.vehicle.getLanePosition(vehID)
    posLat2 = traci.vehicle.getLateralLanePosition(vehID)
    if (       abs(x - x2) > 0.1 
            or abs(y - y2) > 0.1 
            or exLane != lane2 
            or (exPos is not None and abs(exPos - pos2) > 0.1)
            or (exPosLat is not None and abs(exPosLat - posLat2) > 0.1)
            ):
        print(comment, "failed: x=%s, x2=%s,   y=%s, y2=%s,   lane=%s, lane2=%s, pos=%s, pos2=%s   posLat=%s posLat2=%s" % (
            x, x2, y, y2, exLane, lane2, exPos, pos2, exPosLat, posLat2))
    else:
        #print(comment, "success")
        pass


traci.init(PORT)
traci.simulationStep()
traci.route.add("beg", ["beg"])
traci.vehicle.add(vehID, "beg")
check(-1,  0,   ANGLE_UNDEF, "",         None, None,       "1m before the start of the edge")
check(-5,  0,   ANGLE_UNDEF, "",         None, None,       "5m before the start of the edge")
check(198, 1.9, ANGLE_UNDEF, "middle_0", 98.0, 1.9,        "internal corner (inside, segment1)")
check(198, 1.9, 0,           "middle_0", 102,  2.0,        "internal corner (inside, segment2)")
check(201, -1, 0,            "middle_0", 100, -1.41,       "internal corner (outside, near)")
check(203, -4, 0,            "", INVALID, INVALID,         "internal corner (outside, far)")
traci.close()
sumoProcess.wait()
