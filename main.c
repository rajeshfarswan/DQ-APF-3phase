//**********************************************************************//
// THREE PHASE ACTIVE POWER FILTER BASED ON SINE PWM USING DQ CONTROL   //
//**********************************************************************//

//include files
#include "main.h"
#include "user.h"
#include "asmMath.h"

int main(void) //main
{

init(); //processor initialisation routine

starting(); //variable initialisations

  while(1) //main loop 
	{

//current control//
    if(current_Flag) //20Khz
       {     
           //read inverter currents
          asm("disi #0x3FFF");
           {
            Avalue = asmADC(0x0808) - offset; //adc channel 8 
            Bvalue = asmADC(0x0909) - offset; //adc channel 9
            Cvalue = asmADC(0x0A0A) - offset; //adc channel 10
           } 
          asm("disi #0x0000");
           //
           //detect peak current // set if fault
          if(Avalue > current_max) fault_Flag = 1; 
          if(Bvalue > current_max) fault_Flag = 1;
          if(Cvalue > current_max) fault_Flag = 1;  

          if(Avalue < current_min) fault_Flag = 1;  
          if(Bvalue < current_min) fault_Flag = 1;
          if(Cvalue < current_min) fault_Flag = 1; 
           // 

			asmABCtoDQ(); //convert feedback currents a-b-c to dq frame

            //current D PI //use D ref generated from PLL block
            IPreError = currentP_Dout;
            currentP_Dout = asmPIcontroller(current_Dref,Dvalue,current_Pgain,current_Igain);

            Dvalue = currentP_Dout + ffd_FOFout; // add dc-link voltage feedforward feedback

            if(Dvalue >= PWM_offset) Dvalue = PWM_offset;
            if(Dvalue <= PWM_offset_N) Dvalue = PWM_offset_N;
       
            //current Q PI //use Q ref generated from PLL block
            IPreError = currentP_Qout;
            currentP_Qout = asmPIcontroller(current_Qref,Qvalue,current_Pgain,current_Igain);

            Qvalue = currentP_Qout; //copy Q value

            if(Qvalue >= PWM_offset) Qvalue = PWM_offset;     //normalise PI output
            if(Qvalue <= PWM_offset_N) Qvalue = PWM_offset_N;
      
            asmDQtoABC(); //current d-q PI output to three phase refs

            asmPWM(); //generate three phase duty cycle          
            
                               current_Flag = 0;   
                                  } 
//current control//

//D-Q PLL//        
	if(pll_Flag) //1.3Khz
		{
          asm("disi #0x3FFF");
           {
          Avalue = asmADC(0x0505) - offset; //adc channel 5 //read grid voltages  
          Bvalue = asmADC(0x0606) - offset; //adc channel 6    
          Cvalue = asmADC(0x0707) - offset; //adc channel 7
            } 
          asm("disi #0x0000");
          //

          asmABCtoDQ(); //three phase to d-q voltage feedbacks

          Vgrid_Dvalue = Dvalue; //copy grid voltage d-q values
          Vgrid_Qvalue = Qvalue;
          //

          asm("disi #0x3FFF"); //read load currents
            {
          Avalue = asmADC(0x0b0b) - offset; //adc channel 11   
          Bvalue = asmADC(0x0c0c) - offset; //adc channel 12    
          Cvalue = asmADC(0x0d0d) - offset; //adc channel 13
            } 
               asm("disi #0x0000");
          //
          asmABCtoDQ(); //convert feedback currents a-b-c to dq frame
 
          IL_Dvalue = Dvalue; //copy load current D value
          IL_Qvalue = Qvalue; //copy load current Q value
          //

          //PLL filter for grid voltage Q value
          FOF_PreOut = Osc_FOFout;
          Osc_FOFout = asmFO_Filter(Vgrid_Qvalue,Filter_const_P);
          
         /*PLL PI************************************************************/
          IPreError = oscPI_out;
          oscPI_out = asmPIcontroller(0,Osc_FOFout,oscPI_Pgain,oscPI_Igain); //set grid voltage q to zero
          oscPI_out = asmINT_MPQ(OSC_Fcentral,oscPI_out);

          if(oscPI_out >= OSC_Fmax) oscPI_out = OSC_Fmax;
          if(oscPI_out <= OSC_Fmin) oscPI_out = OSC_Fmin;
         /*PLL PI************************************************************/ 

              //High pass filter for load current D value
              FOF_PreOut = Dh_FOFout;
              Dh_FOFout = asmFO_Filter(IL_Dvalue,Filter_const_H);
              Dharmonic = IL_Dvalue - Dh_FOFout; //Extract load current harmonic D component
 
              //Inverter DC link PI control
              IPreError = VDCPI_out;
              VDCPI_out = asmPIcontroller(VDCref,VDC_FOFout,VDCPgain,VDCIgain); //set inverter dc-link to ref

              current_Dref = VDCPI_out + Dharmonic; //Inverter current D ref. = dc-link PI output + harmonic current D component

              if(current_Dref >= current_max) current_Dref = current_max;
              if(current_Dref <= current_min) current_Dref = current_min; 

              //
              current_Qref = IL_Qvalue; //set inverter current Q ref. = load current Q component

              if(current_Qref >= current_max) current_Qref = current_max;
              if(current_Qref <= current_min) current_Qref = current_min;

              //
				         pll_Flag = 0;
						  }
//PLL//	


//feed forward and soft start//
		if(ffd_Flag) //0.5Khz   
      		{  
              asmDClink(); //monitor dc link and generate feedforward
              
              //DC link filter
              FOF_PreOut = VDC;
              VDC_FOFout = asmFO_Filter(VDC,Filter_const_F);

              //ffd filter
              FOF_PreOut = ffd_FOFout;
              ffd_FOFout = asmFO_Filter(ffd_value,Filter_const_F);

                            //soft start and PLL sync delay
                            sync_tick++;

                            if((!sync_flag) && (sync_tick >= _300ms_count)) 
                                   {  
                                       if(ffd_value >= ffd_max) fault_Flag = 1;
                                       if(ffd_value <= ffd_min) fault_Flag = 1;

                                       if(ffd_value == 0) fault_Flag = 1; 
           
                                       SET = 0x0077; //all three switces are enabled
                                       sync_flag = 1; //reset sync flag 
                                       sync_tick = 0;
                                    }   

                           if(sync_flag) //if PLL sync done
                                   {   
                                       if(ffd_value >= ffd_max) fault_Flag = 1;
                                       if(ffd_value <= ffd_min) fault_Flag = 1;

                                       if(ffd_value == 0) fault_Flag = 1; 

                                       VDCref++; //initiate soft start //update inverter dc-link ref

                                           if(VDCref >= VDCref_count)
                                               { 
                                                 VDCref = VDCref_count; //set inverter dc-link ref
                           
                                                   }
                                       sync_tick = 0;
                                       }
                             //soft start and sync delay  
                
       		                    ffd_Flag = 0; //reset flag
       							}
//feed forward and soft start//

    			ClrWdt();
    		}//while end////////////////////////////////////////////////////////

  
		ClrWdt();
	} //main end////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////


