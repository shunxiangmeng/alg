#ifndef IULUSpeechRec_h
#define IULUSpeechRec_h

#include "IULUBaseType.h"

namespace ulu_face{
//最大分片数
#define MAX_SPEECH_REC_PIECE_COUNT 80
//每个分片最大的STEP
#define MAX_SPEECH_REC_PIECE_STEP 48
//最大的top_n
#define MAX_SPEECH_REC_TOP_N 40

#define MAX_SPEECH_REC_TIME_STEPS (MAX_SPEECH_REC_PIECE_COUNT*MAX_SPEECH_REC_PIECE_STEP)
#define MAX_SPEECH_REC_BUFF_LEN (MAX_SPEECH_REC_TIME_STEPS*MAX_SPEECH_REC_TOP_N)

    struct SSpeechRecResult{
        int piece_num = 0;
        int piece_step = 48;
        int top_n = 40;
        
        float blank_probs[MAX_SPEECH_REC_TIME_STEPS];
        float probs[MAX_SPEECH_REC_BUFF_LEN];
        unsigned int top_n_idx[MAX_SPEECH_REC_BUFF_LEN];
    };
    class IULUSpeechRec
    {
    public:
        IULUSpeechRec();
        virtual ~IULUSpeechRec();
        virtual EReturn_Code Init(const char * model_path)=0;
        virtual EReturn_Code Init(const unsigned char * buffer,size_t buffer_len)=0;
        
        //virtual void SetParams(hMFCC::Config& config) = 0;
        virtual int SpeechRec(const unsigned char * buffer,size_t buffer_len,SSpeechRecResult& result) = 0;
        
        virtual int SetOverlapRate(float rate) = 0;

        virtual int ResetRoute(int route_id) = 0;
    };

    extern "C" EReturn_Code CreateSpeechRec(IULUSpeechRec ** pSpeechRec);
    extern "C" EReturn_Code DestroySpeechRec(IULUSpeechRec ** pSpeechRec);
};

#endif /* IULUSpeechRec_h */
