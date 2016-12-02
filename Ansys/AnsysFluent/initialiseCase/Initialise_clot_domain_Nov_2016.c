#include "udf.h"
#define pi 3.141592654

char *clot_name[] = {"clot1","clot2","clot3","clot4"};


DEFINE_INIT(initialise_clot_domain, domain)
{

	/*int CLOTID_array[] = {28,29,30,31};*/

	int L1 = sizeof(clot_name)/sizeof(clot_name[0]);
	int CLOTID_array[L1];
	int loopClot;

	#if RP_HOST

	for (loopClot=0;loopClot<L1;loopClot++)
	{

		CLOTID_array[loopClot] = find_zoneID_by_name(clot_name[loopClot]);
		
	}


	#endif

	host_to_node_int(CLOTID_array,L1);

	int itClot;

	int N_LOOP = sizeof(CLOTID_array)/sizeof(CLOTID_array[0]);


	for (itClot = 0; itClot<N_LOOP;itClot++)
	{
	int clot_ID = CLOTID_array[itClot];

	real CS_V,BS_CS,n_0, epsilon_0, phi_0, k;
	real R_f0, rho_0, rho_fibre, N_AV, dr ,dth, LtVt, L_M;
	unsigned int it, N_CS;
	
	real current_time = RP_Get_Real("flow-time");
	
	Thread *t_clot = Lookup_Thread(domain,clot_ID);
	cell_t c;

	R_f0 = 85; 
	rho_0 = 3; 
	rho_fibre = 0.28; 
	N_AV = 6.02E23;
	dr = 6; 
	dth = 6;
	L_M = 0.04444444444444; 
	
	
	begin_c_loop(c,t_clot)
	{
		
		LtVt = (rho_0/(rho_fibre*1000))*1E21/(pi*R_f0*R_f0); 
		
		CS_V = 2*LtVt/(1E12)*L_M; 
		
		N_CS = R_f0/dr;
		
		BS_CS = 0;
		
		
		for(it = 1; it<N_CS+1; it++)
		{
			BS_CS = BS_CS + pi/asin(dth/(2*it*dr)); 
		}
		
		n_0 = CS_V * BS_CS*1E6*1E15/N_AV; 
		epsilon_0 = 1 - pi*R_f0*R_f0*(LtVt)*1E-21; 
		phi_0 = 1 - epsilon_0;

		k = ((16*pow(phi_0,1.5))*(1 + 56*phi_0*phi_0*phi_0))*1E18/(0.286*(R_f0*R_f0));
		
		
		C_UDSI(c,t_clot,6) = n_0*(1-epsilon_0);
		

		C_UDMI(c,t_clot,1) = R_f0;
		C_UDMI(c,t_clot,2) = k;
		C_UDMI(c,t_clot,9) = n_0;
		C_UDMI(c,t_clot,10) = epsilon_0;
		C_UDMI(c,t_clot,11) = phi_0;

	}
	end_c_loop(c,t_clot);
	
	}
	


}

