#include <stdio.h>
#include "ap_video.h"
#include "video_demo.h"
#include "ap_int.h"

void DemoSobelFrameHw(u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS]){
    int xcoi, ycoi;
    int pxl[1280];
    int pxl2[1280];
    u8 check = 0;


    for(ycoi = 0; ycoi < 720; ycoi++){
        u16 binA = 0;
        u16 binB = 0;
        u16 binAA = 0;
        u16 binBB = 0;
        pxl2[0] = 0;
        pxl2[1] = 0;
        pxl2[1278] = 0;
        pxl2[1279] = 0;


        for(xcoi = 0; xcoi < 1280; xcoi++){
            u16 pxlIn;
            u16 r,g,b;

            if( ycoi == 0){
                pxl[xcoi] = 0;
            }
            else{
                pxlIn = srcFrame[xcoi+ycoi * DEMO_WIDTH];
                r = ((pxlIn & 0xF800) >> (11-3));
                b = ((pxlIn & 0x07C0) >> (6-3));
                g = ((pxlIn & 0x003F) << 2);

                //RBG2HSV(pxlIn,pxlOut,h,s,v);  //HSV calculation
                //output previous row
                if( check == 1 && pxl2[xcoi] == 1  ){
                    destFrame[xcoi + (ycoi-1) * DEMO_WIDTH] = 0xF800;
                }
                else if( check ==1 && pxl2[xcoi] == 2){
                    destFrame[xcoi + (ycoi-1) * DEMO_WIDTH] = 0x00FF;
                }
                else{
                    destFrame[xcoi + (ycoi-1) * DEMO_WIDTH] = srcFrame[xcoi + (ycoi-1) * DEMO_WIDTH];
                }




                if( r <= 50 && 30 <= r && 180 <= b && b <= 200 && 210 <= g ){
                    if( (binB > 0 && binA > 0) || binB == 0){
                        pxl[xcoi] = 1;
                        binA++;
                    }
                    else{
                        pxl[xcoi] = 0;
                    }
                }
                else if( 200 <= r && 180 <= b && g <= 15 && binA > 0 ){
                    if(binA > 0){
                        pxl[xcoi] = 2;
                    }
                    else{
                        pxl[xcoi] = 0;
                    }
                }
                else{
                    pxl[xcoi] = 0;
                }


                if(binA > 0 && binB > 0 && xcoi > 5 && xcoi < 1278){
                    if( pxl[xcoi-2] == 1){
                        int sum = 0;

                        sum += pxl[xcoi-4];
                        sum += pxl[xcoi-3];
                        sum += pxl[xcoi-1];
                        sum += pxl[xcoi];

                        if(sum > 2){
                            pxl2[xcoi-2] = 1;
                            binAA++;
                        }
                        else{
                            pxl2[xcoi-2] = 0;
                        }
                    }
                    else if(pxl[xcoi-2] == 2){
                        int sum = 0;

                        sum += pxl[xcoi-4];
                        sum += pxl[xcoi-3];
                        sum += pxl[xcoi-1];
                        sum += pxl[xcoi];

                        if(sum > 3){
                            pxl2[xcoi-2] = 2;
                            binBB++;
                        }
                        else{
                            pxl2[xcoi-2] = 0;
                        }

                    }
                    else{
                        pxl2[xcoi-2] = 0;
                    }
                }
            }
        }

        if(binAA + binBB > 10){
            int ratio = 0;
            ratio = (binAA << 6)/binBB;
            if( 110 <= ratio && ratio <= 140 ){
                check = 1;
            }
            else{
                check = 0;
            }
        }
        else{
            check = 0;
        }
    }
}
