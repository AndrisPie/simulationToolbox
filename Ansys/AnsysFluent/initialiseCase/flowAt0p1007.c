#include "udf.h"
#define pi 3.141592653589793

DEFINE_PROFILE(flowAt0p1007,t,i)
{


	real R   = 1.8E-3; 
	real mu  = 3.5E-3; 
	real T   = 1.0;    
	real rho = 1060;  
	real omega = 2*pi/T;            	
	real alpha = R*pow((omega*rho/mu),0.5);	
	

	real x_c = 0.0, y_c = 0.0, z_c = 0.0, x[ND_ND], x_d, y_d, z_d;

	/*real time     = RP_Get_Real("flow-time");    */ 				
	real time = 0.1007;
	real Q;
	

	real a_1, b_1, Ber_prev, Bei_prev, B_1r_prev, B_1i_prev,  fact_2n, fact_2n_p_1;
	real Ber, Bei, B_1r, B_1i, Ber1_prev, Bei1_prev, Ber_1, Bei_1;
	real a_2, b_2, a_3, b_3, a_4, b_4, a_5, b_5, nom_r, nom_i, denom_r, denom_i, vel_r,r;
	

	unsigned int n, n_2, it, it_2;
	
	face_t f;
	

	
	real a0 =      4.307  ;
	real a1 =     -0.2427, b1 =      1.125   ;
	real a2 =     -0.9454, b2 =      0.5766  ;
	real a3 =     -0.4395, b3 =     -0.3121  ;
	real a4 =     -0.2677, b4 =    -0.4183  ;
	real a5 =      0.3666, b5 =    -0.2777  ;
	real a6 =     0.07413, b6 =    0.05799  ;
	real a7 =     0.111,   b7 =      -0.01202  ;
	real a8 =     0.0431,  b8 =     0.1057  ;
	real w  =       2*pi  ;
	

	
	Q = (a0 + a1*cos(time*w) + b1*sin(time*w) + a2*cos(2*time*w) + b2*sin(2*time*w) + a3*cos(3*time*w) + b3*sin(3*time*w) + 
		a4*cos(4*time*w) + b4*sin(4*time*w) + a5*cos(5*time*w) + b5*sin(5*time*w) + a6*cos(6*time*w) + b6*sin(6*time*w) + a7*cos(7*time*w) 
		+ b7*sin(7*time*w) + a8*cos(8*time*w) + b8*sin(8*time*w))*1E-6; 
		
		


	a_1 = cos(3*pi/4);
	b_1 = sin(3*pi/4);
	

	Ber_prev = 0;
	Bei_prev = 0;

	B_1r_prev = 0;
	B_1i_prev = 0;
	

	fact_2n_p_1 = 1;
	
	for(n = 0; n < 11;n++)
	{
		fact_2n = 1;    
		if (n>0)
        {
			for (it = 1; it <= 2*n;it++)
			{
				fact_2n = fact_2n*it;
			}
        

        }    
   		fact_2n_p_1 =  fact_2n*(2*n+1);

    Ber = Ber_prev + pow(-1,3*n)*pow((alpha/2),4*n)/(fact_2n*fact_2n);
    Ber_prev = Ber;
    
    Bei = Bei_prev + pow(alpha/2,(4*n+2))*pow(-1,3*n)/(fact_2n_p_1*fact_2n_p_1);
    Bei_prev = Bei;
    
    B_1r = B_1r_prev + a_1*pow((alpha/2),(4*n+1))*pow(-1,3*n)/((fact_2n*fact_2n)*(2*n +1)) - b_1*pow((alpha/2),(4*n+3))*pow(-1,3*n)/((fact_2n_p_1*fact_2n_p_1)*(2*n +2));
    B_1r_prev = B_1r;
    
    B_1i = B_1i_prev + b_1*pow((alpha/2),(4*n+1))*pow(-1,3*n)/((fact_2n*fact_2n)*(2*n +1)) + a_1*pow((alpha/2),(4*n+3))*pow(-1,3*n)/((fact_2n_p_1*fact_2n_p_1)*(2*n +2));
    B_1i_prev = B_1i;
	}
	

	
	 begin_f_loop(f,t)
	 {

		F_CENTROID(x,f,t);
		
		x_d = x[0] - x_c;
		y_d = x[1] - y_c;
		
		if (ND_ND == 3)
		{
		z_d = x[2] - z_c;
		}
		
		else 
		{
		z_d = 0;
		}
		r = pow(x_d*x_d + y_d*y_d + z_d*z_d,0.5);

		
		Ber1_prev = 0;
        Bei1_prev = 0;
        
		for (n_2 = 0; n_2<10; n_2++)
		{

			fact_2n = 1;
            if (n_2 > 0)
            {   
                for (it_2 = 1; it_2 <= 2*n_2; it_2++)
				{
                    fact_2n = fact_2n*it_2;
                }
            }
            
            fact_2n_p_1 =  fact_2n*(2*n_2+1);


			Ber_1 = Ber1_prev + pow(-1,3*n_2)*pow((alpha*r/(2*R)),4*n_2)/(fact_2n*fact_2n);
			Bei_1 = Bei1_prev + pow((alpha*r/(2*R)),(4*n_2+2))*pow(-1,3*n_2)/(fact_2n_p_1*fact_2n_p_1);
        
			Ber1_prev = Ber_1;
			Bei1_prev = Bei_1;


        }    

				
		a_2 = Ber_1;
        b_2 = Bei_1;
        
        nom_r = 1 - (Ber*a_2 + b_2*Bei)/(Ber*Ber + Bei*Bei);
        nom_i = (a_2*Bei - b_2*Ber)/(Ber*Ber + Bei*Bei);
        
        a_3 = B_1r; 
		b_3 = B_1i;
        a_4 = Ber; 
		b_4 = Bei;
        a_5 = a_4*a_1 - b_4*b_1;
		b_5 = b_4*a_1 + b_1*a_4;
        
        denom_r = 1 - (2/alpha)*(a_3*a_5+b_5*b_3)/(a_5*a_5 + b_5*b_5); 
        denom_i = -(2/alpha)*(b_3*a_5 - b_5*a_3)/(a_5*a_5 + b_5*b_5);
                
		vel_r = (Q/(pi*R*R))*(nom_r*denom_r + nom_i*denom_i)/(denom_r*denom_r + denom_i*denom_i);
		
		if (r > R)
		{

			vel_r = 0;
		}		



		F_PROFILE(f,t,i) = vel_r;
		
	

	 }
	 end_f_loop(f,t)


	 
}
