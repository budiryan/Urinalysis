#include "image_processing.h"
/*
This reference colors are from the original paper
#define REF1_VAL {146, 203, 170, 0.0}
#define REF2_VAL {144, 197, 106, 100.0}
#define REF3_VAL {118, 163, 47, 200.0}
#define REF4_VAL {132, 118, 19, 500.0}
#define REF5_VAL {124, 82, 0, 1000.0}
#define REF6_VAL {90, 40, 13, 2000.0}
*/

// Reference color from real environment --> mg / dL
#define REF1_VAL {175, 167, 127, 0}
#define REF2_VAL {135, 159, 119, 15}
#define REF3_VAL {95, 151, 119, 50}
#define REF4_VAL {79, 135, 119, 150}
#define REF5_VAL {71, 103, 87, 500}
#define REF6_VAL {44, 71, 63, 1000}


// Constants used for rgb transformation
extern char str[100];
float ref_x = 95.047;
float ref_y = 100.0;
float ref_z = 108.883;
float interpolation_score;
volatile u16 segmentation[SEGMENT_ROWS * SEGMENT_COLUMNS];
volatile uint16_t frame_buffer[CAMERA_ROWS * CAMERA_COLUMNS];

COLOR_OBJECT ref1 = REF1_VAL;
COLOR_OBJECT ref2 = REF2_VAL;
COLOR_OBJECT ref3 = REF3_VAL;
COLOR_OBJECT ref4 = REF4_VAL;
COLOR_OBJECT ref5 = REF5_VAL;
COLOR_OBJECT ref6 = REF6_VAL;

void assign_interpolation_index(int * idx_b, int * idx_c, int idx_n, const int NUM_REFERENCE, float ref_dist[], float test_ref_dist[]){
    if(idx_n - 1 < 0){
        *idx_b = idx_n;
        *idx_c = idx_n + 1;
        return;
    }
    else if(idx_n + 1 >= NUM_REFERENCE){
        *idx_c = idx_n;
        *idx_b = idx_n - 1;
        return;
    }
    
    float test_to_n_minus_1 = test_ref_dist[idx_n - 1];
    float test_to_n_plus_1 = test_ref_dist[idx_n + 1];
    int idx_node;
    if (test_to_n_minus_1 < test_to_n_plus_1)
        idx_node = 0;
    else
        idx_node = 1;
    switch(idx_node){
        case 0:
            if (test_ref_dist[idx_n - 1] < ref_dist[idx_n - 1]){
               *idx_b = idx_n - 1;
               *idx_c = idx_n;
                return;
            }
        break;
        case 1:
            if (test_ref_dist[idx_n + 1] < ref_dist[idx_n]){
               *idx_b = idx_n;
               *idx_c = idx_n + 1;
                return;
            }
        break;
        
        default:
            // Test data is invalid
            *idx_b = -1;
            *idx_c = -1;
            return;
            
    }
    *idx_b = idx_n;
    *idx_c = idx_n + 1;
}

float interpolate(COLOR_OBJECT test_data){
    /*
     * Algorithm based on the paper: http://ieeexplore.ieee.org/document/6865777/
     * Author: Budi RYAN
     */
    // Define reference data, test data and other necessary flags + variables
    const int NUM_REFERENCE = 6;
    const int NUM_STAGE = NUM_REFERENCE - 1;
    COLOR_OBJECT ref_data[NUM_REFERENCE] = {ref1, ref2, ref3, ref4, ref5, ref6};
    float ref_dist[NUM_STAGE];
    float test_ref_dist[NUM_REFERENCE];
    int idx_n,idx_b, idx_c;
    float cx, bx, final_score;
   
    // Calculate the distance within reference data
    for(int i=0; i < NUM_STAGE; i++){
        ref_dist[i] = RGB_color_Lab_difference_CIE76(ref_data[i], ref_data[i + 1]);
    }

    // Calculate the distance from test data to each of the reference data
    for(int i=0; i < NUM_REFERENCE; i++){
        test_ref_dist[i] = RGB_color_Lab_difference_CIE76(test_data, ref_data[i]);
    }
    /* USEFUL FOR DEBUGGING
    sprintf(str, "%.2f\n%.2f\n%.2f\n%.2f\n%.2f\n%.2f",test_ref_dist[0],test_ref_dist[1],test_ref_dist[2],test_ref_dist[3],test_ref_dist[4],test_ref_dist[5]);
    TM_ILI9341_Puts(180, 40, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    sprintf(str, "%.2f\n%.2f\n%.2f\n%.2f\n%.2f",ref_dist[0],ref_dist[1],ref_dist[2],ref_dist[3],ref_dist[4]);
    TM_ILI9341_Puts(240, 40, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    */
    idx_n = smallest_arg(test_ref_dist, NUM_STAGE);
    
    assign_interpolation_index(&idx_b, &idx_c, idx_n, NUM_REFERENCE, ref_dist, test_ref_dist);
    // test data is out of range
    /*
    if (idx_b < 0 || idx_c < 0){
        TM_ILI9341_Puts(0, 180, "Fail 1", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
        return -1;
    }
     
    // another error checking here
    if ((test_ref_dist[idx_b] > ref_dist[idx_b]) || (test_ref_dist[idx_c] > ref_dist[idx_b])){
        TM_ILI9341_Puts(0, 180, "Fail 2", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
        return -1;
    }
    */
    // use trigonometry formula to find cx
    cx = (powf(test_ref_dist[idx_b], 2.0f) - powf(ref_dist[idx_b], 2.0f) - powf(test_ref_dist[idx_c], 2.0f)) / (-2.0f * ref_dist[idx_b]);
    // score of the test data is finally evaluated
    bx = ref_dist[idx_b] - cx;
    final_score = ((bx / ref_dist[idx_b]) * (float)(ref_data[idx_c].score - ref_data[idx_b].score)) + ref_data[idx_b].score;
    return final_score;
}


