#include "mrt_proto.h"

using namespace MRTProto;
//-----------------------------------------------------------------------------------------------
FrameComposer::FrameComposer()
{
    m_head.mark    = MRT_PROTO_H_Mark;
    m_head.version = MRT_PROTO_H_Ver;
    m_head.type    = reqHandShake;
    m_head.payloadLength = 0;
    setChkSumFunc(mrtCheckSum);
}

bool FrameComposer::setPayLoad(BYTE_t *_pl, PAYLOAD_L_t _l)
{
    if (m_composerLock) return false;
    if (_l+m_head.payloadLength <= PAYLOAD_MAX_SIZE)
    {
        std::memcpy( m_payload, _pl, _l ); m_head.payloadLength = _l;
        return true;
    }
    else return false;
}
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
bool FrameComposer::addPayLoadBlock (BYTE_t * _pl, PAYLOAD_L_t _l)
{
    if (m_composerLock) return false;
    if (_l+m_head.payloadLength <= PAYLOAD_MAX_SIZE)
    {
        std::memcpy( m_payload + m_head.payloadLength, _pl, _l ); m_head.payloadLength += _l;
        return true;
    }
    else return false;
}
//-----------------------------------------------------------------------------------------------
void FrameComposer::composeFrame()
{
    // - Контрольную сумму вносим в Payload только при передаче reqSet (передача уставок)



    std::memcpy( m_frame, &m_head, sizeof(Head) );
    std::memcpy( m_frame + sizeof(Head), m_payload, m_head.payloadLength );
    //FRAME_L_t frameLength = sizeof(Head) + getPayLoadLength();

    //CHKSUM_t chkSum = 0;
    //if (m_crcFunc) chkSum = m_crcFunc( m_frame, frameLength );
    //std::memcpy( m_frame + frameLength, &chkSum, m_head.payloadLength );
}
//-----------------------------------------------------------------------------------------------
void FrameComposer::setChkSumFunc(CHKSUM_t (*_crcFunc)( const BYTE_t * , const FRAME_L_t& ) )
{    
    if (_crcFunc) m_crcFunc = _crcFunc;
}

void FrameComposer::addChkSumToPayload(const BYTE_t * buf, const FRAME_L_t &len)
{
    CHKSUM_t CS = mrtCheckSum(buf,len);
    //FRAME_L_t frameLength = sizeof(Head) + getPayLoadLength();
    //m_head.payloadLength += 2;                  // add CS length to head
    addPayLoadBlock((uint8_t*)(&CS),2);
    //std::memcpy(m_frame + frameLength, &CS, 2); // add CS to payload
}
//-----------------------------------------------------------------------------------------------
CHKSUM_t MRTProto::mrtCheckSum(const BYTE_t * _data, const FRAME_L_t& _length)
{
    FRAME_L_t count{_length};
    CHKSUM_t   res{0xFFFF}; //Load a 16–bit register with FFFF hex (all 1’s).
    while(count--) {
        BYTE_t data1=*_data;
        _data++;
        res ^= data1;
        for(int j=0;j<8;j++) {
            //Shift the CRC register one bit to the right (toward the LSB),
            //(If the LSB was 1): Exclusive OR the CRC register with the polynomial value 0xA001 (1010 0000 0000 0001).
            if(res & 0x0001){
                res=(res>>1) ^0xA001;
            } // LSB(b0)=1
            else res=res>>1; // (If the LSB was 0): Repeat Step 3.
        }
    }
    return res;

    //    while (count++ <_length)
    //    {
    //        res+= *(_data++);
    //    }
    //return res;
}
//-----------------------------------------------------------------------------------------------
//--------------Класс разбора фрейма-------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
FrameParser* FrameParser::pThis = nullptr;

FrameParser::FrameParser (volatile BYTE_t *  t_frame, protoMSG* t_msg)
{
    if(pThis == nullptr) {
        pThis = this;
        mFrame = (BYTE_t*)t_frame; // ptr on global data buffer
        mMsg = t_msg;
    }
}
//-----------------------------------------------------------------------------------------------
FrameParser::ParseErrors FrameParser::parseFrame(const BYTE_t * _frm, const FRAME_L_t& _length)
{
    if (_length >= sizeof (Head)) // + sizeof (CHKSUM_t) )
    {
        auto head = reinterpret_cast<const Head*>(_frm); //чтобы отфильтровать "левый" пакет - нужно закинуть хотя бы
        //количество байт размерности Head. Иначе reinterpret_cast "создаст лажу"

        if (head->mark    != MRT_PROTO_H_Mark)                return parseNotMRT;
        if (head->version != MRT_PROTO_H_Ver)                 return parseWrongVersion;
        if (head->type <= minBound || head->type >= maxBound) return parseWrongType;

        {
            mMsg->type    = head->type;
            mMsg->msgSize = head->payloadLength;
            mMsg->payload = (uint8_t*)_frm + sizeof(Head);
            //memcpy(MSG.payload, _frm+sizeof(Head), head->payloadLength);            
            Emit(mMsg); // send ptr of actual received parsed frame to all subscribed functions
        }

    }
    else return parseWrongFrameLength ;

    return parseOk;
}

bool FrameParser::isFullFrameInPacket(const FRAME_L_t& _length) {
    const MRTProto::Head* head = reinterpret_cast<const MRTProto::Head*>(mFrame);
    if(head->mark != MRTProto::MRT_PROTO_H_Mark) {
        return false;
    }
    if(head->payloadLength > _length - sizeof(MRTProto::Head)) {
        return false;
    } else {
        return true;
    }
}

bool FrameParser::isCmdSet(){
    const MRTProto::Head* head = reinterpret_cast<const MRTProto::Head*>(mFrame);
    if(head->mark != MRTProto::MRT_PROTO_H_Mark) {
        return false;
    }
    if(head->type == MRTProto::FrameType::cmdSet) {
        return true;
    } else {
        return false;
    }
}

//-----------------------------------------------------------------------------------------------




















