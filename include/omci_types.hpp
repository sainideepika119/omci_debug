#pragma once

#include <map>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

// OMCI (ONT Management and Control Interface) message-type codes and the
// Managed Entity (ME) attribute database, per ITU-T G.988.
//
// Everything here is `inline` (C++17) so this header is safe to include
// from more than one translation unit — the original version defined these
// maps directly at namespace scope in a header with no `inline`, which
// only worked because exactly one .cpp file included it; a second
// translation unit would have caused multiple-definition linker errors.

namespace omci {

// Message type codes as they appear in the OMCI message header (hex byte,
// represented as the 2-character hex string used throughout parsing).
namespace msgtype {
    inline constexpr std::string_view kAlarm     = "10";
    inline constexpr std::string_view kAvc       = "11";
    inline constexpr std::string_view kCreate    = "44";
    inline constexpr std::string_view kCreateRes = "24";
    inline constexpr std::string_view kDelete    = "46";
    inline constexpr std::string_view kDeleteRes = "26";
    inline constexpr std::string_view kSet       = "48";
    inline constexpr std::string_view kSetRes    = "28";
    inline constexpr std::string_view kGet       = "49";
    inline constexpr std::string_view kGetRes    = "29";
} // namespace msgtype

// Human-readable name for every message type code seen in practice.
inline const std::unordered_map<std::string, std::string> kMessageTypeNames = {
    {"10", "Alarm"},
    {"11", "Attribute Value Change"},
    {"44", "Create"},
    {"24", "Create Response"},
    {"46", "Delete"},
    {"26", "Delete Response"},
    {"48", "Set"},
    {"28", "Set Response"},
    {"49", "Get"},
    {"29", "Get Response"},
    {"4b", "Get All Alarms"},
    {"2b", "Get All Alarms Response"},
    {"4c", "Get All Alarms Next"},
    {"2c", "Get All Alarms Next Response"},
    {"4d", "Mib Upload"},
    {"2d", "Mib Upload Response"},
    {"4e", "Mib Upload Next"},
    {"2e", "Mib Upload Next Response"},
    {"4f", "MIB Reset"},
    {"2f", "MIB Reset Response"},
    {"58", "Synchronize Time"},
    {"38", "Synchronize Time Response"},
    {"53", "Start Software Download"},
    {"33", "Start Software Download Response"},
    {"54", "Download Section"},
    {"34", "Download Section Response"},
    {"55", "End Software Download"},
    {"35", "End Software Download Response"},
};

// How to render an attribute's value once decoded: as a plain name-only
// row (the ME's own class name, index 0 of each entry below), a decimal
// integer, or a raw hex string. This is the field the original code
// defined but never actually read when formatting output.
enum class AttrType { ClassName, Int, Hex };

struct AttributeInfo {
    std::string name;
    AttrType type;
    int size; // size in bytes
};

// Managed Entity class database: ME class ID -> attribute list.
// Index 0 of each vector is the ME's own display name (AttrType::ClassName);
// indices 1..16 are its up-to-16 OMCI attributes, in attribute-mask order.
inline const std::map<int, std::vector<AttributeInfo>> kClassMap = {

		{2, {
				{"ONU data", AttrType::ClassName, 0},
				{"MIB data sync", AttrType::Int, 1}
			}
		},
		{5, {
				{"Cardholder"	, AttrType::ClassName, 0},
				{"ActualPlugInUnitType", AttrType::Int, 1},
				{"ExpectedPlugInUnitType", AttrType::Int, 1},
				{"ExpectedPortCount", AttrType::Int, 1},
				{"ExpectedEquipmentId", AttrType::Hex, 20},
				{"ActualEquipmentId", AttrType::Hex, 20},
				{"ProtectionProfilePointer", AttrType::Int, 1},
				{"InvokeProtectionSwitch", AttrType::Int, 1},
				{"AlarmReportingControl", AttrType::Hex, 1},
				{"AlarmReportingControlInterval", AttrType::Int, 1}
			}
		},
		{6, {
				{"Circuit pack"	, AttrType::ClassName, 0},
				{"Type", AttrType::Int, 1},
				{"NumberOfPorts", AttrType::Int, 1},
				{"SerialNumber", AttrType::Hex, 8},
				{"Version", AttrType::Hex, 14},
				{"VendorId", AttrType::Hex, 4},
				{"AdministrativeState", AttrType::Int, 1},
				{"OperationalState", AttrType::Int, 1},
				{"BridgedIpInd", AttrType::Int, 1},
				{"EquipmentId", AttrType::Hex, 20},
				{"CardConfiguration", AttrType::Int, 1},
				{"TotalTcontBufferNumber", AttrType::Int, 1},
				{"TotalPriorityQueueNumber", AttrType::Int, 1},
				{"TotalTrafficSchedulerNumber", AttrType::Int, 1},
				{"PowerShedOverride", AttrType::Int, 4},
			}
		},
		{7, {
				{"Software image", AttrType::ClassName, 0},
				{"Version", AttrType::Hex, 14},
				{"IsCommitted", AttrType::Int, 1},
				{"IsActive", AttrType::Int, 1},
				{"IsValid", AttrType::Int, 1},
				{"ProductCode", AttrType::Hex, 25},
				{"ImageHash", AttrType::Hex, 16}
			}
		},
		{11, {
				{"PptpEthernetUni", AttrType::ClassName, 0},
				{"ExpectedType", AttrType::Int, 1},
				{"SensedType", AttrType::Int, 1},
				{"AutoDetectionConfiguration", AttrType::Int, 1},
				{"LoopbackConfiguration", AttrType::Int, 1},
				{"AdministrativeState", AttrType::Int, 1},
				{"OperationalState", AttrType::Int, 1},
				{"ConfigurationInd", AttrType::Int, 1},
				{"MaxFrameSize", AttrType::Int, 2},
				{"DteDceInd", AttrType::Int, 1},
				{"PauseTime", AttrType::Int, 2},
				{"BridgedIpInd", AttrType::Int, 1},
				{"AlarmReportingControl", AttrType::Hex, 1},
				{"AlarmReportingControlInterval", AttrType::Int, 1},
				{"PppoeFilter", AttrType::Hex, 1},
				{"PowerControl", AttrType::Hex, 1}
			}
		},
		{45, {
				{"Mac Bridge Service Profile", AttrType::ClassName, 0},
				{"SpanningTree", AttrType::Hex, 1},
				{"Learning", AttrType::Hex, 1},
				{"PortBridging", AttrType::Hex, 1},
				{"Priority", AttrType::Int, 2},
				{"MaxAge", AttrType::Int, 2},
				{"HelloTime", AttrType::Int, 2},
				{"ForwardDelay", AttrType::Int, 2},
				{"UnknownMacAddressDiscard", AttrType::Hex, 1},
				{"MacLearningDepth", AttrType::Int, 1},
				{"DynamicFilteringAgeingTime", AttrType::Int, 4}
			}
		},
		{46, {
				{"Mac Bridge Config Data", AttrType::ClassName, 0},
				{"MacAddress", AttrType::Hex, 6},
				{"Priority", AttrType::Int, 2},
				{"DesignatedRoot", AttrType::Hex, 6},
				{"RootPathCost", AttrType::Int, 4},
				{"PortCount", AttrType::Int, 1},
				{"RootPortNum", AttrType::Int, 2},
				{"HelloTime", AttrType::Int, 2},
				{"ForwardDelay", AttrType::Int, 2}
			}
		},

		{47, {
				{"Mac Bridge Port Config Data", AttrType::ClassName, 0},
				{"BridgeId", AttrType::Int, 2},
				{"PortNum", AttrType::Int, 1},
				{"TpType", AttrType::Int, 1},
				{"TpPointer", AttrType::Int, 2},
				{"Priority", AttrType::Int, 2},
				{"PathCost", AttrType::Int, 2},
				{"SpanningTree", AttrType::Hex, 1},
				{"EncapMethod", AttrType::Int, 1},
				{"LanFcs", AttrType::Int, 1},
				{"MacAddress", AttrType::Hex, 6},
				{"OutTdPointer", AttrType::Int, 2},
				{"InTdPointer", AttrType::Int, 2},
				{"MacLearningDepth", AttrType::Int, 1}
			}
		},

		{53, {
				{"PptpPotsUni"	, AttrType::ClassName, 0},
				{"AdministrativeState", AttrType::Int, 1},
				{"InterworkingTpPointer", AttrType::Int, 2},
				{"AlarmReportingControl", AttrType::Hex, 1},
				{"AlarmReportingControlInterval", AttrType::Int, 1},
				{"Impedance", AttrType::Int, 1},
				{"TransmissionPath", AttrType::Int, 1},
				{"RxGain", AttrType::Int, 1},
				{"TxGain", AttrType::Int, 1},
				{"OperationalState", AttrType::Int, 1},
				{"HookState", AttrType::Int, 1},
				{"PotsHoldoverTime", AttrType::Int, 2},
				{"NominalFeedVoltage", AttrType::Int, 1},
				{"LossOfSoftswitch", AttrType::Int, 1}
			}
		},
		{134, {
				{"IP host config data", AttrType::ClassName, 0},
				{"IP options", AttrType::Hex, 1},
				{"MAC address", AttrType::Hex, 6},
				{"Onu identifier", AttrType::Hex, 25},
				{"IP address", AttrType::Int, 4},
				{"Mask", AttrType::Hex, 4},
				{"Gateway", AttrType::Hex, 4},
				{"Primary DNS", AttrType::Hex, 4},
				{"Secondary DNS", AttrType::Hex, 4},
				{"Current address", AttrType::Hex, 4},
				{"Current mask", AttrType::Hex, 4},
				{"Current gateway", AttrType::Hex, 4},
				{"Current primary DNS", AttrType::Hex, 4},
				{"Current secondary DNS", AttrType::Hex, 4},
				{"Domain name",AttrType::Hex , 25},
				{"Host name", AttrType::Hex , 25},
				{"Relay agent options", AttrType::Hex, 2}
			}
		},
		{171, {
				{"Extended Vlan Tagging Operation Configuration Data", AttrType::ClassName, 0},
				{"AssociationType", AttrType::Int, 1},
				{"ReceivedFrameVlanTaggingOperationTableMaxSize", AttrType::Int, 2},
				{"InputTpid", AttrType::Int, 2},
				{"OutputTpid", AttrType::Int, 2},
				{"DownstreamMode", AttrType::Int, 1},
				{"ReceivedFrameVlanTaggingOperationTable", AttrType::Hex, 16},
				{"AssociatedManagedEntityPointer", AttrType::Int, 2},
				{"DscpToPBitMapping", AttrType::Hex, 24}
			}
		},
		{244, {
				{"Gpsu Info", AttrType::ClassName, 0},
				{"GpsuStatus", AttrType::Int, 1},
				{"ChargeFlags", AttrType::Int, 1},
				{"AlarmFlags", AttrType::Hex, 3},
				{"VendorCode", AttrType::Hex, 6},
				{"SerialNumber", AttrType::Hex, 13}
			}
		},
		{245, {
				{"Gpsu Pm", AttrType::ClassName, 0},
				{"IntervalEndTime", AttrType::Int, 1},
				{"ThresholdDataId", AttrType::Int, 2},
				{"Battery_voltage", AttrType::Int, 4},
				{"Charging_current", AttrType::Int, 4},
				{"Max_battery_voltage", AttrType::Int, 4},
				{"Max_charging_current", AttrType::Int, 4},
				{"Solar_panel_voltage", AttrType::Int, 4},
				{"Solar_panel_current", AttrType::Int, 4},
				{"Smps_voltage", AttrType::Int, 4},
				{"Smps_current", AttrType::Int, 4},
				{"Load_voltage", AttrType::Int, 4},
				{"Load_current", AttrType::Int, 4}
			}
		},
		{256, {
				{"ONU-G ", AttrType::ClassName, 0},
				{"VendorId", AttrType::Int, 4},
				{"Version", AttrType::Hex, 14},
				{"SerialNumber", AttrType::Hex, 8},
				{"TrafficManagementOption", AttrType::Int, 1},
				{"Deprecated", AttrType::Int, 1},
				{"BatteryBackup", AttrType::Int, 1},
				{"AdministrativeState", AttrType::Int, 1},
				{"OperationalState", AttrType::Int, 1},
				{"OnuSurvivalTime", AttrType::Int, 1},
				{"LogicalOnuId", AttrType::Hex, 24},
				{"LogicalPassword", AttrType::Hex, 12},
				{"CredentialsStatus", AttrType::Int, 1},
				{"ExtendedTCLayerOptions", AttrType::Int, 2}
			}
		},
		{257, {
				{"ONU2-G ", AttrType::ClassName, 0},
				{"EquipmentId", AttrType::Hex, 20},
				{"OmccVersion", AttrType::Int, 1},
				{"VendorProductCode", AttrType::Int, 2},
				{"SecurityCapability", AttrType::Int, 1},
				{"SecurityMode", AttrType::Int, 1},
				{"TotalPriorityQueueNumber", AttrType::Int, 2},
				{"TotalTrafficSchedulerNumber", AttrType::Int, 1},
				{"Deprecated", AttrType::Int, 1},
				{"TotalGemPortIdNumber", AttrType::Int, 2},
				{"SysUpTime", AttrType::Int, 4},
				{"ConnectivityCapability", AttrType::Int, 2},
				{"CurrentConnectivityMode", AttrType::Int, 1},
				{"QosConfigFlexibiltiy", AttrType::Int, 2},
				{"PriorityQueueScaleFactor", AttrType::Int, 2}
			}
		},
		{262, {
				{"T-CONT", AttrType::ClassName, 0},
				{"AllocId", AttrType::Int, 2},
				{"Deprecated", AttrType::Int, 1},
				{"Policy", AttrType::Int, 1}
			}
		},
		{263, {
				{"ANI-G", AttrType::ClassName, 0},
				{"SrIndication", AttrType::Hex, 1},
				{"TotalTcontNumber", AttrType::Int, 2},
				{"GemBlockLength", AttrType::Int, 2},
				{"PiggybackDbaReporting", AttrType::Int, 1},
				{"Deprecated", AttrType::Int, 1},
				{"SfThreshold", AttrType::Int, 1},
				{"SdThreshold", AttrType::Int, 1},
				{"AlarmReportingControl", AttrType::Int, 1},
				{"AlarmReportingControlInterval", AttrType::Int, 1},
				{"OpticalSignalLevel", AttrType::Int, 2},
				{"LowerOptivalThreshold", AttrType::Int, 1},
				{"UpperOptivalThreshold", AttrType::Int, 1},
				{"OntResponseTime", AttrType::Int, 2},
				{"TransmitOpticalLevel", AttrType::Int, 2},
				{"LowerTransmitPowerThreshold", AttrType::Int, 1},
				{"UpperTransmitPowerThreshold", AttrType::Int, 1}
			}
		},
		{264, {
				{"UNI-G", AttrType::ClassName, 0},
				{"Deprecated", AttrType::Int, 2},
				{"AdministrativeState", AttrType::Int, 1},
				{"ManagementCapability", AttrType::Int, 1},
				{"NonOmciManagementId", AttrType::Int, 2},
				{"RelayAgentOptions", AttrType::Int, 2}
			}
		},
		{268, {
				{"Gem Port Network Ctp", AttrType::ClassName, 0},
				{"PortIdValue", AttrType::Int, 2},
				{"TContPointer", AttrType::Int, 2},
				{"Direction", AttrType::Int, 1},
				{"UpstreamTrafficManagementPointer", AttrType::Int, 2},
				{"UpstreamTrafficDescriptorProfilePointer", AttrType::Int, 2},
				{"UniCounter", AttrType::Int, 1},
				{"DownstreamPriorityQueuePointer", AttrType::Int, 2},
				{"EncryptionState", AttrType::Int, 1},
				{"DownstreamTrafficDescriptorProfilePointerr", AttrType::Int, 2}
			}
		},

		{277, {
				{"Priority Queue-G", AttrType::ClassName, 0},
				{"QueueConfigurationOption", AttrType::Int, 2},
				{"MaxQueueSize", AttrType::Int, 2},
				{"AllocatedQueueSize", AttrType::Int, 2},
				{"DiscardBlockCounterResetInterval", AttrType::Int, 2},
				{"BufferOverFlowsDiscardBlockThreshold", AttrType::Int, 2},
				{"RelatedPort", AttrType::Int, 4},
				{"TrafficSchedulerGPointer", AttrType::Int, 2},
				{"Weight", AttrType::Int, 1},
				{"BackPressureOperation", AttrType::Int, 2},
				{"BackPressureTime", AttrType::Int, 4},
				{"BackPressureOccurQueueThreshold", AttrType::Int, 2},
				{"BackPressureOccurQueueThreshold", AttrType::Int, 2},
				{"PacketDropQueueThresholds", AttrType::Hex, 8},
				{"PacketDropMaxP", AttrType::Int, 2},
				{"QueueDropWQ", AttrType::Int, 1},
				{"DropPrecedenceColourMarking", AttrType::Int, 1}
			}
		},
		{281, {
				{"Multicast Gem Interworking Tp", AttrType::ClassName, 0},
				{"GemPortNetworkCtpConnPointer", AttrType::Int, 2},
				{"InterworkingOption", AttrType::Int, 1},
				{"ServiceProfilePointer", AttrType::Int, 2},
				{"InterworkingTermPointPointer", AttrType::Int, 2},
				{"PptpCounter", AttrType::Int, 1},
				{"OperationalState", AttrType::Int, 1},
				{"GalProfilePointer", AttrType::Int, 2},
				{"GalLoopbackConfiguration", AttrType::Int, 1},
				{"MulticastAddressField", AttrType::Hex, 24}
			}
		},
		{296, {
				{"Ethernet Pm History Data 3", AttrType::ClassName, 0},
				{"IntervalEndTime", AttrType::Int, 1},
				{"ThresholdDataId", AttrType::Int, 2},
				{"DropEvents", AttrType::Int, 4},
				{"Octets", AttrType::Int, 4},
				{"Packets", AttrType::Int, 4},
				{"BroadcastPackets", AttrType::Int, 4},
				{"MulticastPackets", AttrType::Int, 4},
				{"UndersizePackets", AttrType::Int, 4},
				{"Fragments", AttrType::Int, 4},
				{"Jabbers", AttrType::Int, 4},
				{"Packets64Octets", AttrType::Int, 4},
				{"Packets127Octets", AttrType::Int, 4},
				{"Packets255Octets", AttrType::Int, 4},
				{"Packets511Octets", AttrType::Int, 4},
				{"Packets1023Octets", AttrType::Int, 4},
				{"Packets1518Octets", AttrType::Int, 4}
			}
		},
		{309, {
				{"Multicast Operations Profile", AttrType::ClassName, 0},
				{"IgmpVersion", AttrType::Int, 1},
				{"IgmpFunction", AttrType::Int, 1},
				{"ImmediateLeave", AttrType::Int, 1},
				{"UpstreamIgmpTci", AttrType::Int, 2},
				{"UpstreamIgmpTagControl", AttrType::Int, 1},
				{"UpstreamIgmpRate", AttrType::Int, 4},
				{"DynamicAccessControlListTable", AttrType::Hex, 24},
				{"StaticAccessControlListTable", AttrType::Hex, 24},
				{"LostGroupsListTable", AttrType::Hex, 10},
				{"Robustness", AttrType::Int, 1},
				{"QuerierIpAddress", AttrType::Int, 4},
				{"QueryInterval", AttrType::Int, 4},
				{"QueryMaxResponseTime", AttrType::Int, 4},
				{"LastMemberQueryInterval", AttrType::Int, 4},
				{"UnauthorizedJoinRequestBehaviour", AttrType::Int, 1},
				{"DownstreamIgmpTci", AttrType::Hex, 3}
			}
		},
		{321, {
				{"Downstream Ethernet Frame Pm", AttrType::ClassName, 0},
				{"IntervalEndTime", AttrType::Int, 1},
				{"ThresholdDataId", AttrType::Int, 2},
				{"DropEvents", AttrType::Int, 4},
				{"Octets", AttrType::Int, 4},
				{"Packets", AttrType::Int, 4},
				{"BroadcastPackets", AttrType::Int, 4},
				{"MulticastPackets", AttrType::Int, 4},
				{"CrcErroredPackets", AttrType::Int, 4},
				{"UndersizePackets", AttrType::Int, 4},
				{"OversizePackets", AttrType::Int, 4},
				{"Packets64Octets", AttrType::Int, 4},
				{"Packets127Octets", AttrType::Int, 4},
				{"Packets255Octets", AttrType::Int, 4},
				{"Packets511Octets", AttrType::Int, 4},
				{"Packets1023Octets", AttrType::Int, 4},
				{"Packets1518Octets", AttrType::Int, 4}
			}
		},
		{322, {
				{"Upstream Ethernet Frame Pm", AttrType::ClassName, 0},
				{"IntervalEndTime", AttrType::Int, 1},
				{"ThresholdDataId", AttrType::Int, 2},
				{"DropEvents", AttrType::Int, 4},
				{"Octets", AttrType::Int, 4},
				{"Packets", AttrType::Int, 4},
				{"BroadcastPackets", AttrType::Int, 4},
				{"MulticastPackets", AttrType::Int, 4},
				{"CrcErroredPackets", AttrType::Int, 4},
				{"UndersizePackets", AttrType::Int, 4},
				{"OversizePackets", AttrType::Int, 4},
				{"Packets64Octets", AttrType::Int, 4},
				{"Packets127Octets", AttrType::Int, 4},
				{"Packets255Octets", AttrType::Int, 4},
				{"Packets511Octets", AttrType::Int, 4},
				{"Packets1023Octets", AttrType::Int, 4},
				{"Packets1518Octets", AttrType::Int, 4}
			}
		},
		{329, {
				{"Virtual Ethernet Interface Point", AttrType::ClassName, 0},
				{"AdministrativeState", AttrType::Int, 1},
				{"OperationalState", AttrType::Int, 1},
				{"InterDomainName", AttrType::Hex, 25},
				{"TcpUdpPointer", AttrType::Int, 2},
				{"IanaAssignedPort", AttrType::Int, 2}
			}
		},
		{425, {
				{"Ethernet FrameExtended PM 64 Bit", AttrType::ClassName, 0},
				{"IntervalEndTime", AttrType::Int, 1},
				{"ControlBlock", AttrType::Hex, 16},
				{"DropEvents", AttrType::Int, 8},
				{"Octets", AttrType::Int, 8},
				{"Frames", AttrType::Int, 8},
				{"BroadcastFrames", AttrType::Int, 8},
				{"MulticastFrames", AttrType::Int, 8},
				{"CrcErroredFrames", AttrType::Int, 8},
				{"UndersizeFrames", AttrType::Int, 8},
				{"OversizeFrames", AttrType::Int, 8},
				{"Frames64Octets", AttrType::Int, 8},
				{"Frames127Octets", AttrType::Int, 8},
				{"Frames255Octets", AttrType::Int, 8},
				{"Frames511Octets", AttrType::Int, 8},
				{"Frames1023Octets", AttrType::Int, 8},
				{"Frames1518Octets", AttrType::Int, 8}
			}
		},
		{65284, {
				{"Sfp Info", AttrType::ClassName, 0},
				{"SfpType", AttrType::Int, 1},
				{"VendorName", AttrType::Hex, 16},
				{"VendorPartNo", AttrType::Hex, 16},
				{"VendorRev", AttrType::Hex, 4},
				{"VendorSerialNumber", AttrType::Hex, 16}
			}
		},
		{65285, {
				{"Sfp Info Pm", AttrType::ClassName, 0},
				{"IntervalEndTime", AttrType::Int, 1},
				{"ThresholdDataId", AttrType::Int, 2},
				{"RxPower", AttrType::Int, 4},
				{"TxPower", AttrType::Int, 4},
				{"Temperature", AttrType::Int, 4},
				{"Current", AttrType::Int, 4},
				{"Voltage", AttrType::Int, 4}
			}
		}
};

} // namespace omci