/***************************************************
 *  Name:        convertRGBtoXYZ
 *
 *  Returns:     Nothing
 *
 *  Parameters:  RGB values and XYZ references
 *
 *  Description: Given a color in RGB values converts  
 *               to the corresponding XYZ values
 *
 ***************************************************/
void convertRGBtoXYZ(int inR, int inG, int inB, float * outX, float * outY, float * outZ) {


	float var_R = (inR / 255.0f); //R from 0 to 255
	float var_G = (inG / 255.0f); //G from 0 to 255
	float var_B = (inB / 255.0f); //B from 0 to 255

	if (var_R > 0.04045f)
		var_R = powf(( (var_R + 0.055f) / 1.055f), 2.4f);
	else 
		var_R = var_R / 12.92f;

	if (var_G > 0.04045f)
		var_G = powf(( (var_G + 0.055f) / 1.055f), 2.4f);
	else
		var_G = var_G / 12.92f;

	if (var_B > 0.04045f)
		var_B = powf(( (var_B + 0.055f) / 1.055f), 2.4f);
	else
		var_B = var_B / 12.92f;

	var_R = var_R * 100;
	var_G = var_G * 100;
	var_B = var_B * 100;

	//Observer. = 2°, Illuminant = D65
	*outX = var_R * 0.4124f + var_G * 0.3576f + var_B * 0.1805f;
	*outY = var_R * 0.2126f + var_G * 0.7152f + var_B * 0.0722f;
	*outZ = var_R * 0.0193f + var_G * 0.1192f + var_B * 0.9505f;
}


/***************************************************
 *  Name:        convertXYZtoLab
 *
 *  Returns:     Nothing
 *
 *  Parameters:  XYZ values and Lab references
 *
 *  Description: Given a color in XYZ values converts  
 *               to the corresponding Lab values
 *
 ***************************************************/
void convertXYZtoLab(float inX, float inY, float inZ, float * outL, float * outa, float * outb) {

	float var_X = (inX / ref_x); //ref_x = 95.047
	float var_Y = (inY / ref_y); //ref_y = 100.0
	float var_Z = (inZ / ref_z); //ref_z = 108.883

	if ( var_X > 0.008856f ) 
		var_X = powf(var_X , ( 1.0f/3 )); 
	else 
		var_X = ( 7.787f * var_X ) + ( 16.0f/116 );

	if ( var_Y > 0.008856f )
		var_Y = powf(var_Y , ( 1.0f/3 )); 
	else
	    var_Y = ( 7.787f * var_Y ) + ( 16.0f/116 );

	if ( var_Z > 0.008856f )
		var_Z = powf(var_Z , ( 1.0f/3 )); 
	else 
		var_Z = ( 7.787f * var_Z ) + ( 16.0f/116 );

	*outL = ( 116 * var_Y ) - 16;
	*outa = 500 * ( var_X - var_Y );
	*outb = 200 * ( var_Y - var_Z );
}


/***************************************************
 *  Name:        convertLabtoXYZ
 *
 *  Returns:     Nothing
 *
 *  Parameters:  Lab values and XYZ references
 *
 *  Description: Given a color in Lab values converts  
 *               to the corresponding XYZ values
 *
 ***************************************************/
