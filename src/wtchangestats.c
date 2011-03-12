#include "wtchangestats.h"

/********************  changestats:   A    ***********/

/*****************                       
 stat: absdiff(_nonzero)
*****************/
WtD_CHANGESTAT_FN(d_absdiff_nonzero){ 
  double p = INPUT_ATTRIB[0];
  
  EXEC_THROUGH_TOGGLES({
      if(p==1.0){
	CHANGE_STAT[0] += fabs(INPUT_ATTRIB[TAIL] - INPUT_ATTRIB[HEAD])*((NEWWT!=0)-(OLDWT!=0));
      } else {
	CHANGE_STAT[0] += pow(fabs(INPUT_ATTRIB[TAIL] - INPUT_ATTRIB[HEAD]), p)*((NEWWT!=0)-(OLDWT!=0));
      }
    });
}

/*****************                       
 stat: absdiff(_sum)
*****************/
WtD_CHANGESTAT_FN(d_absdiff_sum){ 
  double p = INPUT_ATTRIB[0];
  
  EXEC_THROUGH_TOGGLES({
      if(p==1.0){
	CHANGE_STAT[0] += fabs(INPUT_ATTRIB[TAIL] - INPUT_ATTRIB[HEAD])*(NEWWT-OLDWT);
      } else {
	CHANGE_STAT[0] += pow(fabs(INPUT_ATTRIB[TAIL] - INPUT_ATTRIB[HEAD]), p)*(NEWWT-OLDWT);
      }
    });
}

/*****************
 stat: absdiffcat(_nonzero)
*****************/
WtD_CHANGESTAT_FN(d_absdiffcat_nonzero){ 
  double change, absdiff, NAsubstitute, tailval, headval;
  Vertex ninputs;
  int j;
  
  ninputs = N_INPUT_PARAMS - N_NODES;
  NAsubstitute = INPUT_PARAM[ninputs-1];
  
  EXEC_THROUGH_TOGGLES({
      change = (NEWWT!=0)-(OLDWT!=0);
      tailval = INPUT_ATTRIB[TAIL-1];
      headval = INPUT_ATTRIB[HEAD-1];
      if (tailval == NAsubstitute ||  headval == NAsubstitute) absdiff = NAsubstitute;
      else absdiff = fabs(tailval - headval);
      if (absdiff>0){
	for (j=0; j<N_CHANGE_STATS; j++){
	  CHANGE_STAT[j] += (absdiff==INPUT_PARAM[j]) ? change : 0.0;
	}
      }
    });
}

/*****************
 stat: absdiffcat(_sum)
*****************/
WtD_CHANGESTAT_FN(d_absdiffcat_sum){ 
  double change, absdiff, NAsubstitute, tailval, headval;
  Vertex ninputs;
  int j;
  
  ninputs = N_INPUT_PARAMS - N_NODES;
  NAsubstitute = INPUT_PARAM[ninputs-1];
  
  EXEC_THROUGH_TOGGLES({
      change = NEWWT-OLDWT;
      tailval = INPUT_ATTRIB[TAIL-1];
      headval = INPUT_ATTRIB[HEAD-1];
      if (tailval == NAsubstitute ||  headval == NAsubstitute) absdiff = NAsubstitute;
      else absdiff = fabs(tailval - headval);
      if (absdiff>0){
	for (j=0; j<N_CHANGE_STATS; j++){
	  CHANGE_STAT[j] += (absdiff==INPUT_PARAM[j]) ? change : 0.0;
	}
      }
    });
}


/*****************
 stat: atleast
*****************/
WtD_CHANGESTAT_FN(d_atleast){
  EXEC_THROUGH_TOGGLES({
      CHANGE_STAT[0] += (NEWWT>=INPUT_ATTRIB[0]) - (OLDWT>=INPUT_ATTRIB[0]);
    });
}

/********************  changestats:   C    ***********/

/*****************
 stat: cyclicweights(_max)
*****************/

WtD_FROM_S_FN(d_cyclicweights_max)

