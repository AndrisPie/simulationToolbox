/* Include Header */
#include "udf.h"

#define ELindex 3

int CLOTID_array[] = {28,29,30,31};

real volumeAveragedScalar(zoneID,index,memory)
{
	/* Initialising Variables */

	Domain *domain;
	Thread *t_zone;

	real scalar_tot = 0;
	real vol_tot = 0;
	real volume,scalar_value,scalar_average;
	cell_t c;
	domain = Get_Domain(1);

	/* Retrieving zone thread */

	t_zone = Lookup_Thread(domain,zoneID);
	
	/* Cycle through zone */

	#if !RP_HOST
	begin_c_loop(c,t_zone)
	{

		/* If memory = 1, access UDM (user defined memory), else use UDS */

		if (memory == 1)
		{
			scalar_value = C_UDMI(c,t_zone, index);
		}

		else
		{
			scalar_value = C_UDSI(c,t_zone, index);
		}
		
		
		volume = C_VOLUME(c,t_zone);
		
		vol_tot += volume;
		scalar_tot += scalar_value*volume;
	}
	end_c_loop(c,t_zone);
	
	/* Pass value to host to average scalar values and return result */
	
	# if RP_NODE 
	vol_tot = PRF_GRSUM1(vol_tot);
	scalar_tot = PRF_GRSUM1(scalar_tot);
	# endif /* RP_NODE */
	
	#endif /* !RP_HOST */

	node_to_host_real_2(vol_tot,scalar_tot);
	
	#if !RP_NODE
	scalar_average = scalar_tot/vol_tot;
	return scalar_average; 
	#endif /* !RP_NODE */
}



DEFINE_EXECUTE_AT_END(calcVar)
{
/* The core purpose of this UDF : Calculating the prescribable distal pressure from */
/* distal flow into the Wk */

	Domain *domain;
	int i;

#if !RP_HOST
	face_t f;
	cell_t c;
#endif


	/*For every certain number of timesteps, compute averages*/

	if (N_TIME%100 == 0)
	{

		int N_LOOP = sizeof(CLOTID_array)/sizeof(CLOTID_array[0]);
		real averageExtent = 0, averageRes = 0;
		real averageFPLG = 0, averageBPLG = 0;
		real averageFtPA = 0, averageBtPA = 0;
		real averageFPLS = 0, averageBPLS = 0;

		int itClot;

		for (itClot=0;itClot<N_LOOP;itClot++)
		{
			int CLOTID = CLOTID_array[itClot]; 

			averageExtent 	+= volumeAveragedScalar(CLOTID,ELindex,1);
			averageRes 	+= volumeAveragedScalar(CLOTID,2,1);

			averageFtPA 	+= volumeAveragedScalar(CLOTID,0,0);
			averageBtPA 	+= volumeAveragedScalar(CLOTID,1,0);
			averageFPLG 	+= volumeAveragedScalar(CLOTID,2,0);
			averageBPLG 	+= volumeAveragedScalar(CLOTID,3,0);
			averageFPLS 	+= volumeAveragedScalar(CLOTID,4,0);
			averageBPLS 	+= volumeAveragedScalar(CLOTID,5,0);
		}


	#if !RP_NODE
	Message("Average Extent of Lysis\t = %g \n",averageExtent/N_LOOP);	
	Message("Average Resistance\t = %le \n",averageRes/N_LOOP);
	Message("Average Free tPA\t = %g \n",averageFtPA/N_LOOP);
	Message("Average Bnd tPA\t = %g \n",averageBtPA/N_LOOP);
	Message("Average Free PLG\t = %g \n",averageFPLG/N_LOOP);
	Message("Average Bnd PLG\t = %g \n",averageBPLG/N_LOOP);
	Message("Average Free PLS\t = %g \n",averageFPLS/N_LOOP);
	Message("Average Bnd PLS\t = %g \n",averageBPLS/N_LOOP);
	#endif
	}

}