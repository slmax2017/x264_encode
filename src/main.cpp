#define _CRT_SECURE_NO_WARNINGS
#include "stdint.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
extern "C" {
#include "x264.h"
}

using namespace std;

#define IN_FILE_NAME "cuc_ieschool_640x360_yuv420p.yuv"
#define OUT_FILE_NAME "cuc_ieschool_640x360_yuv420p.h264"

int main()
{
    x264_t *handle = NULL;
    x264_nal_t *p_nal = NULL;
    int nal = 0;
    x264_param_t param;
    x264_picture_t in_pic;
    x264_picture_t out_pic;

    x264_param_default_preset(&param, "medium", "film");
    x264_param_apply_profile(&param, "main");
    param.i_width = 640;
    param.i_height = 360;

    x264_picture_init(&in_pic);
    x264_picture_init(&out_pic);

    FILE *in_fp = fopen(IN_FILE_NAME, "rb");
    FILE *out_fp = fopen(OUT_FILE_NAME, "wb");

    x264_picture_alloc(&in_pic, X264_CSP_I420, param.i_width, param.i_height);

    //´ò¿ª±àÂëÆ÷
    handle = x264_encoder_open(&param);
    int iSize = param.i_width * param.i_height;
    int uv = iSize / 4;
    int ret = 0;
    int j = 0;
    while (1) {

        //Y
        ret = fread(in_pic.img.plane[0], 1, iSize, in_fp);
        cout << "y = " << ret << endl;
        if (ret != iSize) {
            break; 
        }

        //U
        ret = fread(in_pic.img.plane[1], 1, uv, in_fp);
        cout << "u = " << ret << endl;
        if (ret != uv)
            break;

        //V
        ret = fread(in_pic.img.plane[2], 1, uv, in_fp);
        cout << "v = " << ret << endl;
        if (ret != uv)
            break; 

        in_pic.i_pts = j++;

        int iret = x264_encoder_encode(handle, &p_nal, &nal, &in_pic, &out_pic);

        for (int i = 0; i < nal; ++i) {
            fwrite(p_nal[i].p_payload, 1, p_nal[i].i_payload, out_fp);
        }

        printf("Succeed encode frame: %5d,in = %d, out = %d\n", j, iret, out_pic.i_dts);
    }

    while (x264_encoder_delayed_frames(handle)) {

        int iret = x264_encoder_encode(handle, &p_nal, &nal, NULL, &out_pic);

        for (int i = 0; i < nal; ++i) {
            fwrite(p_nal[i].p_payload, 1, p_nal[i].i_payload, out_fp);
        }
    }

    fclose(in_fp);
    fclose(out_fp);
    x264_encoder_close(handle);
    x264_picture_clean(&in_pic);
    //x264_picture_clean(&out_pic);

    cin.get();
    return 0;
}