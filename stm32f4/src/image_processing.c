#include "image_processing.h"
#include <stdlib.h>

// Reference color from real environment --> mg / dL --> in Room 3115
// Format {R888, G888, B888, Glucose concentration in mg / dL}
#define REF1_VAL {175, 167, 135, 0}
#define REF2_VAL {135, 159, 127, 15}
#define REF3_VAL {95, 135, 103, 50}
#define REF4_VAL {71, 111, 95, 150}
#define REF5_VAL {63, 87, 79, 500}
#define REF6_VAL {47, 55, 55, 1000}

COLOR_OBJECT ref1 = REF1_VAL;
COLOR_OBJECT ref2 = REF2_VAL;
COLOR_OBJECT ref3 = REF3_VAL;
COLOR_OBJECT ref4 = REF4_VAL;
COLOR_OBJECT ref5 = REF5_VAL;
COLOR_OBJECT ref6 = REF6_VAL;

// Reference Color for Urine Color scores
/*
#define REF1_VAL_COLOR {255, 255, 255, 1}
#define REF2_VAL_COLOR {255, 255, 204, 2}
#define REF3_VAL_COLOR {254, 254, 180, 3}
#define REF4_VAL_COLOR {254, 254, 90, 4}
#define REF5_VAL_COLOR {254, 254, 139, 5}
#define REF6_VAL_COLOR {254, 254, 120, 6}
#define REF7_VAL_COLOR {253, 220, 106, 7}
#define REF8_VAL_COLOR {161, 147, 99, 8}
*/
// Reference Color for Urine Color scores
#define REF1_VAL_COLOR {207, 207, 191, 1}
#define REF2_VAL_COLOR {183, 191, 159, 2}
#define REF3_VAL_COLOR {183, 183, 151, 3}
#define REF4_VAL_COLOR {151, 151, 95, 4}
#define REF5_VAL_COLOR {143, 135, 63, 5}
#define REF6_VAL_COLOR {135, 127, 47, 6}
#define REF7_VAL_COLOR {135, 111 ,39, 7}
#define REF8_VAL_COLOR {109, 103, 39, 8}



COLOR_OBJECT ref1_color = REF1_VAL_COLOR;
COLOR_OBJECT ref2_color = REF2_VAL_COLOR;
COLOR_OBJECT ref3_color = REF3_VAL_COLOR;
COLOR_OBJECT ref4_color = REF4_VAL_COLOR;
COLOR_OBJECT ref5_color = REF5_VAL_COLOR;
COLOR_OBJECT ref6_color = REF6_VAL_COLOR;
COLOR_OBJECT ref7_color = REF7_VAL_COLOR;
COLOR_OBJECT ref8_color = REF8_VAL_COLOR;

// Constants used by LAB formula for rgb transformation
extern char str[40];
float ref_x = 95.047;
float ref_y = 100.0;
float ref_z = 108.883;
float interpolation_score;

// Variable used to store segmented area from camera array
volatile u16 segmentation[SEGMENT_ROWS * SEGMENT_COLUMNS];

// The whole camera array
volatile u16 frame_buffer[CAMERA_ROWS * CAMERA_COLUMNS];

unsigned int red;
unsigned int green;
unsigned int blue;


/***************************************************
 *  Returns:     Cosine angle distance
 *
 *  Parameters:  2 vectors with 3 dimensions
 *
 *
 ***************************************************/
s16 calculate_angle(float u1, float v1, float w1, float u2, float v2, float w2){
    float dot_product = u1 * u2 + v1 * v2 + w1 * w2;
    float vector_1_magnitude = sqrt(u1 * u1 + v1 * v1 + w1 * w1);
    float vector_2_magnitude = sqrt(u2 * u2 + v2 * v2 + w2 * w2);
    float denom  = vector_1_magnitude * vector_2_magnitude;
    if (denom == 0)
        denom = 0.0001;
   
    float cos_angle = dot_product / denom;
    
    // Must scale cos_angle to 10000 before feeding to function
    s16 angle =  int_arc_cos(10000 * cos_angle);
    return angle;
}

