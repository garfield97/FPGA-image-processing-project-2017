#include <stdio.h>
#include "ap_video.h"
#include "video_demo.h"

#define ABS(x)          ((x>0)? x : -x)

void DemoSobelFrameHw(u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS], u32 fApp)
{
	int xcoi, ycoi;
	u32 frameWidth;
	int pxl[DEMO_WIDTH];
	int pxl2[DEMO_WIDTH];
	int check = 0;

	frameWidth = (fApp >> 16);

	for(ycoi = 0; ycoi < DEMO_HEIGHT+1; ycoi++){
		u16 bin A = 0;
		u16 bin B = 0;
		u16 bin AA = 0;
		u16 bin BB = 0;


		for(xcoi = 0; xcoi < DEMO_WIDTH+1; xcoi++){
#pragma HLS PIPELINE II = 1
			u16 rIn, gIn, bIn;
			u16 pxlIn;

			if(ycoi == 0){
				pxl[xcoi] = 0;
				pxl2[xcoi] = 0;
				
			}
			else{
				pxlIn = srcFrame[ycoi*DEMO_WIDTH+xcoi];
				rIn = ((pxlIn & 0xF800) >> (11-3));
				bIn = ((pxlIn & 0x07C0) >> (6-3));
				gIn = ((pxlIn & 0x003F) << 2);

                if( check == 1 && pxl2[xcoi] == 1  ){
                    destFrame[(ycoi-1) * DEMO_WIDTH + xcoi] = 0xF800;
                }
                else if( check ==1 && pxl2[xcoi] == 2){
                    destFrame[(ycoi-1) * DEMO_WIDTH + xcoi] = 0x00FF;
                }
                else{
                    destFrame[(ycoi-1) * DEMO_WIDTH + xcoi] = srcFrame[(ycoi-1) * DEMO_WIDTH + xcoi];
                }

                if( binB == 0  ){
                    if( rIn <= 50 && 30 <= rIn && 180 <= bIn && bIn <= 200 && 210 <= gIn ){ //hl <= h <= hh && vl <= v <= vh && sh <= s <= sh ){
                        pxl[xcoi] = 1;
                        binA ++;
                    }
                    else if( 200 <= rIn && 180 <= bIn && gIn <= 15 && binA > 0 ){ //&& binA > 0){
                        pxl[xcoi] = 2;
                        binB++;
                    }
                    else{
                        pxl[xcoi] = 0;
                    }
                }
                else if( binA > 0  && binB > 0 ){
                    if( 200 <= rIn && 180 <= bIn && gIn <= 15){
                        pxl[xcoi] = 2;
                        binB++;
                    }
                    else{
                        pxl[xcoi] = 0;
                    }
                }
                else{
                    pxl[xcoi] = 0;
                }
			}
		}

		pxl2[0] = 0;
        pxl2[1] = 0;
        pxl2[DEMO_WIDTH - 2] = 0;
        pxl2[DEMO_WIDTH - 1] = 0;

        if( binA  + binB > 0){
            for(int i = 2; i < DEMO_WDTH - 2; i++){
#pragma HLS PIPELINE II = 1
                if( pxl[i] == 1){
                    int sum = 0;

                    sum += pxl[i-2];
                    sum += pxl[i-1];
                    sum += pxl[i + 1];
                    sum += pxl[i + 2];

                    if(sum > 2){
                        pxl2[i] = 1;
                        binAA++;
                    }
                    else{
                        pxl2[i] = 0;
                    }
                }
                else if(pxl[i] == 2){
                    int sum = 0;

                    sum += pxl[i-2];
                    sum += pxl[i-1];
                    sum += pxl[i + 1];
                    sum += pxl[i + 2];

                    if(sum > 3){
                        pxl2[i] = 2;
                        binBB++;
                    }
                    else{
                        pxl2[i] = 0;
                    }

                }
                else{
                    pxl2[i] = 0;
                }
            }
            u16 ratio = 0;
            ratio = (binAA * 64)/binBB;
            if( 40 <= ratio && ratio <= 78 ){
                check = 1;
            }
        }
        else{
            check = 0;
        }
    }
}