WtS_CHANGESTAT_FN(s_cyclicweights_max){ 
  Edge e1, e2;
  Vertex tail, head, node3;
  
  CHANGE_STAT[0]=0;
  for (tail=1; tail <= N_NODES; tail++){
    EXEC_THROUGH_FOUTEDGES(tail, e1, head, {
      double best_path = 0;
      EXEC_THROUGH_OUTEDGES(head, e2, node3, { 
	best_path = fmax(best_path, fmin(GETWT(node3,tail),GETWT(head,node3)));
	})
      CHANGE_STAT[0] += fmin(best_path, GETWT(tail,head));
      })
  }
}

/*****************
 stat: cyclicweights(_sum)
*****************/

WtD_FROM_S_FN(d_cyclicweights_sum)

WtS_CHANGESTAT_FN(s_cyclicweights_sum){ 
  Edge e1, e2;
  Vertex tail, head, node3;
  
  CHANGE_STAT[0]=0;
  for (tail=1; tail <= N_NODES; tail++){
    EXEC_THROUGH_FOUTEDGES(tail, e1, head, {
      double path_strength = 0;
      EXEC_THROUGH_OUTEDGES(head, e2, node3, { 
	path_strength += fmin(GETWT(node3,tail),GETWT(head,node3));
	})
      CHANGE_STAT[0] += fmin(path_strength, GETWT(tail,head));
      })
  }
}

/*****************
 stat: cyclicweights(_threshold)
*****************/

WtD_FROM_S_FN(d_cyclicweights_threshold)

WtS_CHANGESTAT_FN(s_cyclicweights_threshold){ 
  Edge e1, e2;
  Vertex tail, head, node3;
  
  CHANGE_STAT[0]=0;
  for (tail=1; tail <= N_NODES; tail++){
    EXEC_THROUGH_FOUTEDGES(tail, e1, head, {
	if(GETWT(tail,head)<=INPUT_ATTRIB[0]) break;
	EXEC_THROUGH_OUTEDGES(head, e2, node3, { 
	    if(GETWT(node3,tail)>INPUT_ATTRIB[0] && GETWT(head,node3)>INPUT_ATTRIB[0]){
	      CHANGE_STAT[0]++; 
	      break;
	    }
	  })
	  })
      }
}


/********************  changestats:   G    ***********/

/*****************
 stat: greaterthan
*****************/
WtD_CHANGESTAT_FN(d_greaterthan){
  EXEC_THROUGH_TOGGLES({
      CHANGE_STAT[0] += (NEWWT>INPUT_ATTRIB[0]) - (OLDWT>INPUT_ATTRIB[0]);
  });
}

/********************  changestats:   I    ***********/

/*****************
 stat: ininterval
*****************/
WtD_CHANGESTAT_FN(d_ininterval){
  EXEC_THROUGH_TOGGLES({
      CHANGE_STAT[0] += ((INPUT_ATTRIB[2] ? NEWWT>INPUT_ATTRIB[0] : NEWWT>=INPUT_ATTRIB[0]) && (INPUT_ATTRIB[3] ? NEWWT<INPUT_ATTRIB[1] : NEWWT<=INPUT_ATTRIB[1])) - ((INPUT_ATTRIB[2] ? OLDWT>INPUT_ATTRIB[0] : OLDWT>=INPUT_ATTRIB[0]) && (INPUT_ATTRIB[3] ? OLDWT<INPUT_ATTRIB[1] : OLDWT<=INPUT_ATTRIB[1]));
    });
}


/********************  changestats:   M    ***********/

/*****************
 stat: mutual (product a.k.a. correlation)
*****************/
WtD_CHANGESTAT_FN(d_mutual_wt_product){
  EXEC_THROUGH_TOGGLES({
      double htweight = GETWT(HEAD,TAIL);
      CHANGE_STAT[0] += (NEWWT*htweight) - (OLDWT*htweight);
    });
}

/*****************
 stat: mutual (geometric mean)
*****************/
WtD_CHANGESTAT_FN(d_mutual_wt_geom_mean){
  EXEC_THROUGH_TOGGLES({
      double htweight = GETWT(HEAD,TAIL);
      CHANGE_STAT[0] += sqrt(NEWWT*htweight) - sqrt(OLDWT*htweight);
    });
}

/*****************
 stat: mutual (minimum)
*****************/
WtD_CHANGESTAT_FN(d_mutual_wt_min){
  EXEC_THROUGH_TOGGLES({
      double htweight = GETWT(HEAD,TAIL);
      CHANGE_STAT[0] += fmin(NEWWT,htweight) - fmin(OLDWT,htweight);
    });
}


