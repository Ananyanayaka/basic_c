#include "Std_Types.h"
#define PRESBL
#define DioConf_DioChannelGroup_HSIV          (uint8)0x00
typedef uint16 Dio_PortLevelType;
typedef uint8 Dio_PortType;
typedef struct STag_Dio_ChannelGroupType
{
  /* Positions of the Channel Group */
  uint16 usMask;
  /* Position from LSB */
  uint8 ucOffset;
  /* PortGroup Index in the array Dio_GstPortGroup[] with respect to the
     pointer Dio_GpPortGroup of the corresponding configuration */
  uint8 ucPortIndex;
} Dio_ChannelGroupType;
