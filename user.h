//user controlled definations//

//********************************************************
//Iout = (2.5+-(0.625*Ip/6)V)*(32735/5) 

//VDCout = (Vdc*15/600)V)*(32735/5)
//VACout = (2.5-(Vac*6.8/600)V)*(32735/5)
//transformer ratio 400/173 = 2.3121
//********************************************************

//general variables
int offset = 0;
int VDC_const = 29710; //3801  //[(6.8/15)*2]
int VDC = 0;
int VDCref = 0; //dc-link voltage ref
int VDCref_count = 16367; //200V

int VDCPI_out = 0;
int VDC_FOFout = 0;

int VDCPgain = 9;
int VDCIgain = 3;

//PI general variables
int PWM_max = 0; 
int PWM_min = 0;
int PWM_offset = 0;
int PWM_offset_N = 0;
int IPreError = 0;

// harmonic oscillator variables
int qSin = 0;
int qCos = 32440;        //initial value of 0.99

int OSC_F = 0;
int OSC_Fmax =  45;
int OSC_Fmin = -45; 
int OSC_Fcentral = 1372; //2*PI*F*T, F = 50Hz //1372
                         // 150samples //Fs/F

long theta = 0;     //205800
long theta_2PI = 205800; //samples*OSC_Fcentral
                         //205800

//harmonic oscillator PI variables/////////////////////////
int oscPI_Pgain = 18;   //HARMONIC PROPORTIONAL GAIN   //18
int oscPI_Igain = 3;    //HARMONIC INTEGRAL GAIN       //3
///////////////////////////////////////////////////////////

int oscPI_out = 0;

int Vgrid_Dvalue = 0;
int Vgrid_Qvalue = 0;

int I_DFOFout = 0;
int I_QFOFout = 0;

//abc to dq variables
int Avalue = 0;
int Bvalue = 0;
int Cvalue = 0;
int Dvalue = 0;
int Qvalue = 0;

//Current control PI variables/////////////////////////////////
int current_Pgain = 36;        //CURRENT PROPORTIONAL GAIN //27
int current_Igain = 3;         //CURRENT INTEGRAL GAIN     //7
///////////////////////////////////////////////////////////////

int currentP_Dout = 0;
int currentP_Qout = 0;
int current_Dref = 0;
int current_Qref = 0;

//current reference///////////////////////////////////////////////////
//int current_Dsetpoint = -2250; //-750 == 1Apeak (-for inverter mode)
//////////////////////////////////////////////////////////////////////

int DCLINK_MAX = 30800; //180V max//30800 //dc-link max limit

//current protection variables
int current_max =  4091;  //+6A Ipeak limit
int current_min = -4091;  //-6A Ipeak limit

//first order filter variables//////////////////////
int FOF_PreOut = 0;
int Filter_const_I = 45;   // T/tau // 50Hz/20Khz 
int Filter_const_P = 1260; // T/tau // 50hZ/1.3Khz
int Filter_const_F = 1000;  // T/tau //25Hz/0.5Khz
int Filter_const_H = 600; // T/tau // 20hZ/1.3Khz
////////////////////////////////////////////////////

int _300ms_count = 9000;

int ffd_value = 0;

int sync_tick =  0;
int sync_flag = 0; 

int ffd_max =  1666;   //99% of max value 
int ffd_min = -1666;   //99% of min value 

int ffd_FOFout = 0;
int Osc_FOFout = 0;

int IL_Qvalue = 0;
int IL_Dvalue = 0;

int gridQ_PIout = 0;

int QPI_Pgain = 18;
int QPI_Igain = 3;

int Dh_FOFout = 0;

int Dharmonic = 0;