void convertLabtoXYZ( float inL, float ina, float  inb, float * outX, float * outY, float * outZ) {

	float var_Y = ( inL + 16 ) / 116;
	float var_X = (ina/500) + var_Y;
	float var_Z = var_Y - (inb/200);

	if ( powf(var_Y,3.f) > 0.008856f ) 
		var_Y = powf(var_Y,3.f);
	else
		var_Y = ( var_Y - (16.0f/116.0f) ) / 7.787f;

	if ( powf(var_X,3.f) > 0.008856f ) 
		var_X = powf(var_X,3.f);
	else 
		var_X = ( var_X - (16.0f/116.0f) ) / 7.787f;
	
	if ( powf(var_Z,3.f) > 0.008856f )
		var_Z = powf(var_Z,3.f);
	else
		var_Z = ( var_Z - (16.0f/116.0f) ) / 7.787f;

	*outX = ref_x * var_X;     //ref_x =  95.047     Observer= 2°, Illuminant= D65
	*outY = ref_y * var_Y;     //ref_y = 100.000
	*outZ = ref_z * var_Z;     //ref_z = 108.883
}

/***************************************************
 *  Name:        convertXYZtoRGB
 *
 *  Returns:     Nothing
 *
 *  Parameters:  XYZ values and RGB references
 *
 *  Description: Given a color in XYZ values converts  
 *               to the corresponding RGB values
 *
 ***************************************************/
void convertXYZtoRGB(float inX, float inY, float inZ, int * outR, int * outG, int * outB) {


	float var_X = inX/100;
	float var_Y = inY/100;
	float var_Z = inZ/100;

	float var_R = var_X *  3.2406f + (var_Y * -1.5372f) + var_Z * (-0.4986f);
	float var_G = var_X * (-0.9689f) + var_Y *  1.8758f + var_Z *  0.0415f;
	float var_B = var_X *  0.0557f + var_Y * (-0.2040f) + var_Z *  1.0570f;

	if ( var_R > 0.0031308f )
		var_R = 1.055f * powf(var_R, ( 1.0f / 2.4f ) )  - 0.055f;
	else 
		var_R = 12.92f * var_R;

	if ( var_G > 0.0031308f ) 
		var_G = 1.055f * powf(var_G, ( 1.0f / 2.4f ) ) - 0.055f;
	else 
		var_G = 12.92f * var_G;

	if ( var_B > 0.0031308f )
		var_B = 1.055f * powf(var_B, ( 1.0f / 2.4f ) ) - 0.055f;
	else
		var_B = 12.92f * var_B;

	*outR = (int)(var_R * 255);
	*outG = (int)(var_G * 255);
	*outB = (int)(var_B * 255);


}

/***************************************************
 *  Name:        Lab_color_difference_CIE76
 *
 *  Returns:     float
 *
 *  Parameters:  2 Lab color values
 *
 *  Description: Calculates and returns the difference 
 *				 between 2 Lab colors based on the CIE76 formula
 *
 ***************************************************/
float Lab_color_difference_CIE76( float inL1, float ina1, float  inb1, float inL2, float ina2, float  inb2){
	return( sqrt( powf(inL1 - inL2, 2.f) + powf(ina1 - ina2, 2.f) + powf(inb1 - inb2, 2.f) ) );//CIE76
}

/***************************************************
 *  Name:        RGB_color_Lab_difference_CIE76
 *
 *  Returns:     float
 *
 *  Parameters:  2 RGB color values
 *
 *  Description: Converts RGB values to Lab values and returns  
 *				 the difference between 2 Lab colors based on 
 *				 the CIE76 formula
 *
 ***************************************************/
float RGB_color_Lab_difference_CIE76(COLOR_OBJECT color1, COLOR_OBJECT color2){
	float x1; float y1; float z1;
	float x2; float y2; float z2;
	float l1; float a1; float b1;
	float l2; float a2; float b2;

	convertRGBtoXYZ(color1.R, color1.G, color1.B, &x1, &y1, &z1);
	convertRGBtoXYZ(color2.R, color2.G, color2.B, &x2, &y2, &z2);

	convertXYZtoLab(x1, y1, z1, &l1, &a1, &b1);
	convertXYZtoLab(x2, y2, z2, &l2, &a2, &b2); 
    
	return( Lab_color_difference_CIE76(l1 ,a1 ,b1 ,l2 ,a2 ,b2) );
}

/***************************************************
 *  Name:        Lab_color_difference_CIE94
 *
 *  Returns:     float
 *
 *  Parameters:  2 Lab color values
 *
 *  Description: Calculates and returns the difference 
 *				 between 2 Lab colors based on the CIE94 formula
 *
 ***************************************************/