//T1 interrupt for oscillator tracking
		void _ISRFAST __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) //99.88Khz
  			{   
                 //harmonic oscillator for generating Sine and Cos ref.  
 
                  OSC_F =  oscPI_out  + OSC_Fcentral; //determine angular frequency
                 
                  theta = theta + OSC_F;              //increment theta angle
     
         		    if(theta >= theta_2PI)            //reset harmonic oscillator
            		    {
           				qSin = 0;
           				qCos = 32440; //0.99
           				theta = 0;
              				}
              		else 
                    asmHARMONIC();

     			T1us_Flag = 0;
                
   					} //T1 interupt end

///////////////////////////////////////////////////////////////////////

		//fault interrupt
		void _ISR __attribute__((interrupt, no_auto_psv)) _FLTBInterrupt(void)
  			 {
     			PWMenable = 0; //disable pwm
     			SET = 0;       //all switches off
          
     			RL2_ON = 0;    //open all relays 
               
     			RL3_ON = 0;      
     			RL4_ON = 0; 
     			RL5_ON = 0;     
  
		fault_Flag = 0;        //reset flag
            
   			}//fault end

//////////////////////////////////////////////////////////////////////

			//initial startup routine
			void starting(void)
  				{
                    PWM_offset = PWM_PERIOD; //initialise PWM period value
                    PWM_offset_N = -PWM_PERIOD;

					PWM_max = PWM_offset*8; //PI saturation values
					PWM_min = -PWM_offset*8;
					SET = 0; //initialise PWM control registers
					PWMenable = 0; //reset PWM control register
					 //
					FAULT_ENABLE = 1; //0x000f //reset fault register
					delay(30); //delay 30ms
					ADC_ON = 1;
					//precharging init
					RL1_ON = 1;  //precharging enable
					delay(1500); //delay 1500ms
					//precharging init ends
					
					offset = asmADC(0x0e0e); //2.5V offset //read adc channel 14
					//
					//initiates startup
					RL1_ON = 0;  //precharging disable
					delay(30); //delay 30ms
					RL2_ON = 1;  //bypass precharging
					delay(30); //delay 30ms
					
					//set pwm
					PWM1 = PWM_offset;
					PWM2 = PWM_offset;
					PWM3 = PWM_offset;
					//SET = 0x0077; //all three switces are enabled
					//
					RL3_ON = 1;  //connect to grid
					delay(20); //delay 30ms
					RL4_ON = 1;
					delay(20); //delay 30ms
					RL5_ON = 1;
					
					delay(30); //delay 30ms
					//PWM_InterruptEnable = 0;
					PWMenable = 1;
					T1ON = 1;
                    T2ON = 1;
                    T3ON = 1;
                    T4ON = 1;
                    T5ON = 1;
                    
					// 
					  	}//startup routine end

///////////////////////////////////////////////////////////////////////

			











