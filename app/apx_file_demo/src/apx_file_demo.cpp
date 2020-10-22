#include <filesystem>
#include <iostream>
#include <string>
#include "cpp-apx/file_client.h"

using namespace std::string_literals;

static char const* apx_definition =
      "APX/1.2\n"
      "N\"TestNode\"\n"
      "P\"WheelBasedVehicleSpeed\"S:=65535\n"
      "P\"CabTiltLockWarning\"C(0,7):=7\n"
      "P\"VehicleMode\"C(0,15):=15\n"
      "R\"GearSelectionMode\"C(0,7):=7\n"
      "R\"ParkBrakeAlert\"C(0,3):=3\n";

static constexpr apx::port_id_t WheelBasedVehicleSpeed_ID = 0u;
static constexpr apx::port_id_t CabTiltLockWarning_ID = 1u;
static constexpr apx::port_id_t VehicleMode_ID = 2u;
static constexpr apx::port_id_t GearSelectionMode_ID = 0u;
static constexpr apx::port_id_t ParkBrakeAlert_ID = 1u;

static void print_dtl_value(std::string const& name, dtl::ScalarValue const& sv);

static void set_new_output_values(apx::FileClient& client)
{
   bool update_WheelBasedVehicleSpeed = false;
   bool update_CabTiltLockWarning = false;
   bool update_VehicleMode = false;

   if (update_WheelBasedVehicleSpeed)
   {
      auto result = client.write_port(WheelBasedVehicleSpeed_ID, dtl::make_sv<std::uint16_t>(0x1234));
      if (result != APX_NO_ERROR)
      {
         std::cerr << "Call to client.write_port(WheelBasedVehicleSpeed_ID) failed with error " << static_cast<int>(result) << std::endl;
      }
   }

   if (update_CabTiltLockWarning)
   {
      auto result = client.write_port(CabTiltLockWarning_ID, dtl::make_sv<std::uint8_t>(1));
      if (result != APX_NO_ERROR)
      {
         std::cerr << "Call to client.write_port(CabTiltLockWarning_ID) failed with error " << static_cast<int>(result) << std::endl;
      }
   }

   if (update_VehicleMode)
   {
      auto result = client.write_port(VehicleMode_ID, dtl::make_sv<std::uint8_t>(4));
      if (result != APX_NO_ERROR)
      {
         std::cerr << "Call to client.write_port(VehicleMode_ID) failed with error " << static_cast<int>(result) << std::endl;
      }
   }
}

static void save_output(apx::FileClient& client)
{
   auto result = client.save_provide_port_data();
   if (result != APX_NO_ERROR)
   {
      std::cerr << "Call to client.save_provide_port_data() failed with error " << static_cast<int>(result) << std::endl;
   }
}

static void print_input(apx::FileClient& client)
{
   dtl::ScalarValue sv;
   auto result = client.read_port(GearSelectionMode_ID, sv);
   if (result != APX_NO_ERROR)
   {
      std::cerr << "Call to client.read_port(GearSelectionMode_ID) failed with error " << static_cast<int>(result) << std::endl;
   }
   else
   {
      print_dtl_value("GearSelectionMode", sv);
   }
   result = client.read_port(ParkBrakeAlert_ID, sv);
   if (result != APX_NO_ERROR)
   {
      std::cerr << "Call to client.read_port(ParkBrakeAlert_ID) failed with error " << static_cast<int>(result) << std::endl;
   }
   else
   {
      print_dtl_value("ParkBrakeAlert", sv);
   }
}

static bool create_node(apx::FileClient& client)
{
   auto result = client.build_node(apx_definition);
   if (result != APX_NO_ERROR)
   {
      std::cerr << "Call to client.build_node() failed with error " << static_cast<int>(result) << std::endl;
      return false;
   }
   result = client.save_if_new();
   if (result != APX_NO_ERROR)
   {
      std::cerr << "Call to client.save_if_new() failed with error " << static_cast<int>(result) << std::endl;
      return false;
   }
   return true;
}

int main()
{
   apx::FileClient client;
   if (create_node(client))
   {
      print_input(client);
      set_new_output_values(client);
      save_output(client);
   }
   return 0;
}

static void print_dtl_value(std::string const& name, dtl::ScalarValue const& sv)
{
   bool ok{ false };
   std::uint32_t u32_val;
   switch (sv->sv_type())
   {
   case dtl::ScalarType::None:
      break;
   case dtl::ScalarType::UInt32:
      u32_val = sv->to_u32(ok);
      std::cout << name << ": " << u32_val << std::endl;
      break;
   default:
      std::cerr << "Print of scalar type (" << (int)sv->sv_type() << ") not implemented" << std::endl;
   }
}