#include <filesystem>
#include <iostream>
#include <string>
#include "cpp-apx/file_client.h"

using namespace std::string_literals;

static constexpr apx::port_id_t WheelBasedVehicleSpeed_ID = 0u;
static constexpr apx::port_id_t CabTiltLockWarning_ID = 1u;
static constexpr apx::port_id_t VehicleMode_ID = 2u;
static constexpr apx::port_id_t GearSelectionMode_ID = 0u;
static constexpr apx::port_id_t ParkBrakeAlert = 1u;

static void print_dtl_value(std::string const& name, dtl::ScalarValue const& sv);

static void set_new_output(apx::FileClient& client)
{
   auto new_value = dtl::make_sv<std::uint16_t>(0x1234);
   auto result = client.write_port(WheelBasedVehicleSpeed_ID, new_value);
   if (result != APX_NO_ERROR)
   {
      std::cerr << "Call to client.write_port() failed with error " << static_cast<int>(result) << std::endl;
   }

   result = client.save_provide_port_data();
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
      std::cerr << "Call to client.read_port() failed with error " << static_cast<int>(result) << std::endl;
   }
   else
   {
      print_dtl_value("GearSelectionMode", sv);
   }
}

int main()
{
   char const* apx_definition =
      "APX/1.2\n"
      "N\"TestNode1\"\n"
      "P\"WheelBasedVehicleSpeed\"S:=65535\n"
      "P\"CabTiltLockWarning\"C(0,7):=7\n"
      "P\"VehicleMode\"C(0,15):=15\n"
      "R\"GearSelectionMode\"C(0,7):=7\n"
      "R\"ParkBrakeAlert\"C(0,3):=3\n";

   auto current_path = std::filesystem::current_path();
   std::cout << current_path.string() << std::endl;
   apx::FileClient client;
   auto result = client.build_node(apx_definition);
   if (result != APX_NO_ERROR)
   {
      std::cerr << "Call to client.build_node() failed with error " << static_cast<int>(result) << std::endl;
      return 0;
   }
   result = client.save_if_new();
   if (result != APX_NO_ERROR)
   {
      std::cerr << "Call to client.save_if_new() failed with error " << static_cast<int>(result) << std::endl;
      return 0;
   }
   print_input(client);
   set_new_output(client);

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