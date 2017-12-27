/*****************************************************************************
 *
 *     Author: Xilinx, Inc.
 *
 *     This text contains proprietary, confidential information of
 *     Xilinx, Inc. , is distributed by under license from Xilinx,
 *     Inc., and may be used, copied and/or disclosed only pursuant to
 *     the terms of a valid license agreement with Xilinx, Inc.
 *
 *     XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS"
 *     AS A COURTESY TO YOU, SOLELY FOR USE IN DEVELOPING PROGRAMS AND
 *     SOLUTIONS FOR XILINX DEVICES.  BY PROVIDING THIS DESIGN, CODE,
 *     OR INFORMATION AS ONE POSSIBLE IMPLEMENTATION OF THIS FEATURE,
 *     APPLICATION OR STANDARD, XILINX IS MAKING NO REPRESENTATION
 *     THAT THIS IMPLEMENTATION IS FREE FROM ANY CLAIMS OF INFRINGEMENT,
 *     AND YOU ARE RESPONSIBLE FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE
 *     FOR YOUR IMPLEMENTATION.  XILINX EXPRESSLY DISCLAIMS ANY
 *     WARRANTY WHATSOEVER WITH RESPECT TO THE ADEQUACY OF THE
 *     IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OR
 *     REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE FROM CLAIMS OF
 *     INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *     FOR A PARTICULAR PURPOSE.
 *
 *     Xilinx products are not intended for use in life support appliances,
 *     devices, or systems. Use in such applications is expressly prohibited.
 *
 *     (c) Copyright 2011 Xilinx Inc.
 *     All rights reserved.
 *
 *****************************************************************************/

/*
 * Sobel demo code 
 */

//questions for the GTA
//how much slower is it to use u32 than u16?-
//best way to detect a custom pattern-
//how to make the Guassian filter effective
//combine the parts - functions?
//hardware accelerating the functions - is this through vivado, how do we get the code after the HWA
//how to output instructions to the user? line?

//Implementation in hardware
//Overlaying image, massive number of pixels
//fatest way to do it
//rotational application
//our current error
//fixed point



//things to check in the lab
//red for hsv


#include <stdio.h>
#include "ap_video.h"
#include "video_demo.h"

#define ABS(x)          ((x>0)? x : -x)


void gaussiangreybin(u16 pxlIn, u16 &pxlOut, u32 xcoi, u32 ycoi, u16 srcFrame[DEMO_PIXELS]);


void DemoSobelFrameHw(u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS]){
    u32 xcoi, ycoi;

    for(ycoi = 0; ycoi < DEMO_HEIGHT; ycoi++){
        u16 blackcount1 = 0;
        u16 whitecount1 = 0;
        u16 blackcount2 = 0;
        u16 whitecount = 0;

        for(xcoi = 0; xcoi < DEMO_WIDTH; xcoi++){
            u16 pxlIn = 0;
            u16 pxlOut = 0;

            u16 patin, pxlpat2, pxlpat3;
            u16 patout;
            u16 ratio;
            u8 a = 1;
            u8 b = 2;
            u16 red = 0xF800;

            gaussiangreybin(pxlIn, pxlOut, xcoi, ycoi, srcFrame[DEMO_PIXELS]);
            patin = pxlOut;
            gaussiangreybin(pxlIn, pxlOut, (xcoi+1), ycoi, srcFrame[DEMO_PIXELS]);
            pxlpat2 = pxlOut;
            gaussiangreybin(pxlIn, pxlOut, (xcoi+2), ycoi, srcFrame[DEMO_PIXELS]);
            pxlpat3 = pxlOut;

            if( (patin + pxlpat2 + pxlpat3) >= 1){
                if( whitecount == 0){
                    blackcount1 += 1;
                }
                else if( whitecount != 0){
                    blackcount2 += 1;
                }
            }
            else{
                if(blackcount1 != 0 && blackcount2 == 0){
                    whitecount += 1;
                }
            }

            ratio = ((blackcount1 + blackcount2)*100)/whitecount;
            u8 rangeh = 220;
            u8 rangel = 180;
            if( rangel < ratio < rangeh){
                patout = red;
            }
            else{
                patout = patin;
            }

            destFrame[DEMO_PIXELS] = patout;

        }
    }
}