/*****************
 stat: mutual (-|yij-yji|)
*****************/
WtD_CHANGESTAT_FN(d_mutual_wt_nabsdiff){
  EXEC_THROUGH_TOGGLES({
      double htweight = GETWT(HEAD,TAIL);
      CHANGE_STAT[0] -= fabs(NEWWT-htweight) - fabs(OLDWT-htweight);
  });
}

/********************  changestats:   N    ***********/

/*****************
 stat: nodecov (nonzero)
*****************/
WtD_CHANGESTAT_FN(d_nodecov_nonzero){ 
  EXEC_THROUGH_TOGGLES({
      CHANGE_STAT[0] += (INPUT_ATTRIB[TAIL-1] + INPUT_ATTRIB[HEAD-1])*((NEWWT!=0)-(OLDWT!=0));
  });
}

/*****************
 stat: nodecov (sum)
*****************/
WtD_CHANGESTAT_FN(d_nodecov_sum){ 
  EXEC_THROUGH_TOGGLES({
      CHANGE_STAT[0] += (INPUT_ATTRIB[TAIL-1] + INPUT_ATTRIB[HEAD-1])*(NEWWT-OLDWT);
  });
}

/*****************
 stat: nodeicov (nonzero)
*****************/
WtD_CHANGESTAT_FN(d_nodeicov_nonzero){ 
  EXEC_THROUGH_TOGGLES({
      CHANGE_STAT[0] += INPUT_ATTRIB[HEAD-1]*((NEWWT!=0)-(OLDWT!=0));
  });
}

/*****************
 stat: nodeicov (sum)
*****************/
WtD_CHANGESTAT_FN(d_nodeicov_sum){ 
  EXEC_THROUGH_TOGGLES({
      CHANGE_STAT[0] += INPUT_ATTRIB[HEAD-1]*(NEWWT-OLDWT);
  });
}

/*****************
 stat: nodeocov (nonzero)
*****************/
WtD_CHANGESTAT_FN(d_nodeocov_nonzero){ 
  EXEC_THROUGH_TOGGLES({
      CHANGE_STAT[0] += INPUT_ATTRIB[TAIL-1]*((NEWWT!=0)-(OLDWT!=0));
  });
}

/*****************
 stat: nodeocov (sum)
*****************/
WtD_CHANGESTAT_FN(d_nodeocov_sum){ 
  EXEC_THROUGH_TOGGLES({
      CHANGE_STAT[0] += INPUT_ATTRIB[TAIL-1]*(NEWWT-OLDWT);
  });
}

/*****************
 stat: nodefactor (nonzero)
*****************/
WtD_CHANGESTAT_FN(d_nodefactor_nonzero){ 
  double s, factorval;
  int j, tailattr, headattr;
  
  
  EXEC_THROUGH_TOGGLES({
      s = (NEWWT!=0) - (OLDWT!=0);
      tailattr = INPUT_ATTRIB[TAIL-1];
      headattr = INPUT_ATTRIB[HEAD-1];
      for (j=0; j < N_CHANGE_STATS; j++){
	factorval = INPUT_PARAM[j];
	if (tailattr == factorval) CHANGE_STAT[j] += s;
	if (headattr == factorval) CHANGE_STAT[j] += s;
      }
    });
}

/*****************
 stat: nodefactor (sum)
*****************/
WtD_CHANGESTAT_FN(d_nodefactor_sum){ 
  double s, factorval;
  int j, tailattr, headattr;
  
  
  EXEC_THROUGH_TOGGLES({
    s = NEWWT - OLDWT;
    tailattr = INPUT_ATTRIB[TAIL-1];
    headattr = INPUT_ATTRIB[HEAD-1];
    for (j=0; j < N_CHANGE_STATS; j++){
      factorval = INPUT_PARAM[j];
      if (tailattr == factorval) CHANGE_STAT[j] += s;
      if (headattr == factorval) CHANGE_STAT[j] += s;
    }
  });
}

