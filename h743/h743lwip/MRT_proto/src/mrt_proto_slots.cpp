#include "mrt_proto_slots.h"

using namespace MRTProto;

//-------------------------------------------------------------------
//здесь добавляем свои слоты, которые будут обрабатывать сообщения от парсера
//-------------------------------------------------------------------

//-------------------------------------------------------------------
DEC_MRT_SLOT(actionStartScan) {
    if (_msg->type!=FrameType::cmdAction) return;

    auto action = *reinterpret_cast<const pl_cmdAction*>(_msg->payload);
    if (action!=StartScan) return;
    
}
//-------------------------------------------------------------------
DEC_MRT_SLOT(actionStopScan) {
    if (_msg->type!=FrameType::cmdAction) return;
    auto action = *reinterpret_cast<const pl_cmdAction*>(_msg->payload);
    if (action!=StopScan) return;

}
//-------------------------------------------------------------------
DEC_MRT_SLOT(reqSettings) {
    if (_msg->type!=FrameType::reqSet) return;

}
DEC_MRT_SLOT(sndHeartBeat) {
    if (_msg->type!=FrameType::sndHeartBeat) return;
    Ethernet::pThis->sendAnsHeartBeat();    
}
DEC_MRT_SLOT(reqSet) {
    if (_msg->type!=FrameType::reqSet) return;
    Ethernet::pThis->sendAnsSet();    
}