void gaussiangreybin(u16 pxlIn, u16 &pxlOut, u32 xcoi, u32 ycoi, u16 srcFrame[DEMO_PIXELS]){
    u16 r, g, b;
    short gray = 0;

    if( ycoi == 0 || xcoi == 0 || ycoi == (DEMO_HEIGHT-1) || xcoi == (DEMO_WIDTH-1)){ //leave edge pixel
        r = 0;
        g = 0;
        b = 0;
    }
    else{
        float weight = 0;
        int i, j;

        const float x_op[3][3] = { {0.077847,0.123317,0.077847},{0.123317,0.195346,0.123317},{0.077847,0.123317,0.077847}};  //defining two kernels that are applied to pixel rgb values - x-direction

        for(i=0; i < 3; i++){
            for(j = 0; j < 3; j++){
                pxlIn = srcFrame[j-1+xcoi+((i-1+ycoi) * DEMO_WIDTH)]; //go through surrounding pixels
                r = ((pxlIn & 0xF800) >> (11-3));  //to isolate r into 8 bits, & with 0xF800 then shift right
                b = ((pxlIn & 0x07C0) >> (6-3));   //isolating blue
                g = ((pxlIn & 0x003F) << 2);    //isolating green
                gray = (short) ((r * 76 + g * 150 + b * 29 + 128) >> 8);
                weight = weight + ((float)gray * x_op[i][j]);
            }
        }
        gray = (u16)weight;
    }

    //pxlOut = (((gray) & 0x00F8) << (11-3)) | (((gray) & 0x00F8) << (6-3)) | (((gray) & 0x00F8) >> 2); //apply filter
    u8 blackrangeh = 0x000F;
    u8 blackrangel = 0x000F;
    if( blackrangel < gray < blackrangeh){ //set threshold
        pxlOut = 1;
    }
    else{
        pxlOut = 0;
    }
}



/*****************************************************************************
 *
 *
 * Sobel demo code 
 */





// #include <stdio.h>
// #include "ap_video.h"
// #include "video_demo.h"

// #define ABS(x)          ((x>0)? x : -x)

// void DemoSobelFrameHw(u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS]){
//     u32 xcoi, ycoi;

//     for(ycoi = 0; ycoi < DEMO_HEIGHT; ycoi++){
//         for(xcoi = 0; xcoi < DEMO_WIDTH; xcoi++){
//             u16 pxlIn, pxlOut;
//             short gray = 0;

//             pxlIn = srcFrame[xcoi + ycoi * DEMO_WIDTH];
//             u16 r, g, b;

//             if( ycoi == 0 || xcoi == 0 || ycoi == (DEMO_HEIGHT-1) || xcoi == (DEMO_WIDTH-1)){  //if pixel is at edge of screen leave
//                 r = 0;
//                 g = 0;
//                 b = 0;
//             }
//             else{
//                 short x_weight = 0;  //initialise two variables that hold the weighting in x and y axises
//                 float weight = 0;
//                 float g_weight = 0;
//                 float b_weight = 0;
//                 int i, j;

//                 const float x_op[3][3] = { {0.077847,0.123317,0.077847},{0.123317,0.195346,0.123317},{0.077847,0.123317,0.077847}};  //defining two kernels that are applied to pixel rgb values - x-direction

//                 for(i=0; i < 3; i++){
//                     for(j = 0; j < 3; j++){
//                         pxlIn = srcFrame[j-1+xcoi+((i-1+ycoi) * DEMO_WIDTH)]; //go through surrounding pixels
//                         r = ((pxlIn & 0xF800) >> (11-3));  //to isolate r into 8 bits, & with 0xF800 then shift right
//                         b = ((pxlIn & 0x07C0) >> (6-3));   //isolating blue
//                         g = ((pxlIn & 0x003F) << 2);    //isolating green
//                         gray = (short) ((r * 76 + g * 150 + b * 29 + 128) >> 8);
//                         weight = weight + ((float)gray * x_op[i][j]);
//                     }
//                 }
//                 gray = (u16)weight;
//             }

//         pxlOut = (((gray) & 0x00F8) << (11-3)) | (((gray) & 0x00F8) << (6-3)) | (((gray) & 0x00F8) >> 2); //apply filter
//         destFrame[xcoi + ycoi * DEMO_WIDTH] = pxlOut;
//         }
//     }
// }



// void RGB2HSV(u16 pxlIn, u16 &pxlOut){

//     u16 r, g, b;
//     u16 min, max, delta;
//     u16 h,s,v;

//     r = ((pxlIn & 0xF800) >> (11-3));
//     b = ((pxlIn & 0x07C0) >> (6-3));
//     g = ((pxlIn & 0x003F) << 2);


//     //min calc
//     if(r<g && r<b){
//         min=r;
//     }
//     else if(g<r && g<b){
//         min=g;
//     }
//     else{
//         min=b;
//     }
//     //max calc
//     if(r>g && r>b){
//         max=r;
//     }
//     else if(g>r && g>b){
//         max=g;
//     }
//     else{
//         max=b;
//     }

