/*
-------------------------------------------------------------------------------------
	 Modified Windkessel element UDF

	Description : 	enables the use of a 2- or 3-element windkessel boundary condition.
					The file contains a number of functions that need to be hooked to
					Fluent. Suitable for parallel calculations.

-------------------------------------------------------------------------------------
*/

/* Include Header */
#include "udf.h"
#include "stdlib.h"
#include "var.h"

#define RHO0 1060.0
#define N_OUTLETS 2

#define MCA_OUTLET_NAME	"outlet_mca"
#define R_MCA 3.0208E9
#define C_MCA 4.66e-11
#define Z_MCA 1.792E8

#define ACA_OUTLET_NAME	"outlet_aca"
#define R_ACA 4.4668e9
#define C_ACA 2.33e-11
#define Z_ACA 3.3322e8


/* ----------- */
/* Definitions */
/* ----------- */

/* Define variables */
int n_wk;


cxboolean initialized = FALSE;

/* Windkessel Structure Definition */
typedef struct {
	double Q_current;    	/* Current  time stepproximal flow rate */
	double Q_previous; 	  /* Previous time step proximal flow rate */
	double Q_previous2;  	/* 2 Previous time step proximal flow rate */
	double P_current;		  /* Current time stepproximal pressure */
	double P_previous;		  /* Previous time step proximal pressure */
	double P_previous2;	  /* 2 Previous time step proximal pressure */
	double Pout_current;		/* Current back-pressure */
	double Pout_previous;	/* Previous back-pressure */
	double Pout_previous2;  /* 2 Previous back-pressure */
	double Pc_current;		/* Current back-pressure */
	double Pc_previous;	/* Previous back-pressure */
	double Pc_previous2;  /* 2 Previous back-pressure */
	int id;			/* Windkessel element id */
	double R;		/* Resistance */
	double C;		/* Compliance */
	double Z;		/* Impedance */
} WindKessel;

WindKessel *wk;

/* ----------------------- */
/* NODE <-> Host Processes */
/* ----------------------- */

#if PARALLEL

/* Function to pass data from the host to the compute nodes */
void pass_data_to_node()
{
	int i;				/* Windkessel id */
	double *farray;		/* Float   variable array pointer */
	int *iarray;		/* Integer variable array pointer */

	#define NVAR 15		/* WK state contains 15 variables */

	#if RP_HOST
		/* Allocate array sizes */
		farray = (double*)malloc(n_wk*NVAR*sizeof(double));
		iarray = (int*)malloc(n_wk*sizeof(int));
		/* examine all wk */
		for (i=0;i<n_wk;i++)
		{
			/*Store values in arrays */
			farray[NVAR*i] = wk[i].Q_current;
			farray[NVAR*i+1] = wk[i].Q_previous;
			farray[NVAR*i+2] = wk[i].Q_previous2;
			farray[NVAR*i+3] = wk[i].P_current;
			farray[NVAR*i+4] = wk[i].P_previous;
			farray[NVAR*i+5] = wk[i].P_previous2;
			farray[NVAR*i+6] = wk[i].Pout_current;
			farray[NVAR*i+7] = wk[i].Pout_previous;
			farray[NVAR*i+8] = wk[i].Pout_previous2;
			farray[NVAR*i+9] = wk[i].Pc_current;
			farray[NVAR*i+10] = wk[i].Pc_previous;
			farray[NVAR*i+11] = wk[i].Pc_previous2;
			farray[NVAR*i+12] = wk[i].R;
			farray[NVAR*i+13] = wk[i].C;
			farray[NVAR*i+14] = wk[i].Z;
			iarray[i] = wk[i].id;
		}
		/* Transfer arrays from this host to nodes */
		host_to_node_double(farray,NVAR*n_wk);
		host_to_node_int(iarray,n_wk);
		/* Clear Transfer arrays */
		free(farray);
		free(iarray);
	#else	/* Compute-node process */
		/* Allocate memory for windkessel and incoming arrays */
		farray = (double*)malloc(NVAR*n_wk*sizeof(double));
		iarray = (int*)malloc(n_wk*sizeof(int));
		/* Transfer from host to this node */
		host_to_node_double(farray,NVAR*n_wk);
		host_to_node_int(iarray,n_wk);
		for (i=0;i<n_wk;i++)
		{
			/* Fill windkessel on this node */
			wk[i].Q_current = farray[NVAR*i];
			wk[i].Q_previous = farray[NVAR*i+1];
			wk[i].Q_previous2 = farray[NVAR*i+2];
			wk[i].P_current = farray[NVAR*i+3];
			wk[i].P_previous = farray[NVAR*i+4];
			wk[i].P_previous2 = farray[NVAR*i+5];
			wk[i].Pout_current = farray[NVAR*i+6];
			wk[i].Pout_previous = farray[NVAR*i+7];
			wk[i].Pout_previous2 = farray[NVAR*i+8];
			wk[i].Pc_current = farray[NVAR*i+9];
			wk[i].Pc_previous = farray[NVAR*i+10];
			wk[i].Pc_previous2 = farray[NVAR*i+11];
			wk[i].R = farray[NVAR*i+12];
			wk[i].C = farray[NVAR*i+13];
			wk[i].Z = farray[NVAR*i+14];
			wk[i].id = iarray[i];
		}
		/* Clear Transfer arrays */
		free(farray);
		free(iarray);
	#endif
}
#endif

