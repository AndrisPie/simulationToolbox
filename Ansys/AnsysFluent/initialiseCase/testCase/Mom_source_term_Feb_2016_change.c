#include "udf.h"
#define urf 0.7 


DEFINE_SOURCE(Darcian_x,c,t,dS,eqn)
{


	real source,k, mu;
	
	k = C_UDMI(c,t,2);
	mu = C_MU_L(c,t); 
	
	dS[eqn] = - mu*k;
	
	source = - C_U(c,t)*mu*k;
	return source;
	
}

DEFINE_SOURCE(Darcian_y,c,t,dS,eqn)
{


	real source,k, mu;
	
	k = C_UDMI(c,t,2);
	mu = C_MU_L(c,t); 
	
	dS[eqn] = - mu*k;
	
	source = - C_V(c,t)*mu*k;
	return source;
	
}

DEFINE_SOURCE(Darcian_z,c,t,dS,eqn)
{

	real source,k, mu;
	
	k = C_UDMI(c,t,2);
	mu = C_MU_L(c,t); 
	
	dS[eqn] = - mu*k;
	
	source = - C_W(c,t)*mu*k;
	return source;

}