float Lab_color_difference_CIE94( float inL1, float ina1, float  inb1, float inL2, float ina2, float  inb2){
	// case Application.GraphicArts:
		float Kl = 1.0;
		float K1 = 0.045;
		float K2 = 0.015;
	// 	break;
	// case Application.Textiles:
	// 	Kl = 2.0;
	// 	K1 = .048;
	// 	K2 = .014;
	// break;

	float deltaL = inL1 - inL2;
	float deltaA = ina1 - ina2;
	float deltaB = inb1 - inb2;

	float c1 = sqrt(pow(ina1, 2) + pow(inb1, 2));
	float c2 = sqrt(pow(ina2, 2) + pow(inb2, 2));
	float deltaC = c1 - c2;

	float deltaH = pow(deltaA,2) + pow(deltaB,2) - pow(deltaC,2);
	deltaH = deltaH < 0 ? 0 : sqrt(deltaH);

	const float sl = 1.f;
	const float kc = 1.f;
	const float kh = 1.f;

	float sc = 1.f + K1*c1;
	float sh = 1.f + K2*c1;

	float i = pow(deltaL/(Kl*sl), 2) +
	                pow(deltaC/(kc*sc), 2) +
	                pow(deltaH/(kh*sh), 2);

	float finalResult = i < 0 ? 0 : sqrt(i);
	return (finalResult);
}

/***************************************************
 *  Name:        RGB_color_Lab_difference_CIE94
 *
 *  Returns:     float
 *
 *  Parameters:  2 RGB color values
 *
 *  Description: Converts RGB values to Lab values and returns  
 *				 the difference between 2 Lab colors based on 
 *				 the CIE94 formula
 *
 ***************************************************/
float RGB_color_Lab_difference_CIE94( int R1, int G1, int B1, int R2, int G2, int B2){
	float x1=0,y1=0,z1=0;
	float x2=0,y2=0,z2=0;
	float l1=0,a1=0,b1=0;
	float l2=0,a2=0,b2=0;

	convertRGBtoXYZ(R1, G1, B1, &x1, &x1, &z1);
	convertRGBtoXYZ(R2, G2, B2, &x2, &x2, &z2);

	convertXYZtoLab(x1, y1, z1, &l1, &a1, &b1);
	convertXYZtoLab(x2, y2, z2, &l2, &a2, &b2);  

	return( Lab_color_difference_CIE94(l1 ,a1 ,b1 ,l2 ,a2 ,b2) );
}

// Displays the average of each color channel, given an image array
void display_analysis(u16 image[], u16 array_length, COLOR_TYPE color){
    uint64_t r = 0;
    uint64_t g = 0;
    uint64_t b = 0;
    uint64_t temp = 0;
    uint64_t overall = 0;
   
    switch(color){
        case RGB555:
            for(int i = 0 ; i < array_length; i++){
                r += ((image[i] & 0x7C00) >> 10);
                g += ((image[i] & 0x3E0) >> 5);
                b += (image[i] & 0x1F);
            }
            r /= ((float)array_length);
            g /= ((float)array_length);
            b /= ((float)array_length);
            overall = r  << 11;
            overall = overall | (g << 5);
            overall = overall | b;
        break;
        case RGB565:
            for(int i = 0 ; i < array_length; i++){
                r += ((image[i] & 0xF800) >> 11);
                g += ((image[i] & 0x7E0) >> 5);
                b += (image[i] & 0x1F);
            }
            r /= ((float)array_length);
            temp = (float)g / (array_length);
            g = (float)temp / (float)64.0 * (float)32.0;
            b /= ((float)array_length);
            overall = r  << 11;
            temp = (float)g  / (float)32.0 * (float)64.0;
            overall = overall | (temp << 5);
            overall = overall | b;
        break;
    }
    
    // convert to cie lab
    float X, Y, Z;
    float L, A, B;
    // convert r, g, b to 255 scale
    r = r / 32.0f * 255.0f;
    g = g / 32.0f * 255.0f;
    b = b / 32.0f * 255.0f;
    convertRGBtoXYZ(r, g, b, &X, &Y, &Z);
    convertXYZtoLab(X, Y, Z, &L, &A, &B);
    COLOR_OBJECT test = {r, g, b, 0};
    TM_ILI9341_Puts(0, 140, "               ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(0, 160, "               ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(0, 180, "               ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(0, 140, "L: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(0, 160, "A: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(0, 180, "B: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    
    sprintf(str, "%.2f", L);
    TM_ILI9341_Puts(20, 140, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    sprintf(str, "%.2f", A);
    TM_ILI9341_Puts(20, 160, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    sprintf(str, "%.2f", B);
    TM_ILI9341_Puts(20, 180, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    
    
    TM_ILI9341_Puts(100, 140, "glucose score:", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    interpolation_score = interpolate(test) > 0 ? interpolate(test) : 0;
    sprintf(str, "%.2f", interpolation_score);
    TM_ILI9341_Puts(100, 160, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
}
