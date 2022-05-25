#ifndef MRT_PROTO_DATA_H
#define MRT_PROTO_DATA_H
/*-------------------------------------------------------------------
 * Классы для парсинга и упаковки сообщений по протоколу
 * MRT-H
 * Макеев И.А. 08.2021
 * -----------------------------------------------------------------*/
#include "mrt_proto_types.h"
#include "mrt_observer.h"
//-------------------------------------------------------------------
namespace MRTProto {

//-------------------------------------------------------------------

CHKSUM_t mrtCheckSum(const BYTE_t * _data, const FRAME_L_t& _length); //прототип функции подсчета контрольной суммы

//-------Класс компановщик фреймов протокола MRTProto----------------
class FrameComposer
{
public:
    FrameComposer();
    bool setPayLoad (BYTE_t * _pl, PAYLOAD_L_t _l);      //установить всю полезную нагрузку одним куском
    bool addPayLoadBlock (BYTE_t * _pl, PAYLOAD_L_t _l); //добавить кусок структуры или массива в полезную нагрузку
    void setHead (const Head& _h) {m_head = _h;}         //установить настройки заголовка
    void setChkSumFunc(CHKSUM_t (*_crcFunc)(const BYTE_t * , const FRAME_L_t& ) ); //назначить функцию для вычисления контрольной суммы

    //////////////////////////////////////////////////////////////////////////
    void addChkSumToPayload(const BYTE_t * ,const FRAME_L_t&); //добавить вычисленную КС массива в полезную нагрузку
    //////////////////////////////////////////////////////////////////////////

    Head * getHead() {return &m_head;}
    PAYLOAD_L_t getPayLoadLength() const {return m_head.payloadLength;}

    const  BYTE_t * getFrame()       const {return m_frame;}
    const  BYTE_t * getPayload()     const {return m_payload;}
    const FRAME_L_t getFrameLength() const {return m_head.payloadLength + sizeof(Head);} // + sizeof(CHKSUM_t);}

    void composeFrame(); //скомпоновать фрейм с заголовком, полезной нагрузкой и контрольной суммой и сохранить его в m_frame[]

private:
    Head m_head;
    BYTE_t m_payload[PAYLOAD_MAX_SIZE];
    BYTE_t m_frame[BYTE_t_MAX]; //буфер для хранения всего фрейма (актуален только после запуска composeFrame() )
    bool m_composerLock=false; //true: фрейм скомпанован, добавлена контрольная сумма и он готов к отправке.
    //false: фрейм не завершен, сброшен и т.д.

    CHKSUM_t (*m_crcFunc)(const BYTE_t * ,const FRAME_L_t&) = nullptr;
};
//-------Класс парсер фреймов протокола MRTProto---------------------

class FrameParser : public Advertiser
{
public:
    FrameParser(volatile BYTE_t *  t_frame, protoMSG* t_msg);

    typedef enum
    {
        parseOk,
        parseNotMRT,
        parseWrongVersion,
        parseWrongChkSum,
        parseWrongType,
        parseWrongFrameLength,
        parseError,
    } ParseErrors;

    ParseErrors parseFrame(const BYTE_t * ,const FRAME_L_t&); //разбирает и проверяет заголовок и контрольную сумму.
    //формирует структуру protoMSG и раздает указатель на неё
    //всем подключенным слотам

    static FrameParser* pThis;
    //! methods that used in parsing packets
    bool isCmdSet();
    bool isFullFrameInPacket(const FRAME_L_t& _length);
    
    protoMSG* mMsg;

private:
    Head* m_head;
    BYTE_t* m_payload;
    //BYTE_t* m_frame;
    bool m_connected=false;
    const BYTE_t* mFrame = nullptr;

    FrameParser(const FrameParser&) = delete;
    FrameParser& operator=(const FrameParser&) = delete;
    FrameParser(const FrameParser&&) = delete;
    FrameParser& operator=(const FrameParser&&) = delete;
};
//-------------------------------------------------------------------
} //namespace MRTProto
#endif // MRT_PROTO_DATA_H