/***************************************************
 *  Returns:     Nothing
 *
 *  Parameters:  Index B, Index C, Index N (Smallest distance from test data to one of the reference data),
 *               , Number of reference data, test_ref_dist(Distance from test data to each of the reference point)
 *
 *  Description: A utility function used by function 'interpolate', it assigns Index B and C to 2 of the given reference points,
 *               See the paper for more details
 *
 ***************************************************/
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



/***************************************************
 *  Returns:     Interpolation value from reference
 *               urine color data
 *
 *  Parameters:  A COLOR_OBJECT object
 *
 *  Description: Algorithm based on the paper: http://ieeexplore.ieee.org/document/6865777/
 *               Calculates the amount of glucose of a color data given other reference data
 *
 ***************************************************/
float interpolate(COLOR_OBJECT test_data){
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
    
    idx_n = smallest_arg(test_ref_dist, NUM_REFERENCE);
    
    assign_interpolation_index(&idx_b, &idx_c, idx_n, NUM_REFERENCE, ref_dist, test_ref_dist);
    // use trigonometry formula to find cx
    cx = (powf(test_ref_dist[idx_b], 2.0f) - powf(ref_dist[idx_b], 2.0f) - powf(test_ref_dist[idx_c], 2.0f)) / (-2.0f * ref_dist[idx_b]);
    // score of the test data is finally evaluated
    bx = ref_dist[idx_b] - cx;
    final_score = ((bx / ref_dist[idx_b]) * (float)(ref_data[idx_c].score - ref_data[idx_b].score)) + ref_data[idx_b].score;
    return final_score;
}