/*****************
 stat: node i[n] corr 
*****************/
WtD_CHANGESTAT_FN(d_nodeicorr){
  EXEC_THROUGH_TOGGLES({
      for(Vertex i=1; i<=N_NODES; i++){
	if(i==TAIL) continue;
	double yih = GETWT(i,HEAD);
	CHANGE_STAT[0] += (NEWWT*yih) - (OLDWT*yih);
      }
    });
}

/*****************
 stat: nodeifactor (nonzero)
*****************/
WtD_CHANGESTAT_FN(d_nodeifactor_nonzero){ 
  double s, factorval;
  int j, headattr;
  
  
  EXEC_THROUGH_TOGGLES({
      s = (NEWWT!=0) - (OLDWT!=0);
      headattr = INPUT_ATTRIB[TAIL-1];
      for (j=0; j < N_CHANGE_STATS; j++){
	factorval = INPUT_PARAM[j];
	if (headattr == factorval) CHANGE_STAT[j] += s;
      }
    });
}

/*****************
 stat: nodeifactor (sum)
*****************/
WtD_CHANGESTAT_FN(d_nodeifactor_sum){ 
  double s, factorval;
  int j, headattr;
  
  
  EXEC_THROUGH_TOGGLES({
    s = NEWWT - OLDWT;
    headattr = INPUT_ATTRIB[TAIL-1];
    for (j=0; j < N_CHANGE_STATS; j++){
      factorval = INPUT_PARAM[j];
      if (headattr == factorval) CHANGE_STAT[j] += s;
    }
  });
}

/*****************
 stat: node o[ut] corr 
*****************/
WtD_CHANGESTAT_FN(d_nodeocorr){
  EXEC_THROUGH_TOGGLES({
      for(Vertex j=1; j<=N_NODES; j++){
	if(j==HEAD) continue;
	double ytj = GETWT(TAIL,j);
	CHANGE_STAT[0] += (NEWWT*ytj) - (OLDWT*ytj);
      }
    });
}

/*****************
 stat: nodeofactor (nonzero)
*****************/
WtD_CHANGESTAT_FN(d_nodeofactor_nonzero){ 
  double s, factorval;
  int j, tailattr;
  
  
  EXEC_THROUGH_TOGGLES({
      s = (NEWWT!=0) - (OLDWT!=0);
      tailattr = INPUT_ATTRIB[TAIL-1];
      for (j=0; j < N_CHANGE_STATS; j++){
	factorval = INPUT_PARAM[j];
	if (tailattr == factorval) CHANGE_STAT[j] += s;
      }
    });
}

/*****************
 stat: nodeofactor (sum)
*****************/
WtD_CHANGESTAT_FN(d_nodeofactor_sum){ 
  double s, factorval;
  int j, tailattr;
  
  
  EXEC_THROUGH_TOGGLES({
    s = NEWWT - OLDWT;
    tailattr = INPUT_ATTRIB[TAIL-1];
    for (j=0; j < N_CHANGE_STATS; j++){
      factorval = INPUT_PARAM[j];
      if (tailattr == factorval) CHANGE_STAT[j] += s;
    }
  });
}


/*****************
 stat: nonzero
*****************/
WtD_CHANGESTAT_FN(d_nonzero){
  EXEC_THROUGH_TOGGLES({
        CHANGE_STAT[0] += (NEWWT!=0) - (OLDWT!=0);
  });
}

/*****************
 stat: nsumlogfactorial
 Turns a Poisson-reference or geometric-reference ERGM into a Conway-Maxwell-Poisson distribution
*****************/
WtD_CHANGESTAT_FN(d_nsumlogfactorial){
  EXEC_THROUGH_TOGGLES({
      CHANGE_STAT[0] -= lgamma1p(NEWWT)-lgamma1p(OLDWT);
  });
}

/********************  changestats:   S    ***********/

/*****************
 stat: sum
*****************/
WtD_CHANGESTAT_FN(d_sum){
  EXEC_THROUGH_TOGGLES({
      CHANGE_STAT[0] += NEWWT-OLDWT;
  });
}

/*****************
 stat: sum(with power)
*****************/
WtD_CHANGESTAT_FN(d_sum_pow){
  double p = INPUT_ATTRIB[0];
  
  EXEC_THROUGH_TOGGLES({
      CHANGE_STAT[0] += pow(NEWWT,p)-pow(OLDWT,p);
    });
}