/* ------------------------- */
/* Fluent Hookable functions */
/* ------------------------- */

int find_zoneID_by_name(char *name)
{

    int i;
    int a = RP_Get_List_Length("a");
    int b = RP_Get_List_Length("b");


    Domain *domain;
    domain = Get_Domain(1);/*assuming not multiphase flow, there is only one domain with number=1*/
    Thread *c_thread;

    
    for (i = 0; i < a; i++)
    {

	if (strcmp(RP_Get_List_Ref_String("b", i),name) == 0)
	{
		
		return RP_Get_List_Ref_Int("a", i);
		
	}

    }



}





DEFINE_ON_DEMAND(variable_initialization)
{
#if !RP_NODE
  int i;
#endif

	if (wk != NULL)
	{
		free(wk);
		wk = NULL;
	}

#if !RP_NODE
		/* retrieve from Scheme instantiation : No. Wk's */
	n_wk = N_OUTLETS;
#endif

 /* Pass parameters not contained in Wk structure to nodes ( not contained in HOST <-> NODE process ) */
	host_to_node_int_1(n_wk);

	if (n_wk < 1)
		Message("\nNo Windkessel model to initialize...\n",n_wk);
	else
	{

		wk = (WindKessel*)malloc(n_wk*sizeof(WindKessel));

		if (n_wk > 0)
		{
#if !RP_NODE
			for (i=0;i<n_wk;i++)
			{
			/* retrieve from Scheme instantiation : First instantiation variables */
				wk[i].Q_current 	= 0;
				wk[i].Q_previous 	= 0;
				wk[i].Q_previous2 	= 0;
				wk[i].P_current 	= 0; /* Proximal pressure */
				wk[i].P_previous 	= 0;
				wk[i].P_previous2 	= 0;
				wk[i].Pout_current 	= 0;  /* Venous pressure */
				wk[i].Pout_previous = 0;
				wk[i].Pout_previous2 = 0;
				wk[i].Pc_current 	= 0;	/* Intramural pressure */
				wk[i].Pc_previous 	= 0;
				wk[i].Pc_previous2 	= 0;
			}
			
			
				wk[0].R 			= R_ACA;
				wk[0].C 			= C_ACA;
				wk[0].Z 			= Z_ACA;
				wk[0].id 			= find_zoneID_by_name(ACA_OUTLET_NAME);
				Message("\nID of ACA is %d \n", find_zoneID_by_name(ACA_OUTLET_NAME));
				
				wk[1].R 			= R_MCA;
				wk[1].C 			= C_MCA;
				wk[1].Z 			= Z_MCA;
				wk[1].id 			= find_zoneID_by_name(MCA_OUTLET_NAME);
				Message("ID of MCA is %d \n", find_zoneID_by_name(MCA_OUTLET_NAME));

				


#endif

#if PARALLEL
			pass_data_to_node();
#endif
		}
	}
#if !RP_NODE
	Message("Windkessel Data Refreshed...\n");
#endif
}

