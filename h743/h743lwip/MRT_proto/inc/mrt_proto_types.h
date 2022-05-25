#ifndef MRT_PROTO_TYPES_H
#define MRT_PROTO_TYPES_H
/*-------------------------------------------------------------------
 * Описание структур и типов данных для парсинга и упаковки сообщений
 * по протоколу MRT-H
 * Макеев И.А. 08.2021
 * -----------------------------------------------------------------*/
//#include <inttypes.h>
//#include <stdint.h>
#include <memory>
#include <cstring>
#define BYTE_t_MAX 65535
//-------------------------------------------------------------------
namespace MRTProto {

typedef uint16_t MARK_t;
typedef uint16_t VERSION_t;
typedef uint16_t PAYLOAD_L_t;
typedef uint16_t FRAME_L_t;
typedef uint8_t BYTE_t;
///////////////////////////////
//typedef uint32_t CHKSUM_t ;
typedef uint16_t CHKSUM_t ;
//////////////////////////////
typedef uint16_t FRMTYPE_t ;
typedef uint32_t PARAMVALUE_t ;
typedef uint8_t  ACTION_t ;
typedef uint8_t  DEVSTATE_t;

constexpr MARK_t    MRT_PROTO_H_Mark = 0x0facu;
constexpr VERSION_t MRT_PROTO_H_Ver  = 0x0001u;
constexpr int       PAYLOAD_MAX_SIZE = 65535;

typedef enum : FRMTYPE_t
{
 minBound,

  reqHandShake, //запрос приветствия
  ansHandShake, //ответ на приветствие

  reqSet,     //запрос значений уставок
  ansSet,     //ответ со значениями уставок
  cmdSet,     //команда на выставление уставок

  cmdAction,  //команда на выполнение действия

  reqDevState, //запрос состояний устройств(а) (включено\выключено\режим\ и т.д)
  ansDevState, //ответ состояния устройств(а)

  cmdSetMode, //команда перевода установки в различные режимы

//  reqDevReady, //запрос готовности к работе всех девайсов
//  ansDevReady, //ответ готовности к работе всех девайсов

  sndHeartBeat, //отправить пульс
  ansHeartBeat, //прием пульса

//  reqSndConfigJson,
//  rcvSndConfigJson,

 maxBound
}FrameType;
//-------------------------------------------------------------------
//ВНИМАНИЕ! Последовательность объявления и объеденённые группы рассчитаны на
//использование в алгоритмах. Изменение порядка может привести к ошибкам.
//Добавление новых констант только в блоки по типам команд.
//-------------------------------------------------------------------
typedef enum : ACTION_t
{
   //Запус алгоритмов
   StartScan,
   StopScan,
   StartTraining,

   //Управление отдельными девайсами
   BeltLeft, //группа управления транспортёром
   BeltRight,
   BeltStop,

   EmitterOn1, //группа управления включением излучателей
   EmitterOn2,
   EmitterOn3,
   EmitterOn4,

   EmitterOff1,//группа управления выключением излучателей
   EmitterOff2,
   EmitterOff3,
   EmitterOff4,

   DeviceOn, //группа команд управления модулями
   DeviceOff,

}pl_cmdAction;
//-------------------------------------------------------------------
typedef enum : DEVSTATE_t
{
   EmitterStoped,
   EmitterError,
   EmitterWorking,
   EmitterTraining,
}pl_reqStateEmitter;
//-------------------------------------------------------------------
typedef enum : DEVSTATE_t
{
   TunnelStoped      = 0b00000001,
   TunnelMoveLeft    = 0b00000010,
   TunnelMoveRight   = 0b00000100,
   TunnelReverced    = 0b00001000,
   TunnelBarLeftEr   = 0b00010000,
   TunnelBarRightEr  = 0b00100000,
   TunnelBarLeftCls  = 0b01000000,
   TUnnelBarRightCls = 0b10000000,
}pl_reqStateTunnel;
//-------------------------------------------------------------------
typedef enum : DEVSTATE_t
{
   PowerTunnelOn     = 0b00000001,
   PowerEmittersOn   = 0b00000010,
   PowerRemoteCtrlOn = 0b00000100,
   PowerPCOn         = 0b00001000,
   PowerDetectorsOn  = 0b00010000,
}pl_reqStatePower;
//-------------------------------------------------------------------
typedef enum : DEVSTATE_t
{
   DeviceIsOk,
   DeviceError,
   DeviceIsOn,
   DeviceIsOff,
}pl_devState;
//-------------------------------------------------------------------
typedef enum : BYTE_t
{
   ModeDirectControl, //прямое управление узлами с ПК
   ModeNormalControl,

   ModeContScan, //режим непрерывного ввода багажа
   ModeReturnScan, //возвратный режим
   ModeNormalScan,

   ModeAutotest, //режим автотеста

   ModeUpdateFirmware, //режим обновления прошивок узлов установки
}pl_mode;
//---------------Блок для структур с упаковкой-----------------------
#pragma pack(push,1)
struct Head
{
        MARK_t mark;
     VERSION_t version;
     FrameType type;
   PAYLOAD_L_t payloadLength;
};
//-------------------------------------------------------------------
struct pl_settings // для cmdSet и ansSet
{
        BYTE_t ID;
        BYTE_t param;
  PARAMVALUE_t val;
};
//-------------------------------------------------------------------
struct pl_reqSet
{
        BYTE_t ID;
        BYTE_t param;
};
//-------------------------------------------------------------------
struct pl_ansDevState
{
        BYTE_t ID;
        BYTE_t state;
};
//-------------------------------------------------------------------


//-------------------------------------------------------------------
#pragma pack(pop)
//--------------конец блока для структур-----------------------------


}
#endif // MRT_PROTO_TYPES_H
