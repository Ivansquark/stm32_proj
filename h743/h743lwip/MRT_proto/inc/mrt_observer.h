#ifndef MRT_OBSERVER_H
#define MRT_OBSERVER_H
/*-------------------------------------------------------------------
 * Реализация шаблона "Наблюдатель" для оповещения объектов о приёме
 * сообщений конкретного типа.
 * Макеев И.А. 09.2021
 * -----------------------------------------------------------------*/
#include "mrt_proto_types.h"
//-------------------------------------------------------------------
namespace MRTProto
{

constexpr int MSG_BUFFER_SIZE     = 64;
constexpr int SUBSCRIBERS_MAX_CNT = 100;
class protoMSG;

typedef void (*MSG_SLOT)(const protoMSG *); //тип функций обработчиков событий

static MRTProto::MSG_SLOT slotBuf[SUBSCRIBERS_MAX_CNT];

//-------Класс публицист(раздаёт сообщения)--------------------------
class Advertiser
{
public:
    void Connect(MSG_SLOT  _sbscr) //добавляем функцию обработки событий (слот)
     {
        if ((m_slotCnt < SUBSCRIBERS_MAX_CNT) && (_sbscr!=nullptr))
        {
          m_slots[m_slotCnt] = _sbscr;
          ++m_slotCnt;
        }
     }

    void Emit(const protoMSG* _msg) const //раздаем всем слотам msg
    {
        for (int i=0; i<m_slotCnt; ++i) {
            m_slots[i](_msg);
        }
    }

protected:
    int m_slotCnt = 0; //количество подключенных слотов
    //MSG_SLOT m_slots[SUBSCRIBERS_MAX_CNT]; //подключенные слоты TODO: заменить на контейнер
     MSG_SLOT *m_slots=slotBuf;
};

//-------Класс сообщений для подписчиков-----------------------------
class protoMSG
{
public:
    protoMSG(){}

    FrameType type = sndHeartBeat;
       //BYTE_t payload[MSG_BUFFER_SIZE];
    BYTE_t* payload = nullptr;
  PAYLOAD_L_t msgSize = 0;

  protoMSG(const protoMSG& src) //конструктор копирования
  {
    type    = src.type;
    msgSize = src.msgSize;
    std::memcpy( payload, src.payload, src.msgSize );
  }

  protoMSG& operator= (protoMSG src) //оператор присваивания
  {
     if (this == &src) return *this; //проверка самоприсвоения
     type    = src.type;
     msgSize = src.msgSize;
     std::memcpy( payload, src.payload, src.msgSize );
     return *this;
  }
};

}
//-------------------------------------------------------------------
#endif // MRT_OBSERVER_H