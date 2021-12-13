#include "mex.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <EmergentCameraAPIs.h>
#include <EvtParamAttribute.h>
#include <gigevisiondeviceinfo.h>
#include <EmergentFrameSave.h>

#define SUCCESS 0
#define MAX_CAMERAS 4
#define BUFFPREVIEW 1500
#define FIXEDBUFFER 1500

using namespace std;
using namespace Emergent;

void printParam(char *name ,unsigned int param);
void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) {
    
    CEmergentCamera cam_p[MAX_CAMERAS];
    CEmergentFrame evtFrame[BUFFPREVIEW];
    struct GigEVisionDeviceInfo deviceInfo[MAX_CAMERAS];
    unsigned int listcam_buf_size_ = MAX_CAMERAS;
    unsigned int count_;
    unsigned int ancho = 0;
    unsigned int alto = 0;
    unsigned int gain  = 0;
    unsigned int exposure = 0;
    unsigned int frame_rate = 0;
    unsigned int FRAMESCAP = (unsigned int)mxGetScalar(prhs[0]);
    int CAMERAS = 0;
    int ReturnVal = SUCCESS;
    unsigned int frames_recd = 0;
    EVT_ERROR err = EVT_SUCCESS;
    
    
    EVT_ListDevices(deviceInfo, &listcam_buf_size_, &count_);
    cout << "Camaras conectadas:"<< count_ << endl;
    CAMERAS = count_;
    if(count_ != 0){
        for(int cam = 0; cam < CAMERAS; cam ++){
            EVT_CameraOpen(&cam_p[cam], &deviceInfo[cam]);
            cout << "Abriendo camara: " << cam+1<< endl;
            EVT_CameraOpenStream(&cam_p[cam]);
        }
    }else{cout<<"No hay camaras conectadas." << endl;}
    EVT_CameraGetUInt32Param(&cam_p[0],"Height", &alto);
    EVT_CameraGetUInt32Param(&cam_p[0],"Width", &ancho);
    EVT_CameraGetUInt32Param(&cam_p[0],"Gain", &gain);
    EVT_CameraGetUInt32Param(&cam_p[0],"Exposure", &exposure);
    EVT_CameraGetUInt32Param(&cam_p[0],"FrameRate", &frame_rate);
    printParam("Gain",gain);
    printParam("Alto",alto);
    printParam("Ancho",ancho);
    printParam("Exposure",exposure);
    printParam("Frame Rate",frame_rate);
    plhs[0] = mxCreateNumericMatrix(ancho*alto,FRAMESCAP,mxUINT8_CLASS,mxREAL);
    unsigned char *outMatrix = (unsigned char*)mxGetData(plhs[0]);
    // plhs[1] = mxCreateNumericMatrix(1,ancho*alto,mxUINT8_CLASS,mxREAL);
    //unsigned char *outMatrix_ = (unsigned char*)mxGetData(plhs[1]);
    plhs[1]= mxCreateDoubleScalar(alto);
    plhs[2]= mxCreateDoubleScalar(ancho);
    plhs[3] = mxCreateDoubleScalar(frame_rate);
    plhs[4] = mxCreateDoubleScalar(gain);
    plhs[5] = mxCreateDoubleScalar(exposure);
    plhs[6] = mxCreateDoubleScalar(CAMERAS);
    for(int frame = 0; frame < BUFFPREVIEW ; frame++){
        evtFrame[frame].size_x = alto;
        evtFrame[frame].size_y = ancho;
        evtFrame[frame].pixel_type = GVSP_PIX_BAYGB8;
        EVT_AllocateFrameBuffer(&cam_p[0], &evtFrame[frame], EVT_FRAME_BUFFER_ZERO_COPY);
        EVT_CameraQueueFrame(&cam_p[0], &evtFrame[frame]);
    }
    
    for(int cam = 0; cam < CAMERAS; cam++){
        EVT_CameraExecuteCommand(&cam_p[cam], "AcquisitionStart");
    }
    
    for(int frame = 0; frame < FRAMESCAP; frame++){
        EVT_CameraQueueFrame(&cam_p[0], &evtFrame[frame]);
        err = EVT_CameraGetFrame(&cam_p[0], &evtFrame[frame], EVT_INFINITE);
        if (err) {
            cout << "EVT_CameraGetFrame Error!" << endl;
            break;
        }
        else{frames_recd++;}
        
        
        
        for(int i = 0; i < alto*ancho ;i++){
            outMatrix[i+(frame*alto*ancho)] = evtFrame[frame].imagePtr[i];
            
        }
    }
    
    EVT_CameraExecuteCommand(&cam_p[0], "AcquisitionStop");
    
    for(int frame=0;frame<FRAMESCAP;frame++)
    {
        EVT_ReleaseFrameBuffer(&cam_p[0], &evtFrame[frame]);
    }
    
    for(int cam = 0; cam < CAMERAS; cam++){
        EVT_CameraCloseStream(&cam_p[cam]);
        EVT_CameraClose(&cam_p[cam]);
    }
}



void printParam(char *name ,unsigned int param){
    cout << name  << ": "<< param << endl;
}
