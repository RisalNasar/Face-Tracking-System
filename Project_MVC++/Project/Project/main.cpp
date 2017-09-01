


#include <afx.h>
#include <stdio.h>
#include "cv.h"
#include "highgui.h"
 
int enable_port;
int skin_detection_method;
int print_to_port;
char pan[5],tilt[5];

int center_x,center_y;
int pan_value, tilt_value;

int Hmin = 96;	// Room
//int Hmin = 70;		// DSP Lab
int Hmax = 115;
int Ymin = 23;
int Ymax = 105;
int Crmin = 88;
int Crmax = 156;
int Cbmin = 109;
int Cbmax = 168;
int Redmin = 0;
int Redmax = 185;
int Greenmin = 0;
int Greenmax = 185;
int Bluemin = 0;
int Bluemax = 185;

IplImage *frame			= NULL;
IplImage *tpl			= NULL;
IplImage *tm			= NULL;
IplImage *tpl2			= NULL;
IplImage *pyr			= NULL;




int object_x0, object_y0;
int state = 1;

int frameW,frameH,frameR;
int pos_x1,pos_y1,pos_x2,pos_y2;
int win_x0,win_y0;

int tpl_width, tpl_height, window_width, window_height;


int accept_mouse_input;


double minval,maxval;
CvPoint minloc, maxloc;


 CvRect face;
int width_difference = 100;
int height_difference = 100;


 // Declarations of functions
int detect_faces(IplImage*,CvHaarClassifierCascade*,int);
void GetSkinMask(IplImage*, IplImage*, int, int);
void mouseHandler( int event, int x, int y, int flags, void *param );
void trackObject(IplImage*);
void calculate_cordinates();
void calculate_pan_and_tilt();
void convert_values();



