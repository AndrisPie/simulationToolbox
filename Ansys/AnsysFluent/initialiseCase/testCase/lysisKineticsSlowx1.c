#include "udf.h"
#define k_2 0.3  
#define K_M 0.16 
#define k_cat 2.178
#define gamma 0.1 
#define k_move 0
#define pi 3.141592654
#define E_crit 0.95
#define E_a 0.94



real k_a[] = {0.01,0.1,0.1}; 
real k_r[] = {0.0058,3.8,0.05}; 

DEFINE_SOURCE(free_tPA,c,t,dS,eqn)
{

	real Cs_tPA, n_tPA, Cs_PLG, n_PLG, Cs_PLS, n_PLS, n_tot, L_PLS, n_free;
	real R_f,epsilon,CS_V,BS_CS,n_0,phi,k,E_L, phi_0, epsilon_0;
	real R_f0, rho_0, rho_fibre, N_AV, dr ,dth, source, LtVt, L_M;
	real k_crit, t_crit, k_prev, t_prev, dk_dt, perm_constant;
	unsigned int it, N_CS;
	
	real current_time = RP_Get_Real("flow-time");

	Cs_tPA = C_UDSI(c,t,0);
	n_tPA  = C_UDSI(c,t,1);
	Cs_PLG = C_UDSI(c,t,2);
	n_PLG  = C_UDSI(c,t,3);
	Cs_PLS = C_UDSI(c,t,4);
	n_PLS  = C_UDSI(c,t,5);
	n_tot  = C_UDSI(c,t,6);
	L_PLS  = C_UDSI(c,t,7);
	
	n_free = n_tot - (n_tPA + n_PLG + n_PLS);

	n_0 = C_UDMI(c,t,9);
	


	epsilon_0 = C_UDMI(c,t,10);

	E_L = 1 - n_tot/(n_0*(1-epsilon_0));
	if (E_L <0)
	{
		E_L = 0;
	}
	
	if (E_L > 1)
	{
		E_L = 1;
	}
	
	R_f0 = 	C_UDMI(c,t,1); 
	
	phi_0 = C_UDMI(c,t,11);
	
	phi = phi_0*(1-E_L);
	
	epsilon = 1 - phi;
	
	k = ((16*pow(phi,1.5))*(1 + 56*phi*phi*phi))*1E18/(0.286*(R_f0*R_f0));


	if (E_L > E_crit)
	{
		k = 0;
	}

	
	C_UDMI(c,t,0) = epsilon;

	C_UDMI(c,t,2) = k;
	C_UDMI(c,t,3) = E_L;



	source = -(k_a[0]*(Cs_tPA/epsilon)*n_free - k_r[0]*n_tPA)*C_R(c,t);
	
	
	return source;
}

DEFINE_SOURCE(bound_tPA,c,t,dS,eqn)
{

	real source, Cs_tPA, n_tPA, Cs_PLG, n_PLG, Cs_PLS, n_PLS, n_tot, L_PLS, n_free, epsilon, E_L;
	
	Cs_tPA = C_UDSI(c,t,0);
	n_tPA  = C_UDSI(c,t,1);
	Cs_PLG = C_UDSI(c,t,2);
	n_PLG  = C_UDSI(c,t,3);
	Cs_PLS = C_UDSI(c,t,4);
	n_PLS  = C_UDSI(c,t,5);
	n_tot  = C_UDSI(c,t,6);
	L_PLS  = C_UDSI(c,t,7);
	
	epsilon = C_UDMI(c,t,0);
	E_L = C_UDMI(c,t,3);

	n_free = n_tot - (n_tPA + n_PLG + n_PLS);	

	source = (k_a[0]*(Cs_tPA/epsilon)*n_free - k_r[0]*n_tPA)*C_R(c,t);
	

	return source;
}

DEFINE_SOURCE(free_PLG,c,t,dS,eqn)
{

	real source, Cs_tPA, n_tPA, Cs_PLG, n_PLG, Cs_PLS, n_PLS, n_tot, L_PLS, n_free, epsilon, E_L;

	Cs_tPA = C_UDSI(c,t,0);
	n_tPA  = C_UDSI(c,t,1);
	Cs_PLG = C_UDSI(c,t,2);
	n_PLG  = C_UDSI(c,t,3);
	Cs_PLS = C_UDSI(c,t,4);
	n_PLS  = C_UDSI(c,t,5);
	n_tot  = C_UDSI(c,t,6);
	L_PLS  = C_UDSI(c,t,7);
	
	epsilon = C_UDMI(c,t,0);
	
	n_free = n_tot - (n_tPA + n_PLG + n_PLS);	

	source = -(k_a[1]*(Cs_PLG/epsilon)*n_free - k_r[1]*n_PLG)*C_R(c,t);
	E_L = C_UDMI(c,t,3);
	
	
	return source;
}