//     v = max;
//     delta = max - min;
//     if( v == 0){
//         h = 0;
//         s = 0;
//     }
//     s = (255*delta)/v;
//     else if( s == 0) {
//         h = 0;
//     }
//     else{
//         if( r == max){
//             h = (43*(g-b))/delta;
//         }
//         else if( b == max){
//             h = 171 + ((43*(r-g))/delta);
//             //h = (((((r-g)*1000)/delta) + 4000)/1000)* 60;
//             //h = (h*255)/360;
//         }
//         else{
//             h = 85 + ((43*(b-r))/delta);
//             //h = (((((b-r)*1000)/delta) + 4000)/1000)* 60;
//             //h = (h*255)/360;   
//         }

//     }
//     //s = (((delta*1000)/max)*255)/100000;
//     pxlOut = (((h + 4) & 0x00F8) << (11-3)) | (((s + 4) & 0x00F8) << (6-3)) | (((v + 4) & 0x00F8) >> 2);

//     return;

// }

// void gaussiangreybin(u16 pxlIn, u16 &pxlOut){
//     u16 r, g, b;
            
//     if( ycoi == 0 || xcoi == 0 || ycoi == (DEMO_HEIGHT-1) || xcoi == (DEMO_WIDTH-1))  //if pixel is at edge of screen leave
//         r = 0;
//         g = 0;
//         b = 0;
//     else{
//         short x_weight = 0;  //initialise two variables that hold the weighting in x and y axises
//         u16 r_weight = 0;
//         u16 g_weight = 0;
//         u16 b_weight = 0;
//         int i, j;

//         u16 pxlIn;
//         short gray;
//         const short x_op[3][3] = { {1,2,1},{2,4,2},{1,2,1}};  //defining two kernels that are applied to pixel rgb values - x-direction
        
//         for(i=0; i < 3; i++){
//             for(j = 0; j < 3; j++){
//                 pxlIn = srcFrame[j-1+xcoi+((i-1+ycoi) * DEMO_WIDTH)]; //go through surrounding pixels
//                 r = ((pxlIn & 0xF800) >> (11-3));  //to isolate r into 8 bits, & with 0xF800 then shift right
//                 b = ((pxlIn & 0x07C0) >> (6-3));   //isolating blue
//                 g = ((pxlIn & 0x003F) << 2);    //isolating green
//                 r_weight = r_weight + (r * x_op[i][j]); 
//                 g_weight = g_weight + (g * x_op[i][j]);
//                 b_weight = b_weight + (b * y_op[i][j]);
//             }
//         }

//         r = r_weight/9;
//         g = g_weight/9;
//         b = b_weight/9;
//     }
    
//     pxlOut = (((r) & 0x00F8) << (11-3)) | (((b) & 0x00F8) << (6-3)) | (((g) & 0x00F8) >> 2); //apply filter

//  }


// #include <stdio.h>
// #include "ap_video.h"
// #include "video_demo.h"

// #define ABS(x)          ((x>0)? x : -x)

// void DemoSobelFrameHw(u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS]){
//     u32 xcoi, ycoi;

//     for(ycoi = 0; ycoi < DEMO_HEIGHT; ycoi++){
//         for(xcoi = 0; xcoi < DEMO_WIDTH; xcoi++){
//             u16 pxlIn, pxlOut;

//             pxlIn = srcFrame[xcoi + ycoi * DEMO_WIDTH];
//                      u16 r, g, b;

//             if( ycoi == 0 || xcoi == 0 || ycoi == (DEMO_HEIGHT-1) || xcoi == (DEMO_WIDTH-1)){  //if pixel is at edge of screen leave
//                 r = 0;
//                 g = 0;
//                 b = 0;
//             }
//             else{
//                 short x_weight = 0;  //initialise two variables that hold the weighting in x and y axises
//                 float r_weight = 0;
//                 float g_weight = 0;
//                 float b_weight = 0;
//                 int i, j;

//                 u16 pxlIn;
//                 short gray;
//                 const float x_op[3][3] = { {0.077847,0.123317,0.077847},{0.123317,0.195346,0.123317},{0.077847,0.123317,0.077847}};  //defining two kernels that are applied to pixel rgb values - x-direction

