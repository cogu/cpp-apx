/*****************************************************************************
* \file      sample_nodes.c
* \author    Conny Gustafsson
* \date      2020-10-12
* \brief     Sample APX definitions for unit test purposes
*
* Copyright (c) 2020 Conny Gustafsson
* Permission is hereby granted, free of charge, to any person obtaining a copy of
* this software and associated documentation files (the "Software"), to deal in
* the Software without restriction, including without limitation the rights to
* use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
* the Software, and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:

* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.

* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
* FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
* IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
******************************************************************************/
#include "sample_nodes.h"

namespace apx
{
   const char* g_sample_node1 =
      "APX/1.2\n"
      "N\"TestNode1\"\n"
      "P\"WheelBasedVehicleSpeed\"S:=65535\n"
      "P\"CabTiltLockWarning\"C(0,7):=7\n"
      "P\"VehicleMode\"C(0,15):=15\n"
      "R\"GearSelectionMode\"C(0,7):=7\n";

   const char* g_sample_node2 =
      "APX/1.2\n"
      "N\"TestNode2\"\n"
      "P\"WheelBasedVehicleSpeed\"S:=65535\n"
      "P\"ParkBrakeAlert\"C(0,3):=3\n"
      "R\"VehicleMode\"C(0,15):=15\n";

   const char* g_sample_node3 =
      "APX/1.2\n"
      "N\"TestNode3\"\n"
      "R\"WheelBasedVehicleSpeed\"S:=65535\n";

   const char* g_sample_node4 =
      "APX/1.2\n"
      "N\"TestNode4\"\n"
      "R\"WheelBasedVehicleSpeed\"S:=65535\n"
      "R\"ParkBrakeAlert\"C(0,3):=3\n"
      "R\"VehicleMode\"C(0,15):=15\n";

   const char* g_sample_node5 =
      "APX/1.2\n"
      "N\"TestNode5\"\n"
      "R\"WheelBasedVehicleSpeed\"S:=65535\n"
      "R\"CabTiltLockWarning\"C(0,7):=7\n"
      "P\"GearSelectionMode\"C(0,7):=7\n";

   const char* g_sample_node6 =
      "APX/1.3\n"
      "N\"TestNode6\"\n"
      "P\"DiagReq\"b[128*]\n"
      "R\"DiagRsp\"b[128*]\n";

}