/********************  changestats:   T    ***********/

/*****************
 stat: transitiveweights(_max)
*****************/

/* WtD_CHANGESTAT_FN(d_transitiveweights_max){ */
/*   Edge e1, e2; */
/*   Vertex node3; */
/*   EXEC_THROUGH_TOGGLES({ */
/*       /\* Changed dyad as the focus dyad. *\/ */
/*       double best_path = 0; */
/*       EXEC_THROUGH_INEDGES(HEAD, e1, node3, {  */
/* 	best_path = fmax(best_path, fmin(GETWT(TAIL,node3),GETWT(node3,HEAD))); */
/* 	}) */
/*       CHANGE_STAT[0] += fmin(best_path, NEWWT) - fmin(best_path, OLDWT); */

/*       /\* Changed dyad as a part of a two-path.  */
/* 	 A dyad (i,j) is potentially affected by (tail,head) iff: */
/* 	 y(i,j)>0 & (tail=i & (head,j)>0 | head=j & (i,tail)>0). */
/*       *\/ */
/*       /\* For all ties (tail=i,j)>0, *\/ */
/*       EXEC_THROUGH_OUTEDGES(TAIL, e1, node3, { */
/* 	  double ijwt = GETWT(HEAD,node3); */
/* 	  /\* If (head,j)>0), (tail,head) can affect (i,j). *\/ */
/* 	  if(ijwt>0){ */
/* 	    best_path = 0; */
/* 	    EXEC_THROUGH_INEDGES(HEAD, e1, node3, {  */
/* 		best_path = fmax(best_path, fmin(GETWT(TAIL,node3),GETWT(node3,HEAD))); */
/* 	      }) */
/* 	      CHANGE_STAT[0] += fmin(best_path, NEWWT) - fmin(best_path, OLDWT); */
/* 	  } */
/* 	} */
/*   }); */
/* } */

WtD_FROM_S_FN(d_transitiveweights_max)

WtS_CHANGESTAT_FN(s_transitiveweights_max){ 
  Edge e1, e2;
  Vertex tail, head, node3;
  
  CHANGE_STAT[0]=0;
  for (tail=1; tail <= N_NODES; tail++){
    EXEC_THROUGH_FOUTEDGES(tail, e1, head, {
      double best_path = 0;
      EXEC_THROUGH_INEDGES(head, e2, node3, { 
	best_path = fmax(best_path, fmin(GETWT(tail,node3),GETWT(node3,head)));
	})
      CHANGE_STAT[0] += fmin(best_path, GETWT(tail,head));
      })
  }
}

/*****************
 stat: transitiveweights(_sum)
*****************/

WtD_FROM_S_FN(d_transitiveweights_sum)

WtS_CHANGESTAT_FN(s_transitiveweights_sum){ 
  Edge e1, e2;
  Vertex tail, head, node3;
  
  CHANGE_STAT[0]=0;
  for (tail=1; tail <= N_NODES; tail++){
    EXEC_THROUGH_FOUTEDGES(tail, e1, head, {
      double path_strength = 0;
      EXEC_THROUGH_INEDGES(head, e2, node3, { 
	path_strength += fmin(GETWT(tail,node3),GETWT(node3,head));
	})
      CHANGE_STAT[0] += fmin(path_strength, GETWT(tail,head));
      })
  }
}

/*****************
 stat: transitiveweights (threshold)
*****************/

WtD_FROM_S_FN(d_transitiveweights_threshold)

WtS_CHANGESTAT_FN(s_transitiveweights_threshold){ 
  Edge e1, e2;
  Vertex tail, head, node3;
  
  CHANGE_STAT[0]=0;
  for (tail=1; tail <= N_NODES; tail++){
    EXEC_THROUGH_FOUTEDGES(tail, e1, head, {
	if(GETWT(tail,head)<=INPUT_ATTRIB[0]) break;
	EXEC_THROUGH_INEDGES(head, e2, node3, { 
	    if(GETWT(tail,node3)>INPUT_ATTRIB[0] && GETWT(node3,head)>INPUT_ATTRIB[0]){
	      CHANGE_STAT[0]++; 
	      break;
	    }
	  })
	  })
      }
}