//................................................. main code ..........................................................................
int main( int argc, char** argv )
{
	enable_port = 1;
	// Enable port
	// 0 = Disabled
	// 1 = Enabled

	skin_detection_method = 1;
	// Skin Detection Method
	// 0 = No Skin Detection
	// 1 = HSV
	// 2 = YCbCr1
	// 3 = RGB


	HANDLE hPort;
	DWORD byteswritten;
	DCB dcb;
	CString PortSpecifier;
	bool port_opened;

	if(enable_port)
	{
		// Initializing the Port
		PortSpecifier = "\\\\.\\COM4";
		hPort = CreateFile(
			PortSpecifier,
			GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			0,
			NULL
			);
		port_opened = true;

		if (!GetCommState(hPort,&dcb)){
			port_opened = false;
		} 
		else{
			dcb.BaudRate = CBR_9600; //9600 Baud
			dcb.ByteSize = 8; //8 data bits
			dcb.Parity = NOPARITY; //no parity
			dcb.StopBits = ONESTOPBIT; //1 stop
		}
		if (!SetCommState(hPort,&dcb)){
			port_opened = false;
		}

		if(!port_opened){
			printf("Port could not be opened!\n");
			return 3;
		}
		else{
			printf("Port opened\n");
		}
	}

    CvCapture   *capture;
    int key = 0;
	int count_no_haar_detection = 0;
	int count_template_matching_rounds = 0;
	int confirm_face = 0;


	IplImage *frame_sk			= NULL;
	IplImage *frame_mask_bw		= NULL;
	accept_mouse_input = 0;
	tpl_width = tpl_height = window_width = window_height = 0;
	int selection = 0;

	pan_value = 750;
	//tilt_value = 800;
	tilt_value = 725;

	char *cascade_name;
	//cascade_name = "haarcascade_frontalface_alt.xml";
	cascade_name = "haarcascade_frontalface_alt_tree.xml";
	CvHaarClassifierCascade *cascade;

 
    // initialize camera 
	capture = cvCreateCameraCapture(0);


    // always check 
    if( !capture ) return 1;

    // set video properties, needed by template image 
 	cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, 1600 );
	cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, 900 );
	cvSetCaptureProperty( capture, CV_CAP_PROP_FPS, 30);

	frameH    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
	frameW    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	frameR	  = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
	printf("frameW: %d	frameH: %d	frameR: %d	\n",frameW,frameH,frameR);

	//frameH = frameH*2;
	//frameW = frameW*2;
	


    frame = cvQueryFrame( capture );
    if ( !frame ) return 2;

	//CvSize sz = cvSize( frame->width & -2, frame->height & -2);
	//if(pyr != NULL)
	//	cvReleaseImage(&pyr);
	//pyr = cvCreateImage( cvSize(sz.width*2, sz.height*2), 8, 3 );

	//cvPyrUp( frame, pyr, CV_GAUSSIAN_5x5 );
	//frame = pyr;

	if(skin_detection_method != 0){
		if(frame_sk != 0)
			cvReleaseImage(&frame_sk);
		frame_sk = cvCreateImage(cvGetSize(frame),8,3);

		if(frame_mask_bw != NULL)
			cvReleaseImage(&frame_mask_bw);
		frame_mask_bw = cvCreateImage(cvGetSize(frame),8,3);
	}
	
	cascade = (CvHaarClassifierCascade*)cvLoad(cascade_name,0,0,0);
	// Load the Haar Cascade   

   
    // create a window and install mouse handler 


	if(skin_detection_method != 0){
		cvNamedWindow( "video_sk",CV_WINDOW_AUTOSIZE);
		if(skin_detection_method == 1){
			cvCreateTrackbar( "Hmin", "video_sk", &Hmin, 256, 0 );
			cvCreateTrackbar( "Hmax", "video_sk", &Hmax, 256, 0 );
		}
		else if(skin_detection_method == 2){
			cvCreateTrackbar( "Ymin", "video_sk", &Ymin, 256, 0 );
			cvCreateTrackbar( "Ymax", "video_sk", &Ymax, 256, 0 );
			cvCreateTrackbar( "Cbmin", "video_sk", &Cbmin, 256, 0 );
			cvCreateTrackbar( "Cbmax", "video_sk", &Cbmax, 256, 0 );
			cvCreateTrackbar( "Crmin", "video_sk", &Crmin, 256, 0 );
			cvCreateTrackbar( "Crmax", "video_sk", &Crmax, 256, 0 );
	
		}
		else if(skin_detection_method == 3){
			cvCreateTrackbar( "Redmin", "video_sk", &Redmin, 256, 0 );
			cvCreateTrackbar( "Redmax", "video_sk", &Redmax, 256, 0 );
			cvCreateTrackbar( "Greenmin", "video_sk", &Greenmin, 256, 0 );
			cvCreateTrackbar( "Greenmax", "video_sk", &Greenmax, 256, 0 );
			cvCreateTrackbar( "Bluemin", "video_sk", &Bluemin, 256, 0 );
			cvCreateTrackbar( "Bluemax", "video_sk", &Bluemax, 256, 0 );
	
		}

	}
	cvNamedWindow("tpl",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("tpl2",CV_WINDOW_AUTOSIZE);

    cvNamedWindow( "video", CV_WINDOW_AUTOSIZE );
    cvSetMouseCallback( "video", mouseHandler, NULL );

			calculate_cordinates();
			calculate_pan_and_tilt();
			convert_values();

			if(enable_port)
			{	

				print_to_port = WriteFile(hPort,pan,strlen(pan),&byteswritten,NULL);

				print_to_port = WriteFile(hPort,tilt,strlen(tilt),&byteswritten,NULL);
				
			}



//.....................................................while loop ........................................................................   
    while( key != 27 ) {
		
		if(key == '0'){
			selection = 0;
			printf("Normal Program Enabled\n");

		}
		else if(key == '2'){
			selection = 2;
			printf("Face Detection on ROI only, enabled \n");		
		}
		else if(key == '1'){
			selection = 1;
			printf("Face Detection on Frame Enabled\n");		
		}
		else if(key == '3'){
			selection = 3;
			printf("Manual control of Pan - Tilt system enabled.");
			printf("Press 'a','s','d','f' to control.\n");
			printf("Press '0' to exit from this mode.\n");
		
		}
		else if(key == '4'){
			printf("Enter password : \n");

			if(cvWaitKey(0)=='c')
				if(cvWaitKey(0)=='a')
					if(cvWaitKey(0) == 'l'){
						selection = 4;
						printf("Calibration mode of Pan - Tilt system is enabled.");
						printf("Press 'a','s','d','f','g' to calibrate Pan");
						printf(" and 'z','x','c','v','b' to calibrate Tilt\n");
						printf("Press '0' to exit from this mode.\n");
					}
			if(selection != 4)
				printf("Incorrect Password ... !\n");
			
				
		}
		else if(key == '5'){
			selection = 5;
			printf("Template Matching only, enabled.\n");
		
		}
		else if(key == 'b'){
			if(enable_port)
			{	
				pan_value = 750;
				tilt_value = 735;
				convert_values();
				print_to_port = WriteFile(hPort,pan,strlen(pan),&byteswritten,NULL);

				print_to_port = WriteFile(hPort,tilt,strlen(tilt),&byteswritten,NULL);
				
			}
		
		}

		if(selection == 1){
			state = 1;

		}
		else if(selection == 2){
			state = 3;

		}
		else if(selection == 5){
			state = 4;
		
		}
		else if(selection == 3){
			state = 0;
		}
		else if(selection == 4){
			state = 0;
		}
		else if(selection == 0){
			
		}




        // get a frame 
		/*
        frame = cvQueryFrame( capture );
        if( !frame ) break;

		*/

		cvGrabFrame( capture );
		frame = cvRetrieveFrame(capture);
		if(!frame) break;




		

		//if(pyr != NULL)
		//	cvReleaseImage(&pyr);
		//pyr = cvCreateImage( cvSize(sz.width*2, sz.height*2), 8, 3 );

		//cvPyrUp( frame, pyr, CV_GAUSSIAN_5x5 );
		//frame = pyr;
//....................................................accept mouse input....................................................................
		if(accept_mouse_input == 1 ) {

			object_x0 = pos_x1;
			object_y0 = pos_y1;


			tpl_width = pos_x2 - pos_x1;
			tpl_height = pos_y2 - pos_y1;

			// For cvPyrdown() and cvPyrUp() in the Skin Detection process
			if(tpl_width%2 == 1)
				tpl_width--;
			if(tpl_height%2 == 1)
				tpl_height--;



			window_width = tpl_width + width_difference;
			window_height = tpl_height+ height_difference;



			win_x0 = object_x0 - ( ( window_width  - tpl_width  ) / 2 );
			win_y0 = object_y0 - ( ( window_height - tpl_height ) / 2 );
   
			if(win_x0<=0)
				win_x0 = 0;
			if(win_y0<0)
				win_y0 = 0;
			if(window_width>frameW)
				window_width = frameW;
			if(window_height> frameH)
				window_height = frameH ;
			if((win_x0+window_width)> frameW)
				win_x0 = frameW-window_width;
			if((win_y0+window_height)>frameH)
				win_y0 = (frameH-window_height);

					
			cvSetImageROI( frame,
                       cvRect( object_x0,
                               object_y0,
                               tpl_width,
                               tpl_height ) );

			// create template image 
			if(tpl != NULL)
				cvReleaseImage(&tpl);
			tpl = cvCreateImage( cvSize( tpl_width, tpl_height ),
				frame->depth, frame->nChannels );
		    cvCopy( frame, tpl, NULL );



			cvResetImageROI( frame );

			// template is available, start tracking! 
			fprintf( stdout, "Template selected. Start tracking... \n" );
			state = 4;
			accept_mouse_input = 0;
		}
 
//............................................................state = 1................................................................
        // perform tracking if template is available 
		if(state == 1){

			int detect_face_result_temp = 0;
			if(skin_detection_method != 0){
				GetSkinMask(frame, frame_mask_bw,0, 0);
				cvAnd(frame,frame_mask_bw,frame_sk);

				detect_face_result_temp = detect_faces(frame_sk,cascade,1);

			}
			else
				detect_face_result_temp = detect_faces(frame,cascade,1);

			if(detect_face_result_temp == 1){
				object_x0 = face.x;
				object_y0 = face.y;


				tpl_width = face.width;
				tpl_height = face.height;

				window_width = tpl_width + width_difference;
				window_height = tpl_height+ height_difference;

				win_x0 = object_x0 - ( ( window_width  - tpl_width  ) / 2 );
				win_y0 = object_y0 - ( ( window_height - tpl_height ) / 2 );
   
	
				if(win_x0<=0)
					win_x0 = 0;
				if(win_y0<0)
					win_y0 = 0;
				if(window_width>frameW)
					window_width = frameW;
				if(window_height> frameH)
					window_height = frameH ;
				if((win_x0+window_width)> frameW)
					win_x0 = frameW-window_width;
				if((win_y0+window_height)>frameH)
					win_y0 = (frameH-window_height);


				cvSetImageROI( frame,
                       cvRect( object_x0,
                               object_y0,
                               tpl_width,
                               tpl_height ) );

				// create template image 
				if(tpl != NULL)
					cvReleaseImage(&tpl); // Release image to free memory
				tpl = cvCreateImage( cvSize( tpl_width, tpl_height ),
					frame->depth, frame->nChannels );
				cvCopy( frame, tpl, NULL );
				cvResetImageROI( frame);


				cvRectangle( frame, cvPoint(face.x,face.y),
						cvPoint(face.x+face.width,face.y+face.height),
						CV_RGB(255,0,0), 2);

				if(skin_detection_method != 0){
					cvRectangle( frame_sk, cvPoint(face.x,face.y),
						cvPoint(face.x+face.width,face.y+face.height),
						CV_RGB(255,0,0), 2);	
				}
			
				state = 2;
			}
		}


//......................................................state = 2......................................................................
		else if( state == 2 ) {

			int detect_face_result_temp = 0;


			cvSetImageROI( frame,
                   cvRect( win_x0,
                           win_y0,
                           window_width,
                           window_height ) );

			if(tpl2 != NULL)
					cvReleaseImage(&tpl2);
			tpl2 = cvCreateImage( cvSize( window_width, window_height ),
				frame->depth,
				frame->nChannels );
			cvCopy( frame, tpl2, NULL );
			cvResetImageROI( frame );	

			detect_face_result_temp = detect_faces(tpl2,cascade,1);

			if(detect_face_result_temp == 1){
		
				object_x0 = win_x0 + face.x;
				object_y0 = win_y0 + face.y;

				tpl_width = face.width;
				tpl_height = face.height;

				cvSetImageROI( frame,
                       cvRect( object_x0,
                               object_y0,
                               tpl_width,
                               tpl_height ) );

				// create template image 
				if(tpl != NULL)
					cvReleaseImage(&tpl);
				tpl = cvCreateImage( cvSize( tpl_width, tpl_height ),frame->depth,
					frame->nChannels );
				cvCopy( frame, tpl, NULL );
				cvResetImageROI( frame );
				



				cvRectangle( frame, cvPoint(object_x0,object_y0),
						cvPoint(object_x0 + tpl_width,object_y0 + tpl_height),
						CV_RGB(255,0,0), 2);
				cvRectangle(frame,
						cvPoint(win_x0,win_y0),
						cvPoint(win_x0+window_width, win_y0+window_height),
						cvScalar(255,0,0,0),2);

				cvRectangle( tpl2, cvPoint(face.x,face.y),
						cvPoint(face.x+face.width,face.y+face.height),
						CV_RGB(255,0,0), 2);

				window_width = tpl_width + width_difference;
				window_height = tpl_height+ height_difference;

				win_x0 = object_x0 - ( ( window_width  - tpl_width  ) / 2 );
				win_y0 = object_y0 - ( ( window_height - tpl_height ) / 2 );
   
	
				if(win_x0<=0)
					win_x0 = 0;
				if(win_y0<0)
					win_y0 = 0;
				if(window_width>frameW)
					window_width = frameW;
				if(window_height> frameH)
					window_height = frameH ;
				if((win_x0+window_width)> frameW)
					win_x0 = frameW-window_width;
				if((win_y0+window_height)>frameH)
					win_y0 = (frameH-window_height);

				confirm_face++;
				if(confirm_face == 1){
					state = 3;
					confirm_face = 0;
				}

				

			}
			else {
				state = 1;
				confirm_face = 0;
			}
		
		}
		
//.......................................................state = 3 .................................................................
		else if( state == 3  ){

			int detect_face_result_temp = 0;
			
			cvSetImageROI( frame,
                   cvRect( win_x0,
                           win_y0,
                           window_width,
                           window_height ) );

			if(tpl2 != NULL)
				cvReleaseImage(&tpl2);
			tpl2 = cvCreateImage( cvSize( window_width, window_height ),
				frame->depth, frame->nChannels );
			cvCopy( frame, tpl2, NULL );
			cvResetImageROI( frame );	


			detect_face_result_temp = detect_faces(tpl2,cascade,1);

			if(detect_face_result_temp == 1){
		
			//if(0){
		
				object_x0 = win_x0 + face.x;
				object_y0 = win_y0 + face.y;


				tpl_width = face.width;
				tpl_height = face.height;


					
				cvSetImageROI( frame,
                       cvRect( object_x0,
                               object_y0,
                               tpl_width,
                               tpl_height ) );

				// create template image 
				if(tpl != NULL)
					cvReleaseImage(&tpl); // Release image to free memory
				tpl = cvCreateImage( cvSize( tpl_width, tpl_height ),
					frame->depth, frame->nChannels );
				cvCopy( frame, tpl, NULL );
				cvResetImageROI( frame );




				cvRectangle( frame, cvPoint(object_x0,object_y0),
						cvPoint(object_x0 + tpl_width,object_y0 + tpl_height),
						CV_RGB(255,0,0), 2);
				cvRectangle(frame,
						cvPoint(win_x0,win_y0),
						cvPoint(win_x0+window_width, win_y0+window_height),
						cvScalar(255,0,0,0),2);

				cvRectangle( tpl2, cvPoint(face.x,face.y),
						cvPoint(face.x+face.width,face.y+face.height),
						CV_RGB(255,0,0), 2);

				window_width = tpl_width + width_difference;
				window_height = tpl_height+ height_difference;

				win_x0 = object_x0 - ( ( window_width  - tpl_width  ) / 2 );
				win_y0 = object_y0 - ( ( window_height - tpl_height ) / 2 );
   
	
				if(win_x0<=0)
					win_x0 = 0;
				if(win_y0<0)
					win_y0 = 0;
				if(window_width>frameW)
					window_width = frameW;
				if(window_height> frameH)
					window_height = frameH ;
				if((win_x0+window_width)> frameW)
					win_x0 = frameW-window_width;
				if((win_y0+window_height)>frameH)
					win_y0 = (frameH-window_height);


				count_no_haar_detection = 0;

				

			}
			else {

				state = 4;
				count_no_haar_detection++;
				if(count_no_haar_detection == 4){
					count_no_haar_detection = 0;
					state = 1;

				}
				if(count_no_haar_detection == 1){
					IplImage *tpl_temp, *tpl_sk_temp;
					tpl_width = tpl_width/3;
					cvSetImageROI(tpl,cvRect( tpl_width,
									0,
									tpl_width,
									tpl_height ));

					// create template image 

					tpl_temp = cvCreateImage( cvSize( tpl_width, tpl_height ),
                         frame->depth, frame->nChannels );
					cvCopy(tpl,tpl_temp,NULL);
					cvResetImageROI(tpl);

					if(tpl != NULL)
						cvReleaseImage(&tpl);
					tpl = tpl_temp;

					

					//cvReleaseImage(&tpl_temp);
					//cvReleaseImage(&tpl_sk_temp);
				
				}
				
			}
		}
//...............................................state == 4.........................................................................
		else if(state == 4){

				count_template_matching_rounds++;

				cvSetImageROI( frame,
                   cvRect( win_x0,
                           win_y0,
                           window_width,
                           window_height ) );
			   
				
				if(tpl2 != NULL)
					cvReleaseImage(&tpl2);
				tpl2 = cvCreateImage( cvSize( window_width, window_height ),
					frame->depth, frame->nChannels );
				cvCopy( frame, tpl2, NULL );
				cvResetImageROI( frame );	

				trackObject(tpl2);
				//trackObject(tpl2_sk);


				object_x0 = win_x0 + minloc.x;
				object_y0 = win_y0 + minloc.y;


				cvSetImageROI(frame,cvRect( object_x0,
									object_y0,
									tpl_width,
									tpl_height ));

				// create template image 
				if(tpl != NULL)
					cvReleaseImage(&tpl);
				tpl = cvCreateImage( cvSize( tpl_width, tpl_height ),
                         frame->depth, frame->nChannels );
				cvCopy(frame,tpl,NULL);
				cvResetImageROI(frame);
				

				win_x0 = object_x0 - ( ( window_width  - tpl_width  ) / 2 );
				win_y0 = object_y0 - ( ( window_height - tpl_height ) / 2 );
   
	
				if(win_x0<=0)
					win_x0 = 0;
				if(win_y0<0)
					win_y0 = 0;
				if(window_width>frameW)
					window_width = frameW;
				if(window_height> frameH)
					window_height = frameH ;
				if((win_x0+window_width)> frameW)
					win_x0 = frameW-window_width;
				if((win_y0+window_height)>frameH)
					win_y0 = (frameH-window_height);


	
				cvRectangle( frame,
                     cvPoint( object_x0, object_y0 ),
                     cvPoint( object_x0 + tpl_width,
                              object_y0 + tpl_height ),
                     cvScalar( 0, 0, 255, 0 ), 2 );
				
				cvRectangle(frame,
					cvPoint(win_x0,win_y0),
					cvPoint(win_x0+window_width, win_y0+window_height),
					cvScalar(255,0,0,0),2);	

				cvRectangle( tpl2,
                     cvPoint( minloc.x, minloc.y ),
                     cvPoint( minloc.x + tpl_width,
                              minloc.y + tpl_height ),
                     cvScalar( 0, 0, 255, 0 ), 2 );
				 
				if(count_template_matching_rounds == 5){
					count_template_matching_rounds = 0;
					state = 3;
				}
		}
		
		//double t = (double)cvGetTickCount();

//.........................................Manual control of Pan and Tilt...................................................................
		if(selection == 3){
			int key = '1';
			if(!enable_port){
				printf("COM port is not enabled ... !\n");
				key = '0';
			}
			while(key != '0'){
				if(key == 'a')
					pan_value = pan_value + 1;
				else if(key == 's')
					pan_value = pan_value - 1;
				else if(key == 'd')
					tilt_value = tilt_value + 1;
				else if(key == 'f')
					tilt_value = tilt_value - 1;
				if((key == 'a')||(key == 's')||(key == 'd')||(key == 'f')){
					convert_values();
					print_to_port = WriteFile(hPort,pan,strlen(pan),&byteswritten,NULL);
					print_to_port = WriteFile(hPort,tilt,strlen(tilt),&byteswritten,NULL);
				
				}
				key = cvWaitKey(0);
			}
			state = 1;
			selection = 0;
			printf("Normal Program Enabled\n");	
		}
//........................................Calibrate Pan and Tilt.........................................................................
		if(selection == 4){
			char key[2];
			key[0] = '1';
			key[1] = '\0';
			if(!enable_port){
				printf("COM port is not enabled ... !\n");
				key[0] = '0';
			}
			while(key[0] != '0'){

				if((key[0] == 'a') || (key[0] == 's') || (key[0] == 'd') || (key[0] == 'f') || 
					(key[0] == 'g') || (key[0] == 'z') ||(key[0] == 'x') || (key[0] == 'c') || 
					(key[0] == 'v') || (key[0] == 'b'))
					print_to_port = WriteFile(hPort,key,strlen(key),&byteswritten,NULL);
			
				key[0] = cvWaitKey(0);
			}
			state = 1;
			selection = 0;
			printf("Normal Program Enabled\n");
		
		}

//..................send cordinates of target to COM port...............................................................................
		if(state == 3 || state == 4){
			calculate_cordinates();
			calculate_pan_and_tilt();
			convert_values();
			if(enable_port)
			{	

				print_to_port = WriteFile(hPort,pan,strlen(pan),&byteswritten,NULL);

				print_to_port = WriteFile(hPort,tilt,strlen(tilt),&byteswritten,NULL);
			}
		}
		
		//t = (double)cvGetTickCount() - t;
		//printf( "detection time = %gms\n", t/((double)cvGetTickFrequency()*1000.) );
			
				/*
				double t = (double)cvGetTickCount();
	
				t = (double)cvGetTickCount() - t;
				printf( "detection time = %gms\n", t/((double)cvGetTickFrequency()*1000.) );
				*/

		if(skin_detection_method != 0){
			cvShowImage("video_sk", frame_sk);

		}

		cvShowImage("tpl",tpl);

		cvShowImage("tpl2",tpl2);

 		cvShowImage( "video", frame );

        // exit if user press ESC
        key = cvWaitKey( 1 );
	}
 
	
    // free memory 
    cvDestroyWindow( "video" );
	if(skin_detection_method != 0){
		cvDestroyWindow("video_sk");
	}
	cvDestroyWindow("tpl");

	cvDestroyWindow("tpl2");

	if(skin_detection_method != 0){

		cvReleaseImage(&frame_mask_bw);

	}

    cvReleaseCapture( &capture );
    cvReleaseImage( &tpl );

	cvReleaseImage(&tpl2);

    cvReleaseHaarClassifierCascade( &cascade );

	if(enable_port) CloseHandle(hPort); //close the handle  

   
    return 0;
}