DEFINE_SOURCE(bound_PLG,c,t,dS,eqn)
{

	real source, Cs_tPA, n_tPA, Cs_PLG, n_PLG, Cs_PLS, n_PLS, n_tot, L_PLS, n_free, epsilon, E_L;

	Cs_tPA = C_UDSI(c,t,0);
	n_tPA  = C_UDSI(c,t,1);
	Cs_PLG = C_UDSI(c,t,2);
	n_PLG  = C_UDSI(c,t,3);
	Cs_PLS = C_UDSI(c,t,4);
	n_PLS  = C_UDSI(c,t,5);
	n_tot  = C_UDSI(c,t,6);
	L_PLS  = C_UDSI(c,t,7);
	
	epsilon = C_UDMI(c,t,0);


	
	n_free = n_tot - (n_tPA + n_PLG + n_PLS);	



	source = ((k_a[1]*(Cs_PLG/epsilon)*n_free - k_r[1]*n_PLG) - k_2*n_PLG*n_tPA/(K_M*(1-epsilon) + n_PLG))*C_R(c,t);
	E_L = C_UDMI(c,t,3);
	if ( E_L > E_crit)
	{
		source = ((k_a[1]*(Cs_PLG/epsilon)*n_free - k_r[1]*n_PLG))*C_R(c,t);
	}
	



	if (n_PLG < 0)
	{
		n_PLG = 0;
		source = 0;
	}


	if (n_tPA < 0)
	{
		n_tPA = 0;
		source = 0;
	}
	
	return source;
}


DEFINE_SOURCE(free_PLS,c,t,dS,eqn)
{

	real source, Cs_tPA, n_tPA, Cs_PLG, n_PLG, Cs_PLS, n_PLS, n_tot, L_PLS, n_free, epsilon, E_L;

	Cs_tPA = C_UDSI(c,t,0);
	n_tPA  = C_UDSI(c,t,1);
	Cs_PLG = C_UDSI(c,t,2);
	n_PLG  = C_UDSI(c,t,3);
	Cs_PLS = C_UDSI(c,t,4);
	n_PLS  = C_UDSI(c,t,5);
	n_tot  = C_UDSI(c,t,6);
	L_PLS  = C_UDSI(c,t,7);
	
	epsilon = C_UDMI(c,t,0);
	
	n_free = n_tot - (n_tPA + n_PLG + n_PLS);	

	source = -(k_a[2]*(Cs_PLS/epsilon)*n_free - k_r[2]*n_PLS - k_r[2]*L_PLS)*C_R(c,t);
	E_L = C_UDMI(c,t,3);
	
	return source;
}


DEFINE_SOURCE(bound_PLS,c,t,dS,eqn)
{

	real source, Cs_tPA, n_tPA, Cs_PLG, n_PLG, Cs_PLS, n_PLS, n_tot, L_PLS, n_free, epsilon,  E_L;

	Cs_tPA = C_UDSI(c,t,0);
	n_tPA  = C_UDSI(c,t,1);
	Cs_PLG = C_UDSI(c,t,2);
	n_PLG  = C_UDSI(c,t,3);
	Cs_PLS = C_UDSI(c,t,4);
	n_PLS  = C_UDSI(c,t,5);
	n_tot  = C_UDSI(c,t,6);
	L_PLS  = C_UDSI(c,t,7);
	
	epsilon = C_UDMI(c,t,0);
	n_free = n_tot - (n_tPA + n_PLG + n_PLS);
	E_L = C_UDMI(c,t,3);




	source =  (k_a[2]*(Cs_PLS/epsilon)*n_free - k_r[2]*n_PLS + k_2*n_PLG*n_tPA/(K_M*(1-epsilon) + n_PLG) - k_cat*gamma*n_PLS)*C_R(c,t);
		
	if ( E_L > E_crit)
	{
		source = (k_a[2]*(Cs_PLS/epsilon)*n_free - k_r[2]*n_PLS - k_cat*gamma*n_PLS)*C_R(c,t);
	}
	
	

	if (n_PLG < 0)
	{
		n_PLG = 0;
		source = 0;
	}


	if (n_tPA < 0)
	{
		n_tPA = 0;
		source = 0;
	}

	return source;
}

DEFINE_SOURCE(tot_BS,c,t,dS,eqn)
{

	real source, Cs_tPA, n_tPA, Cs_PLG, n_PLG, Cs_PLS, n_PLS, n_tot, L_PLS, n_free, epsilon, E_L;

	Cs_tPA = C_UDSI(c,t,0);
	n_tPA  = C_UDSI(c,t,1);
	Cs_PLG = C_UDSI(c,t,2);
	n_PLG  = C_UDSI(c,t,3);
	Cs_PLS = C_UDSI(c,t,4);
	n_PLS  = C_UDSI(c,t,5);
	n_tot  = C_UDSI(c,t,6);
	L_PLS  = C_UDSI(c,t,7);

	epsilon = C_UDMI(c,t,0);
	E_L = C_UDMI(c,t,3);
	source = - k_cat*gamma*n_PLS*C_R(c,t);
	

	return source;

}

DEFINE_SOURCE(L_PLS_woof,c,t,dS,eqn)
{

	real source, Cs_tPA, n_tPA, Cs_PLG, n_PLG, Cs_PLS, n_PLS, n_tot, L_PLS, n_free, epsilon, E_L;
	
	Cs_tPA = C_UDSI(c,t,0);
	n_tPA  = C_UDSI(c,t,1);
	Cs_PLG = C_UDSI(c,t,2);
	n_PLG  = C_UDSI(c,t,3);
	Cs_PLS = C_UDSI(c,t,4);
	n_PLS  = C_UDSI(c,t,5);
	n_tot  = C_UDSI(c,t,6);
	L_PLS  = C_UDSI(c,t,7);

	E_L = C_UDMI(c,t,3);
	
	epsilon = C_UDMI(c,t,0);
	n_free = n_tot - (n_tPA + n_PLG + n_PLS);

	source = (k_cat*gamma*n_PLS - k_r[2]*L_PLS)*C_R(c,t);
	
	
	return source;

}