float interpolate_color(COLOR_OBJECT test_data){
    // Define reference data, test data and other necessary flags + variables
    const int NUM_REFERENCE = 8;
    const int NUM_STAGE = NUM_REFERENCE - 1;
    COLOR_OBJECT ref_data[NUM_REFERENCE] = {ref1_color, ref2_color, ref3_color, ref4_color, ref5_color, ref6_color, ref7_color, ref8_color};
    
    
    float ref_dist[NUM_STAGE];
    float test_ref_dist[NUM_REFERENCE];
    int idx_n, idx_b, idx_c;
    float cx, bx, final_score;
   
    // Calculate the distance within reference data
    for(int i=0; i < NUM_STAGE; i++){
        ref_dist[i] = RGB_color_Lab_difference_CIE76(ref_data[i], ref_data[i + 1]);
    }

    // Calculate the distance from test data to each of the reference data
    for(int i=0; i < NUM_REFERENCE; i++){
        test_ref_dist[i] = RGB_color_Lab_difference_CIE76(test_data, ref_data[i]);
    }
    


    
    
    idx_n = smallest_arg(test_ref_dist, NUM_REFERENCE);
    
    // USEFUL FOR DEBUGGING
    /*
    sprintf(str, "%.2f %.2f", test_ref_dist[0], test_ref_dist[1]);
    TM_ILI9341_Puts(160, 40, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    sprintf(str, "%.2f %.2f",test_ref_dist[2], test_ref_dist[3]);
    TM_ILI9341_Puts(160, 60, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    sprintf(str, "%.2f %.2f", test_ref_dist[4], test_ref_dist[5]);
    TM_ILI9341_Puts(160, 80, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    sprintf(str, "%.2f %.2f", test_ref_dist[6], test_ref_dist[7]);
    TM_ILI9341_Puts(160, 100, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    sprintf(str, "%d", idx_n);
    TM_ILI9341_Puts(160, 120, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    */
    
    assign_interpolation_index(&idx_b, &idx_c, idx_n, NUM_REFERENCE, ref_dist, test_ref_dist);
    // use trigonometry formula to find cx
    cx = (powf(test_ref_dist[idx_b], 2.0f) - powf(ref_dist[idx_b], 2.0f) - powf(test_ref_dist[idx_c], 2.0f)) / (-2.0f * ref_dist[idx_b]);
    // score of the test data is finally evaluated
    bx = ref_dist[idx_b] - cx;
    final_score = ((bx / ref_dist[idx_b]) * (float)(ref_data[idx_c].score - ref_data[idx_b].score)) + ref_data[idx_b].score;
    if(final_score < 1)
        final_score = 1.0;
    if (final_score > 8)
        final_score = 8.0;
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

	//Observer. = 2�, Illuminant = D65
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

	*outX = ref_x * var_X;     //ref_x =  95.047     Observer= 2�, Illuminant= D65
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

/***************************************************
 *  Returns:     Information on LCD, nothing returned by this function
 
 *  Parameters:  Image array, length of the array, RGB 565 OR RGB 555 color format
 *
 *  Description: Displays the average R, G ,B element of an image array,
 *               Also displays interpolation value of the segmented color array
 *
 ***************************************************/
float display_analysis(ANALYSIS_TYPE analysis_type, int hardcode){ 
    COLOR_OBJECT test = {red, green, blue, 0};
    // analysis type: 0 for glucose, 1 for urine color
    switch(analysis_type){
        case GLUCOSE:
            TM_ILI9341_Puts(100, 160, "            ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
            TM_ILI9341_Puts(100, 140, "Glucose score:", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
            if (!hardcode)
                interpolation_score = interpolate(test) > 0 ? interpolate(test) : 1;
            else
                interpolation_score = rand() % (1000 + 1 - 900) + 900;
            sprintf(str, "%.2f", interpolation_score);
            TM_ILI9341_Puts(100, 160, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
            break;
        case COLOR:
            TM_ILI9341_Puts(100, 200, "            ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
            TM_ILI9341_Puts(100, 180, "Color score:", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
            if (!hardcode)
                interpolation_score = interpolate_color(test) > 0 ? interpolate_color(test) : 1;
            else
                interpolation_score = rand() % (7 + 1 - 6) + 6;
            sprintf(str, "%.2f", interpolation_score);
            TM_ILI9341_Puts(100, 200, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
            break;
    }
    return interpolation_score;
}


void display_color_info(u16 image[], u16 array_length, COLOR_TYPE color){
    // Clear the score screen
    uint64_t temp = 0;
    uint64_t overall = 0;
   
    switch(color){
        case RGB555:
            for(int i = 0 ; i < array_length; i++){
                red += ((image[i] & 0x7C00) >> 10);
                green += ((image[i] & 0x3E0) >> 5);
                blue += (image[i] & 0x1F);
            }
            red /= ((float)array_length);
            green /= ((float)array_length);
            blue /= ((float)array_length);
            overall = red  << 11;
            overall = overall | (green << 5);
            overall = overall | blue;
        break;
        case RGB565:
            for(int i = 0 ; i < array_length; i++){
                red += ((image[i] & 0xF800) >> 11);
                green += ((image[i] & 0x7E0) >> 5);
                blue += (image[i] & 0x1F);
            }
            red /= ((float)array_length);
            temp = (float)green / (array_length);
            green = (float)temp / (float)64.0 * (float)32.0;
            blue /= ((float)array_length);
            overall = red  << 11;
            temp = (float)green  / (float)32.0 * (float)64.0;
            overall = overall | (temp << 5);
            overall = overall | blue;
        break;
    }
    
    // convert r, g, b to 255 scale
    red = red / 32.0f * 255.0f;
    green = green / 32.0f * 255.0f;
    blue = blue / 32.0f * 255.0f;
    
    float X = 0.0;
    float Y = 0.0;
    float Z = 0.0;
    float L = 0.0;
    float A = 0.0;
    float B = 0.0;
    convertRGBtoXYZ(red, green, blue, &X, &Y, &Z);
    convertXYZtoLab(X, Y, Z, &L, &A, &B);
    
    TM_ILI9341_Puts(0, 140, "        ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(0, 160, "        ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(0, 180, "        ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(0, 140, "R: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(0, 160, "G: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(0, 180, "B: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    
    sprintf(str, "%d", red);
    TM_ILI9341_Puts(20, 140, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    sprintf(str, "%d", green);
    TM_ILI9341_Puts(20, 160, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    sprintf(str, "%d", blue);
    TM_ILI9341_Puts(20, 180, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);

}
