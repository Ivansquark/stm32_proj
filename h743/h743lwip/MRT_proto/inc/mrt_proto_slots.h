#ifndef MRT_PROTO_SLOTS
#define MRT_PROTO_SLOTS

#include "mrt_observer.h"
#include "net.h"

#define DEC_MRT_SLOT(_func) extern void _func(const MRTProto::protoMSG* _msg)
//здесь добавляем прототипы своих слотов, которые будут обрабатывать сообщения от парсера
DEC_MRT_SLOT(handShakeRcv);
DEC_MRT_SLOT(actionStartScan);
DEC_MRT_SLOT(actionStopScan);
DEC_MRT_SLOT(reqSettings);
DEC_MRT_SLOT(printProtocol);
DEC_MRT_SLOT(sndHeartBeat);
DEC_MRT_SLOT(reqSet);

#endif //MRT_PROTO_SLOTS