//                 for(i=0; i < 3; i++){
//                     for(j = 0; j < 3; j++){
//                         pxlIn = srcFrame[j-1+xcoi+((i-1+ycoi) * DEMO_WIDTH)]; //go through surrounding pixels
//                         r = ((pxlIn & 0xF800) >> (11-3));  //to isolate r into 8 bits, & with 0xF800 then shift right
//                         b = ((pxlIn & 0x07C0) >> (6-3));   //isolating blue
//                         g = ((pxlIn & 0x003F) << 2);    //isolating green
//                         r_weight = r_weight + ((float)r * x_op[i][j]);
//                         g_weight = g_weight + ((float)g * x_op[i][j]);
//                         b_weight = b_weight + ((float)b* x_op[i][j]);
//                     }
//                 }

//                 r = (u16)r_weight;
//                 g = (u16)g_weight;
//                 b = (u16)b_weight;
//             }

//         pxlOut = (((r) & 0x00F8) << (11-3)) | (((b) & 0x00F8) << (6-3)) | (((g) & 0x00F8) >> 2); //apply filter
//         destFrame[xcoi + ycoi * DEMO_WIDTH] = pxlOut;
//         }
//     }
// }

// void Binarise(u16 pxlIn, u16 &pxlOut){

//     u16 count;

//     if ( 0xFFF< pxlIn < 0xFFFF){
//         pxlOut = 1;
//     }
//     else{
//         pxlOut = 0;
//     }
// }



void DemoSobelFrameHw(u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS], u32 fApp)
{
   int xcoi, ycoi;
   u32 frameWidth;
   
   ap_linebuffer<u8, 3, DEMO_WIDTH> buff_A;
   ap_window<u8,3,3> buff_C;
   
   frameWidth = (fApp >> 16);
   
   for(ycoi = 0; ycoi < DEMO_HEIGHT+1; ycoi++){
       for(xcoi = 0; xcoi < DEMO_WIDTH+1; xcoi++){
#pragma AP PIPELINE II = 1
           u8 temp, grayIn = 0;
           u16 rIn, gIn, bIn;
           u16 pxlIn, pxlInOld = 0;
           u16 edge;
           
           if(xcoi < DEMO_WIDTH){
               buff_A.shift_up(xcoi);
               temp = buff_A.getval(0,xcoi);
           }
           if((xcoi < DEMO_WIDTH) & (ycoi < DEMO_HEIGHT)){
               pxlInOld = pxlIn;
               pxlIn = srcFrame[ycoi*DEMO_WIDTH+xcoi];
               rIn = ((pxlIn & 0xF800) >> (11-3));
               bIn = ((pxlIn & 0x07C0) >> (6-3));
               gIn = ((pxlIn & 0x003F) << 2);
               grayIn = (rIn * 76 + gIn * 150 + bIn * 29 + 128) >> 8;
               buff_A.insert_bottom(grayIn,xcoi);
           }
           buff_C.shift_right();
           if(xcoi < DEMO_WIDTH){
               buff_C.insert(buff_A.getval(2,xcoi),0,2);
               buff_C.insert(temp,1,2);
               buff_C.insert(grayIn,2,2);
           }
           
           if( ycoi <= 1 || xcoi <= 0 || ycoi == DEMO_HEIGHT || xcoi == DEMO_WIDTH)
               edge=0;
           else{
               short x_weight = 0, y_weight = 0;
               u8 i, j;
               const short x_op[3][3] = { {-1,0,1},{-2,0,2},{-1,0,1}};
               const short y_op[3][3] = { {1,2,1},	{0,0,0}, {-1,-2,-1}};
               
               for(i=0; i < 3; i++){
                   for(j = 0; j < 3; j++){
                       x_weight = x_weight + (buff_C.getval(i,j) * x_op[i][j]);
                       y_weight = y_weight + (buff_C.getval(i,j) * y_op[i][j]);
                   }
               }
               edge = ABS(x_weight) + ABS(y_weight);
               if(edge > 200)	edge = 255;
               else if(edge < 100)		edge = 0;
           }
           
           if(ycoi > 0 && xcoi > 0){
               if (fApp & 0x01)
               {
                   if (xcoi < (frameWidth / 2))
                       destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = pxlInOld;
                   else
                       destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = (((buff_C.getval(1,1)) & 0x00F8) << (11-3)) | (((buff_C.getval(1,1)) & 0x00F8) << (6-3)) | (((buff_C.getval(1,1)) & 0x00F8) >> 2);
               }
               else
               {
                   if (xcoi < (frameWidth / 2))
                       destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = pxlInOld;
                   else
                       destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = (((edge) & 0x00F8) << (11-3)) | (((edge) & 0x00F8) << (6-3)) | (((edge) & 0x00F8) >> 2);
               }
           }
       }
   }
}