/* Write to File,								*/
/* Execute at end ( of time-step or iteration)	*/
/* -------------------------------------------	*/
DEFINE_EXECUTE_AT_END(write_out)
{

/* Writes Wk information to txt file . Developers purpose*/

#if !RP_NODE
	FILE *fp;
	int i;

	Message("Writing Windkessel currrent information...\n");
	fp = fopen("./wk_values.txt","a");
	fprintf(fp,"%d ",n_wk);
	for (i=0;i<n_wk;i++)
	{
		fprintf(fp,"%le ",wk[i].Q_current);
		fprintf(fp,"%le ",wk[i].Q_previous);
		fprintf(fp,"%le ",wk[i].Q_previous2);
		fprintf(fp,"%le ",wk[i].P_current);
		fprintf(fp,"%le ",wk[i].P_previous);
		fprintf(fp,"%le ",wk[i].P_previous2);
		fprintf(fp,"%le ",wk[i].Pout_current);
		fprintf(fp,"%le ",wk[i].Pout_previous);
		fprintf(fp,"%le ",wk[i].Pout_previous2);
		fprintf(fp,"%le ",wk[i].Pc_current);
		fprintf(fp,"%le ",wk[i].Pc_previous);
		fprintf(fp,"%le ",wk[i].Pc_previous2);
		fprintf(fp,"%d ",wk[i].id);
		fprintf(fp,"%le ",wk[i].R);
		fprintf(fp,"%le ",wk[i].C);
		fprintf(fp,"%le ",wk[i].Z);
	}
	fclose(fp);
#endif
}

/*       Calculate flow rate on a thread	      */
/* -------------------------------------------	*/

real calculate_flow_rate(Thread *tf)
{
	face_t f;
  real q;

	q = 0.0;
#if !RP_HOST
	 		begin_f_loop(f,tf)
	 		{
	 				if PRINCIPAL_FACE_P(f,tf)
	 				{
#if RP_2D
						q += 2*M_PI*F_FLUX(f,tf)/F_R(f,tf);
#else
						q += F_FLUX(f,tf)/F_R(f,tf);
#endif
	 				}
	 		}
	 		end_f_loop(f,tf)

	 		q = PRF_GRSUM1(q);
#endif

	node_to_host_real_1(q);

	return q;
}

/*       Calculate mass flow rate on a thread	      */
/* -------------------------------------------	*/

real calculate_mass_flow_rate(Thread *tf)
{
	face_t f;
  real q;

	q = 0.0;
	#if !RP_HOST
	 		begin_f_loop(f,tf)
	 		{
	 				if PRINCIPAL_FACE_P(f,tf)
	 				{
#if RP_2D
						q += 2*M_PI*F_FLUX(f,tf);
#else
						q += F_FLUX(f,tf);
#endif
	 				}
	 		}
	 		end_f_loop(f,tf)

	 		q = PRF_GRSUM1(q);
	#endif

	node_to_host_real_1(q);

	return q;
}


real first_order_derivative(real x, real xp)
{
	real dt = CURRENT_TIMESTEP;
	real derivative;

	derivative = (x - xp)/dt;

	return derivative;
}

real front_first_order_derivative()
{
	real dt = CURRENT_TIMESTEP;
	real derivative;

	derivative = 1.0/dt;

	return derivative;
}

real back_first_order_derivative(real xp)
{
	real dt = CURRENT_TIMESTEP;
	real derivative;

	derivative = -xp/dt;

	return derivative;
}

real second_order_derivative(real x, real xp,real xp2)
{
	real dt = CURRENT_TIMESTEP;
	real derivative;

	derivative = (1.5*x - 2.0*xp + 0.5*xp2)/dt;

	return derivative;
}

real front_second_order_derivative()
{
	real dt = CURRENT_TIMESTEP;
	real derivative;

	derivative = 1.5/dt;

	return derivative;
}

real back_second_order_derivative(real xp, real xp2)
{
	real dt = CURRENT_TIMESTEP;
	real derivative;

	derivative = (-2.0*xp + 0.5*xp2)/dt;

	return derivative;
}

real derivative(real x, real xp, real xp2)
{
	real d;

	if (rp_dual_time <2)
			d = first_order_derivative(x,xp);
	else
			d = second_order_derivative(x,xp,xp2);

	return d;
}

real front_derivative()
{
	real d;

	if (rp_dual_time <2)
			d = front_first_order_derivative();
	else
			d = front_second_order_derivative();

	return d;
}

real back_derivative(real xp, real xp2)
{
		real d;

		if (rp_dual_time <2)
				d = back_first_order_derivative(xp);
		else
				d = back_second_order_derivative(xp,xp2);

		return d;
}

/* Three elements WindKessel                   */
/* ------------------------------------------- */

/*		(p,q) ----Z-----+------R----- (pout,qout)
									pa
									|
									C
								 |
						 (pc,qc) */