//...............mouse handler ...........................................................................................................

void mouseHandler( int event, int x, int y, int flags, void *param )
{

	if(event == CV_EVENT_LBUTTONDOWN){
		pos_x1 = x;
		pos_y1 = y;
	
	}
	else if( event == CV_EVENT_LBUTTONUP ) {
		//printf("x : %d, y : %d\t",x,y);
		pos_x2 = x;
		pos_y2 = y;
		if((pos_x2 > pos_x1)&&(pos_y2>pos_y1))
		accept_mouse_input = 1;

    }
}
 
//..............track object (Template Matching)..........................................................................................

void trackObject(IplImage *image)
{

	CvRect searchROI, templateROI;

		searchROI = cvGetImageROI(image);
		templateROI = cvGetImageROI(tpl);
	
		tm = cvCreateImage( cvSize( searchROI.width - templateROI.width + 1,
			searchROI.height - templateROI.height + 1),
                        IPL_DEPTH_32F, 1 );

		cvMatchTemplate( image, tpl, tm, CV_TM_SQDIFF_NORMED);
		//cvMatchTemplate(image,tpl,tm,CV_TM_CCORR_NORMED);
		//cvMatchTemplate(image,tpl,tm,CV_TM_CCORR);
		//cvMatchTemplate(image,tpl,tm,CV_TM_CCOEFF_NORMED);
		cvMinMaxLoc( tm, &minval, &maxval, &minloc, &maxloc, 0 );
	
		cvReleaseImage(&tm);
}

