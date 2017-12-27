void DemoSobelFrameHw(u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS], u32 fApp)
{
   int xcoi, ycoi;
   u32 frameWidth;
   
   ap_linebuffer<u8, 3, DEMO_WIDTH> buff_A; //create buffer 3xdemo_width(1920)
   ap_window<u8,3,3> buff_C; //create buffer for sliding window (3x3)
   
   //fApp = fApp | (((u32) videoCapt.timing.HActiveVideo) << 16)
   frameWidth = (fApp >> 16);
   
   for(ycoi = 0; ycoi < DEMO_HEIGHT+1; ycoi++){
       for(xcoi = 0; xcoi < DEMO_WIDTH+1; xcoi++){
#pragma AP PIPELINE II = 1
           u8 temp, grayIn = 0;
           u16 rIn, gIn, bIn;  //rgb values defined
           u16 pxlIn, pxlInOld = 0;  //pxl variables,pxlIn and pxlInOld
           u16 edge;  //for pixels on edge of screen
           
           if(xcoi < DEMO_WIDTH){
               buff_A.shift_up(xcoi);     //if pixel is valid, shift buffer up at xcoi
               temp = buff_A.getval(0,xcoi); //from buff_A get x coordinate from 1st row
           }
           if((xcoi < DEMO_WIDTH) & (ycoi < DEMO_HEIGHT)){
               pxlInOld = pxlIn;	//store old pixel into pxlInOld
               pxlIn = srcFrame[ycoi*DEMO_WIDTH+xcoi];  //take in new pxl
               rIn = ((pxlIn & 0xF800) >> (11-3)); //r
               bIn = ((pxlIn & 0x07C0) >> (6-3));  //b
               gIn = ((pxlIn & 0x003F) << 2);      //g
               grayIn = (rIn * 76 + gIn * 150 + bIn * 29 + 128) >> 8;  //greyscale pxlIn
               buff_A.insert_bottom(grayIn,xcoi);  //insert resulting pxl into A at bottom
           }
           buff_C.shift_right();  //shift buffer c to the right
           if(xcoi < DEMO_WIDTH){  //if valid xcoi
               buff_C.insert(buff_A.getval(2,xcoi),0,2); //insert 
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