void Wk_pressure_update(int i, Domain *d, real rho)
{
	Thread *tf;
	real p,dpc,dpq;

	tf = Lookup_Thread(d,wk[i].id);

	wk[i].Q_current = calculate_mass_flow_rate(tf)/rho;
	dpc = derivative(wk[i].Pc_current,wk[i].Pc_previous,wk[i].Pc_previous2);
	dpq = derivative(wk[i].Q_current,wk[i].Q_previous,wk[i].Q_previous2);

	p = wk[i].Q_current
	- wk[i].C*back_derivative(wk[i].P_previous,wk[i].P_previous2)
	+ wk[i].Z*(wk[i].C*dpq+wk[i].Q_current/wk[i].R)
	+ wk[i].Pout_current/wk[i].R+wk[i].C*dpc;
	wk[i].P_current = p/(1.0/wk[i].R+wk[i].C*front_derivative());
}

/* Execute at end ( of time-step or iteration) */
/* ------------------------------------------- */

DEFINE_EXECUTE_AT_END(execute)
{
/* The core purpose of this UDF : Calculating the prescribable distal pressure from */
/* distal flow into the Wk */

	Domain *domain;
	Thread *tf;
	Thread *tc;
	real p,dpc,dpq;
	int i;

#if !RP_HOST
	face_t f;
	cell_t c;
	real NV_VEC(A), NV_VEC(Ui);
	real area,tarea;
#endif

#if !RP_NODE
	double *farray;
#endif

	domain = Get_Domain(1);

	/* retrieve from Scheme instantiation : No. Wk's */
#if !RP_NODE
	n_wk = N_OUTLETS;
#endif
	node_to_host_int_1(n_wk);

	/* Loop over Wk's */
	for (i=0;i<n_wk;i++)
	{

		/* Find associated thread face and thread cell */
		tf = Lookup_Thread(domain,wk[i].id);
		tc = THREAD_T0(tf);

		/* Save previous states */
		wk[i].P_previous2 = wk[i].P_previous;
		wk[i].Q_previous2 = wk[i].Q_previous;
		wk[i].Pout_previous2 = wk[i].Pout_previous;
		wk[i].Pc_previous2 = wk[i].Pc_previous;

		wk[i].P_previous = wk[i].P_current;
		wk[i].Q_previous = wk[i].Q_current;
		wk[i].Pout_previous = wk[i].Pout_current;
		wk[i].Pc_previous = wk[i].Pc_current;

/*	Flow determination	*/

		Wk_pressure_update(i,domain,RHO0);

#if !RP_NODE
		Message("Windkessel %d :\tQ = %le m3 and P = %le Pa\n",i+1,wk[i].Q_current,wk[i].P_current);
		Message("\t(Pout = %le Pa,Pc = %le Pa)\n",wk[i].Pout_current,wk[i].Pc_current);
#endif
	}




}

/* Prescribe Wk boundary condition				*/
/* -------------------------------------------  */
DEFINE_PROFILE(pressure,tf,nb)
{
	Thread *t0;
	face_t f;
	cell_t c,c0;
	real NV_VEC(A);
	real Pt, Ps, Pd, Q, dir, v;
	int i;
	cxboolean not_set;
	real p_dyn, rho, vmag;

	Pt = 0.0;
	Pd = 0.0;
	Ps = 0.0;
	Q = 0.0;
	not_set = TRUE;

	for (i=0;i<n_wk;i++)
		if (THREAD_ID(tf)==wk[i].id)
			Ps = wk[i].P_current;
			Q = wk[i].Q_current;
			not_set = FALSE;

	if (not_set)
	{
			Message0("Warning -- WindKessel model is not defined for zone %d\n",THREAD_ID(tf));
	}
	else
	{
		t0 = THREAD_T0(tf);
		begin_f_loop(f,tf)
		{
			if PRINCIPAL_FACE_P(f,tf)
			{
				/* Determine flow */
				c0 = F_C0(f,tf);
				F_AREA(A,f,tf);				/* Face Area vector */
				v = cellVelocityMagnitude(c0,t0);

#if RP_3D
				dir = A[0]*F_U(f,tf)+A[1]*F_V(f,tf)+A[2]*F_W(f,tf);							/* Face flow direction */
#else
				dir = A[0]*F_U(f,tf)+A[1]*F_V(f,tf);							/* Face flow direction */
#endif

				Pd = 0.5*C_R(c0,t0)*v*v;
				Pt = Ps + Pd;

				/* Prescribe pressure */
				if (dir > 0)
				{
					F_PROFILE(f,tf,nb) = Pt - Pd;					/* static pressure */
				}
				else
					F_PROFILE(f,tf,nb) = Pt;               /* total pressure */
			}
		}
		end_f_loop(f,tf)
	}
}