// .................detect faces (haar classifier) .....................................................................................

int detect_faces( IplImage* image,CvHaarClassifierCascade* cascade,int do_pyramids)
{
    IplImage* small_image = image;
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* faces;
    int i, scale = 1;
	int temp = 0;

    // if the flag is specified, down-scale the input image to get a performance boost 
	//  w/o loosing quality
    if( do_pyramids )
    {
        small_image = cvCreateImage( cvSize(image->width/2,image->height/2),
			IPL_DEPTH_8U, 3 );
        cvPyrDown( image, small_image, CV_GAUSSIAN_5x5 );
        scale = 2;
    }

	faces = cvHaarDetectObjects( small_image, cascade, storage, 
								1.2,						// Scale factor
								2,							// Minimum number of neighbours
								CV_HAAR_FIND_BIGGEST_OBJECT,// Operation flag
								cvSize(20,20));				// Minimum size of the face
									
    /* draw all the rectangles */
    for( i = 0; i < faces->total; i++ )
    {
        // extract the rectanlges only 
        CvRect face_rect = *(CvRect*)cvGetSeqElem( faces, i );

		face.height = face_rect.height*scale;
		face.width  = face_rect.width*scale;
		face.x = face_rect.x*scale;
		face.y = face_rect.y*scale;
		
		temp = faces->total;
    }

    if( small_image != image )
        cvReleaseImage( &small_image );
    cvReleaseMemStorage( &storage );
	return temp;
}

//.....................get skin mask .........................................................................................

void GetSkinMask(IplImage * src_RGB, IplImage * mask_BW, int erosions, int dilations)
{
	CvSize sz = cvSize( src_RGB->width & -2, src_RGB->height & -2);
	//get the size of input_image (src_RGB)

	IplImage* pyr = cvCreateImage( cvSize(sz.width/2, sz.height/2), 8, 3 ); 
	//create 2 temp-images pyr is needed to downsample the source image,
	// which is required for noise reduction.

	IplImage* src = cvCreateImage(cvGetSize(src_RGB), IPL_DEPTH_8U ,3);
	cvCopyImage(src_RGB, src);

	IplImage* tmpHSV = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U , 3);

	//remove noise from input image = cvPyrDown() + cvPyrUp() 
	// using the CV_GAUSSIAN_5X5

	cvPyrDown( src, pyr, 7 );
	// Downsamples an image. Function performs downsampling step of the Gaussian
	//	Pyramid Decomposition.Convolutes the source image with the specified filter 
	//	(CV_GAUSSIAN_5X5 = 7) and then downsamples the image by rejecting even rows 
	//	and columns.

	cvPyrUp( pyr, src, 7 );
	/* Performs up-sampling step of Gaussian pyramid decomposition.First it 
	upsamples the source image by injecting even zero rows and columns and then 
	convolves result with the specified filtermultiplied by 4 for interpolation. 
	So the destination image is four times larger than the source image.
	*/

	if(skin_detection_method == 2)
		cvCvtColor(src ,tmpHSV , CV_RGB2YCrCb);		// RGB to YCrCb color conversion
	else if(skin_detection_method == 1)
		cvCvtColor(src, tmpHSV, CV_RGB2HSV);
	else if(skin_detection_method == 3)
		cvCopyImage(src,tmpHSV);

	uchar H;
	uchar S;
	uchar V;

	uchar Y;
	uchar Cb;
	uchar Cr;

	uchar R;
	uchar G;
	uchar B;

	CvPixelPosition8u pos_src;	// To move through the pixels easily
	CvPixelPosition8u pos_dst;

	int x =0;
	int y =0;

	//Initializing the pixel position structure
	CV_INIT_PIXEL_POS(pos_src,(unsigned char *) tmpHSV->imageData,
		tmpHSV->widthStep,cvGetSize(tmpHSV),x,y,tmpHSV->origin);

	CV_INIT_PIXEL_POS(pos_dst,(unsigned char *) mask_BW->imageData,
		mask_BW->widthStep,cvGetSize(mask_BW),x,y,mask_BW->origin);



	uchar * ptr_src;		// A single pixel
	uchar * ptr_dst;


	for( y=0;y<src-> height; y++)
	{

		for ( x=0; x<src->width; x++)
		{

			ptr_src = CV_MOVE_TO(pos_src,x,y,3);// Move to the position
			ptr_dst = CV_MOVE_TO(pos_dst,x,y,3);

			H = ptr_src[0];	// Y, Cb, Cr extraction from source pixel
			S = ptr_src[1];
			V = ptr_src[2];

			Y = H;
			Cr = S;
			Cb = V;

			R = H;
			G = S;
			B = V;

			// Color condition and detection
			if(skin_detection_method == 1){
				if((H > Hmin) && (H < Hmax))
				{
					ptr_dst[0] = 255;		// Mask creation
					ptr_dst[1] = 255;
					ptr_dst[2] = 255;
				}
				else
				{
					ptr_dst[0] = 0;
					ptr_dst[1] = 0;
					ptr_dst[2] = 0;
				}
			}
			else if(skin_detection_method == 2){
				if((Y > Ymin) && (Y < Ymax) && (Cb > Cbmin) &&
					(Cb < Cbmax) && (Cr > Crmin) && (Cr < Crmax))
				{
					ptr_dst[0] = 255;		// Mask creation
					ptr_dst[1] = 255;
					ptr_dst[2] = 255;
				}
				else
				{
					ptr_dst[0] = 0;
					ptr_dst[1] = 0;
					ptr_dst[2] = 0;
				}
			}
			else if(skin_detection_method == 3){
				if((R < Redmax) && (G < Greenmax) && (B < Bluemax) &&
					(R > Redmin) &&(G > Greenmin) && (B > Bluemin))
				{
					ptr_dst[0] = 255;		// Mask creation
					ptr_dst[1] = 255;
					ptr_dst[2] = 255;
				}
				else
				{
					ptr_dst[0] = 0;
					ptr_dst[1] = 0;
					ptr_dst[2] = 0;
				}
			}
		}
	}
	if(erosions>0) 
		cvErode(mask_BW,mask_BW,0,erosions);	// Erode the mask
	if (dilations>0) 
		cvDilate(mask_BW,mask_BW,0,dilations);	// Dialate the mask

    //Apply an averaging filter to remove noise from mask_BW image
    cvSmooth(mask_BW, mask_BW, CV_MEDIAN, 5, 5, 0, 0);

	cvReleaseImage(&pyr);
	cvReleaseImage(&tmpHSV);
	cvReleaseImage(&src);
}

//..................calculate cordinates......................................................................................................
void calculate_cordinates(void){

	center_x = object_x0 + tpl_width/2;
	center_y = object_y0 + tpl_height/2;

	center_x = frameW - center_x;

	center_x = center_x - frameW/2;	// center_x >= -480 and  <= 480
	center_y = center_y - frameH/2;	// center_y >= -360 and  <= 360

}
//................calculate pan and tilt ...................................................................................................
void calculate_pan_and_tilt(void){

	if(center_x > 200)
		pan_value = pan_value + 4;
	else if(center_x > 100)
		pan_value = pan_value + 1;
	else if(center_x < -200)
		pan_value = pan_value - 4;
	else if(center_x < -100)
		pan_value = pan_value - 1;

	if(center_y > 200)
		tilt_value = tilt_value + 4;
	else if(center_y > 100)
		tilt_value = tilt_value + 1;
	else if(center_y < -200)
		tilt_value = tilt_value - 4;
	else if(center_y < -100)
		tilt_value = tilt_value - 1;
	
}

//............convert values...........................................................................................................
void convert_values(void){

	int temp;
	temp = pan_value;
	pan[3] = (temp%10)+'0';
	temp = temp/10;
	pan[2] = (temp%10)+'0';
	temp = temp/10;
	pan[1] = (temp%10)+'0';
	temp = temp/10;
	pan[0] = 'p';
	pan[4] = '\0';


	
	temp = tilt_value;
	tilt[3] = (temp%10)+'0';
	temp = temp/10;
	tilt[2] = (temp%10)+'0';
	temp = temp/10;
	tilt[1] = (temp%10)+'0';
	temp = temp/10;
	tilt[0] = 't';
	tilt[4] = '\0';

}

