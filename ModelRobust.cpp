//
// Created by simon on 1/25/2020.
//
#include <cstddef>
#include "ModelRobust.h"
using namespace ::dashoptimization;

using namespace std;

ModelRobust::ModelRobust( Data* d, int gamma1_, int gamma2_, int gamma3_) {
    data = d;
    this->gamma1 = gamma1_;
    this->gamma2 = gamma2_;
    this->gamma3 = gamma3_;
    pbRob = new XPRBprob("MyProb");
    pbRob->setMsgLevel(0);
    // I: array(T) of mpvar	! Inventory level at end of period t
    I= new XPRBvar[this->data->getNPer()+1];
    for(int t=1; t<=this->data->getNPer(); t++)
    {
        I[t] = this->pbRob->newVar("I",XPRB_PL,0.0,XPRB_INFINITY);
    }

    // Q: array(T, S) of mpvar ! Qtty ordered from    !supplier s in period t
    Q = new XPRBvar*[this->data->getNPer()+1];
    for(int t=1; t<=this->data->getNPer(); t++)
    {
        Q[t] = new XPRBvar[this->data->getNSup()+1];
        for(int s= 1; s<=this->data->getNSup(); s++)
        {
            Q[t][s] = this->pbRob->newVar("Q",XPRB_PL,0.0,XPRB_INFINITY); //XPRB_BV
        }
    }

    // Y: array(T, S) of mpvar ! equal 1 if an order is !passed to supplier s in period t
    Y = new XPRBvar*[this->data->getNPer()+1];
    for(int t=1; t<=this->data->getNPer(); t++)
    {
        Y[t] = new XPRBvar[this->data->getNSup()+1];
        for(int s= 1; s<=this->data->getNSup(); s++)
        {
            Y[t][s] = this->pbRob->newVar("Y",XPRB_BV); //XPRB_BV
        }
    }

    //Del: array(T, T, S) of mpvar !equal 1 if ! order passed in t1 from supplier s ! is received in t2
  /*  this->Delta= new XPRBvar**[this->data->getNPer()+1];
    for(int t=1; t<=this->data->getNPer(); t++)
    {
        this->Delta[t]= new XPRBvar*[this->data->getNPer()+1];
        for(int tau=1; tau<=this->data->getNPer(); tau++)
        {
            this->Delta[t][tau]= new XPRBvar[this->data->getNSup()+1];
            for(int s=1; s<=this->data->getNSup(); s++)
            {
                this->Delta[t][tau][s] = this->pbRob->newVar("delta");
            }
        }
    }*/

    // c: array(T) of mpvar	! total holding or backordering cost
    c= new XPRBvar[this->data->getNPer()+1];
    for(int t=1; t<=this->data->getNPer(); t++)
    {
        c[t] = this->pbRob->newVar("c",XPRB_PL,0.0,XPRB_INFINITY);
    }

    //rho: array(T) of mpvar ! dualvariables
    rho1= new XPRBvar[this->data->getNPer()+1];
    for(int t=1; t<=this->data->getNPer(); t++)
    {
        rho1[t] = this->pbRob->newVar(XPRBnewname("rho1_%d", t),XPRB_PL,0.0,XPRB_INFINITY);

    }

    //rho: array(T) of mpvar ! dualvariables
    rho2= new XPRBvar[this->data->getNPer()+1];
    for(int t=1; t<=this->data->getNPer(); t++)
    {

        rho2[t] = this->pbRob->newVar(XPRBnewname("rho2_%d", t),XPRB_PL,0.0,XPRB_INFINITY);

    }

    rho3= new XPRBvar[this->data->getNPer()+1];
    for(int t=1; t<=this->data->getNPer(); t++)
    {
        rho3[t] = this->pbRob->newVar(XPRBnewname("rho3_%d", t),XPRB_PL,0.0,XPRB_INFINITY);

    }

    // xhi: array(T,  T, S) of mpvar ! dualvariables
    this->xhi= new XPRBvar**[this->data->getNPer()+1];
    for(int t=1; t<=this->data->getNPer(); t++)
    {
        this->xhi[t]= new XPRBvar*[this->data->getNPer()+1];
        for(int tau=1; tau<=this->data->getNPer(); tau++)
        {
            this->xhi[t][tau]= new XPRBvar[this->data->getNSup()+1];
            for(int s=1; s<=this->data->getNSup(); s++)
            {
                this->xhi[t][tau][s] = this->pbRob->newVar("xhi",XPRB_PL,0.0,XPRB_INFINITY);
            }
        }
    }

 // lambda: array(T,  T, S) of mpvar ! dualvariables
    this->lambda= new XPRBvar**[this->data->getNPer()+1];
    for(int t=1; t<=this->data->getNPer(); t++)
    {
        this->lambda[t]= new XPRBvar*[this->data->getNPer()+1];
        for(int tau=1; tau<=this->data->getNPer(); tau++)
        {
            this->lambda[t][tau]= new XPRBvar[this->data->getNSup()+1];
            for(int s=1; s<=this->data->getNSup(); s++)
            {
                this->lambda[t][tau][s] = this->pbRob->newVar("lambda",XPRB_PL,0.0,XPRB_INFINITY);
            }
        }
    }
// psi: array(T,  T, S) of mpvar ! dualvariables
    this->psi= new XPRBvar**[this->data->getNPer()+1];
    for(int t=1; t<=this->data->getNPer(); t++)
    {
        this->psi[t]= new XPRBvar*[this->data->getNPer()+1];
        for(int tau=1; tau<=this->data->getNPer(); tau++)
        {
            this->psi[t][tau]= new XPRBvar[this->data->getNSup()+1];
            for(int s=1; s<=this->data->getNSup(); s++)
            {
                this->psi[t][tau][s] = this->pbRob->newVar("psi",XPRB_PL,0.0,XPRB_INFINITY);
            }
        }
    }
// rhob: array(T) of mpvar ! dualvariables
    rhob1= new XPRBvar[this->data->getNPer()+1];
    for(int t=1; t<=this->data->getNPer(); t++)
    {
        rhob1[t] = this->pbRob->newVar(XPRBnewname("rhob1_%d", t),XPRB_PL,0.0,XPRB_INFINITY);
    }

    rhob2= new XPRBvar[this->data->getNPer()+1];
    for(int t=1; t<=this->data->getNPer(); t++)
    {
        rhob2[t] = this->pbRob->newVar(XPRBnewname("rhob2_%d", t),XPRB_PL,0.0,XPRB_INFINITY);
    }

    rhob3= new XPRBvar[this->data->getNPer()+1];
    for(int t=1; t<=this->data->getNPer(); t++)
    {
        rhob3[t] = this->pbRob->newVar(XPRBnewname("rhob3_%d", t),XPRB_PL,0.0,XPRB_INFINITY);
    }

    // xhib: array(T,  T, S) of mpvar ! dualvariables
    this->xhib= new XPRBvar**[this->data->getNPer()+1];
    for(int t=1; t<=this->data->getNPer(); t++)
    {
        this->xhib[t]= new XPRBvar*[this->data->getNPer()+1];
        for(int tau=1; tau<=this->data->getNPer(); tau++)
        {
            this->xhib[t][tau]= new XPRBvar[this->data->getNSup()+1];
            for(int s=1; s<=this->data->getNSup(); s++)
            {
                this->xhib[t][tau][s] = this->pbRob->newVar("xhib",XPRB_PL,0.0,XPRB_INFINITY);
            }
        }
    }

    // lambdab: array(T,  T, S) of mpvar ! dualvariables
    this->lambdab= new XPRBvar**[this->data->getNPer()+1];
    for(int t=1; t<=this->data->getNPer(); t++)
    {
        this->lambdab[t]= new XPRBvar*[this->data->getNPer()+1];
        for(int tau=1; tau<=this->data->getNPer(); tau++)
        {
            this->lambdab[t][tau]= new XPRBvar[this->data->getNSup()+1];
            for(int s=1; s<=this->data->getNSup(); s++)
            {
                this->lambdab[t][tau][s] = this->pbRob->newVar("lambdab",XPRB_PL,0.0,XPRB_INFINITY);
            }
        }
    }
// psib: array(T,  T, S) of mpvar ! dualvariables
    this->psib= new XPRBvar**[this->data->getNPer()+1];
    for(int t=1; t<=this->data->getNPer(); t++)
    {
        this->psib[t]= new XPRBvar*[this->data->getNPer()+1];
        for(int tau=1; tau<=this->data->getNPer(); tau++)
        {
            this->psib[t][tau]= new XPRBvar[this->data->getNSup()+1];
            for(int s=1; s<=this->data->getNSup(); s++)
            {
                this->psib[t][tau][s] = this->pbRob->newVar("psib",XPRB_PL,0.0,XPRB_INFINITY);
            }
        }
    }



   // BigM := sum(t in T)d(t)	!the maximum that can be delivered by and supplier at any periods
    double BigM = 0;
    for(int t=1; t<=this->data->getNPer(); t++)
    {
        BigM+=this->data->getDemand(t-1);
    }
   //  TotalCost := sum(t in T)c(t)+sum(t in T, s in S)(Y(t,s)*o(s)+Q(t,s)*p(s))
    XPRBexpr cumulativec=*new XPRBexpr();
    for(int t = 1; t<= this->data->getNPer(); t++)
    { cumulativec+= c[t];}

    Data::print("Build Objective");
    //TotalCost := C + sum(t in T, s in S)(YY(t,s)*o(s)+Q(t,s)*p(s))
    XPRBexpr lobj;
 //   lobj += cumulativec;
    for(int t = 1; t<= this->data->getNPer(); t++) {
        lobj += c[t];
        for (int s = 1; s <= this->data->getNSup(); s++)
            lobj += (Q[t][s] * this->data->getPrice(s - 1) + this->Y[t][s] * this->data->getSetup(s - 1));
    }


    this->pbRob->setObj(this->pbRob->newCtr("OBJ", lobj));
    this->pbRob->setSense(XPRB_MINIM);

    //Constraint related to holding cost
    //forall(t in T)
    //ConstraintHolding(t) := c(t) >=  rho(t)*(Gamma-Suppliers)+ sum(s in S)( sum(tau in 1..t-Lmax(s) )( -xhi(tau,t,s)) + sum(tau in 1..t)(psi(tau,t,s))) - h * sum(tau in 1..t)d(tau)

    XPRBexpr* cumulativexi=new XPRBexpr[this->data->getNPer()+1];
    XPRBexpr* cumulativepsi=new XPRBexpr[this->data->getNPer()+1];
    for(int t = 1; t<= this->data->getNPer(); t++)
    {
        cumulativexi[t] = *new XPRBexpr();
        cumulativepsi[t] = *new XPRBexpr();
        for(int s= 1; s<=this->data->getNSup(); s++)
        {
            for(int tau = 1; tau <= t-this->data->getLMax(s-1); tau++)
            {
                cumulativexi[t] -= xhi[tau][t][s];
            }
            for(int tau = 1; tau <= t; tau++)
            {
                cumulativepsi[t] += psi[tau][t][s];
            }
        }
    }

    int* cumulativeBizar = new int[this->data->getNPer()+1];
    double* cumulativeDemand=new double[this->data->getNPer()+1];
    for(int t = 1; t<= this->data->getNPer(); t++)
    {
        cumulativeBizar[t] = 0;
        for(int s= 1; s<=this->data->getNSup(); s++) {
            cumulativeBizar[t] += t - this->data->getLMin(s - 1);
        }
        cumulativeDemand[t] = 0.0;


        for(int tau = 1; tau <= t; tau++)
        {
            cumulativeDemand[t] += this->data->getDemand(tau-1);

        }
    }

    int*  constantgamma2 = new int[this->data->getNPer()+1];;
    int*  constantgamma3 = new int[this->data->getNPer()+1];;
    for (int t = 1; t <= this->data->getNPer(); t++) {
        constantgamma2 [t] = 0;
        constantgamma3 [t] = 0;
        for (int s = 1; s <= this->data->getNSup(); s++) {
           // for (int t = 1; t < this->data->getNPer(); t++) {
                for (int tau = 1; tau <= t-this->data->getLMin(s -1); tau++) {
                    constantgamma2[t] += 1;
                    constantgamma3 [t] += t-tau-this->data->getLMin(s -1) +1;
                }
          //  }
        }
       // cout<<constantgamma3[t]<<endl;
    }

    Data::print("ConstraintHoldin");
        //cout<< this->gamma<<"ouch???"<<constant<<endl;
        for (int t = 1; t <= this->data->getNPer(); t++) {/*this->data->getNSup()) \*/
            this->pbRob->newCtr(XPRBnewname("ConstraintHolding%d", t),
                                c[t] >= this->rho1[t] * ( this->gamma1 - this->data->getNSup()) \
                                         + this->rho2[t] * ( this->gamma2 - constantgamma2[t]) \
                                         + this->rho3[t] * ( this->gamma3 -  constantgamma3 [t]) \
                                            + cumulativexi[t] \
                                            + cumulativepsi[t] \
                                            - this->data->getch() * cumulativeDemand[t]);

        }


 //   forall(t in T)
 //   ConstraintBacklog(t) := c(t) >= rhob(t)* (Gamma-Suppliers)+ sum(s in S)( sum(tau in 1..t-Lmax(s) )( -xhib(tau,t,s)) + sum(tau in 1..t)(psib(tau,t,s)) ) + b * sum(tau in 1..t)d(tau)
    XPRBexpr* cumulativexib=new XPRBexpr[this->data->getNPer()+1];
    XPRBexpr* cumulativepsib=new XPRBexpr[this->data->getNPer()+1];
    for(int t = 1; t<= this->data->getNPer(); t++)
    {
        cumulativexib[t] = *new XPRBexpr();
        cumulativepsib[t] = *new XPRBexpr();
        for(int s= 1; s<=this->data->getNSup(); s++)
        {
            for(int tau = 1; tau <= t-this->data->getLMax(s-1); tau++)
            {
                cumulativexib[t] -= xhib[tau][t][s];
            }
            for(int tau = 1; tau <= t; tau++)
            {
                cumulativepsib[t] += psib[tau][t][s];
            }
        }
    }

    Data::print("ConstraintBacklog");
        for (int t = 1; t <= this->data->getNPer(); t++) {
            this->pbRob->newCtr(XPRBnewname("ConstraintBacklog%d", t),
                                c[t] >= this->rhob1[t] * ( this->gamma1 - this->data->getNSup()) \
                                         + this->rhob2[t] * ( this->gamma2 - constantgamma2[t]) \
                                         + this->rhob3[t] * ( this->gamma3 -  constantgamma3 [t]) \
                                            + cumulativexib[t] \
                                            + cumulativepsib[t] \
                                            + this->data->getcb() * cumulativeDemand[t]);

        }





   //  forall(t in T, s in S)
   // ConstraintBigM(t,s) := Q(t,s) <= BigM*Y(t,s)
    Data::print("ConstraintBigM");
    XPRBctr** BigMConstraints = new XPRBctr*[ this->data->getNPer()+1];
    for(int t = 1; t<= this->data->getNPer(); t++)
    {
        BigMConstraints[t] = new XPRBctr[ this->data->getNSup()+1];

        for(int s= 1; s<=this->data->getNSup(); s++)
        {
            BigMConstraints[t][s]=this->pbRob->newCtr(XPRBnewname("ConstraintBigM%d%d",t,s),
                                                    this->Q[t][s] <= this->Y[t][s]*BigM );
        }
    }

    //      Constrain_SatisfyAllDemand := sum(t in T, s in S) Q(t,s) >= sum(t in T) d(t
    Data::print("cumulativeQuantityTot");
    XPRBexpr* cumulativeQuantityTot=new XPRBexpr();
    for(int s= 1; s<=this->data->getNSup(); s++)
    {
        for(int t =1; t<= this->data->getNPer(); t++)
        {
            *cumulativeQuantityTot += this->Q[t][s];
        }
    }
    Data::print("MeetDemand");
    this->pbRob->newCtr(XPRBnewname("MeetDemand"),
                      *cumulativeQuantityTot >= cumulativeDemand[this->data->getNPer()] );



   // forall(t in T, tau in T, s in S) do
    for(int t = 1; t<= this->data->getNPer(); t++)
    {


        for(int tau = 1; tau<= this->data->getNPer(); tau++) {

            for(int s= 1; s<=this->data->getNSup(); s++) {

                if (true) {
                    if (tau > t - this->data->getLMax(s - 1) && tau <= t - this->data->getLMin(s - 1)) {
                        //     if (tau > t -Lmax(s) and (tau<= t-Lmin(s))) then

                        if (tau == t - this->data->getLMin(s - 1)) {
                            //if tau = t - Lmin(s) then
                            //ConstraintUncertaintySet1(tau, t, s) := psib(tau, t, s) - rhob( t) >= -b * Q(tau,s)
                            this->pbRob->newCtr(XPRBnewname("ConstraintUncertaintySet1%d%d%d", tau, t, s),
                                                this->psib[tau][t][s] - this->rhob1[t] - this->rhob2[t] - (t-tau- this->data->getLMin(s - 1) +1) * this->rhob3[t] >=
                                                -this->data->getcb() * this->Q[tau][s]);
                            //ConstraintUncertaintySet2(tau, t, s) := psi(tau, t, s) - rho( t) >= h * Q(tau,s)
                            this->pbRob->newCtr(XPRBnewname("ConstraintUncertaintySet2%d%d%d", tau, t, s),
                                                this->psi[tau][t][s] - this->rho1[t] - this->rho2[t] - (t-tau- this->data->getLMin(s - 1) +1) * this->rho3[t] >=
                                                this->data->getch() * this->Q[tau][s]);
                        } else {
                            //else
                            //ConstraintUncertaintySet1(tau, t, s) := psib(tau, t, s)  >= -b * Q(tau,s)
                            this->pbRob->newCtr(XPRBnewname("ConstraintUncertaintySet1%d%d%d", tau, t, s),
                                                this->psib[tau][t][s] - this->rhob2[t] - (t-tau- this->data->getLMin(s - 1) +1) * this->rhob3[t] >= -this->data->getcb() * this->Q[tau][s]);
                            //ConstraintUncertaintySet2(tau, t, s) := psi(tau, t, s)  >= h * Q(tau,s)
                            this->pbRob->newCtr(XPRBnewname("ConstraintUncertaintySet2%d%d%d", tau, t, s),
                                                this->psi[tau][t][s]- this->rho2[t] - (t-tau- this->data->getLMin(s - 1) +1) * this->rho3[t] >= this->data->getch() * this->Q[tau][s]);
                            //end-if
                        }

                    } else {
                        if (tau > t - this->data->getLMin(s - 1)) {
                            //    elif (tau > t -Lmin(s)) then
                            //ConstraintUncertaintySet3(tau, t, s) := psib(tau, t, s) +lambdab(tau, t, s) >= -b * Q(tau,s)
                            this->pbRob->newCtr(XPRBnewname("ConstraintUncertaintySet3%d%d%d", tau, t, s),
                                                this->psib[tau][t][s] + this->lambdab[tau][t][s] >=
                                                -this->data->getcb() * this->Q[tau][s]);
                            //ConstraintUncertaintySet4(tau, t, s) := psi(tau, t, s)  +lambda(tau, t, s) >= h * Q(tau,s)
                            this->pbRob->newCtr(XPRBnewname("ConstraintUncertaintySet4%d%d%d", tau, t, s),
                                                this->psi[tau][t][s] + this->lambda[tau][t][s] >=
                                                this->data->getch() * this->Q[tau][s]);


                        } else {
                            if (tau <= t - this->data->getLMax(s - 1)) {
                                //elif (tau <= t -Lmax(s)) then
                                //ConstraintUncertaintySet5(tau, t, s) := psib(tau, t, s) -xhib(tau, t, s) >= -b * Q(tau,s)
                                this->pbRob->newCtr(XPRBnewname("ConstraintUncertaintySet5%d%d%d", tau, t, s),
                                                    this->psib[tau][t][s] - this->xhib[tau][t][s] - this->rhob2[t] - (t-tau- this->data->getLMin(s - 1) +1) * this->rhob3[t]>=
                                                    -this->data->getcb() * this->Q[tau][s]);

                                //ConstraintUncertaintySet6(tau, t, s) := psi(tau, t, s)  -xhi(tau, t, s) >= h * Q(tau,s)
                                this->pbRob->newCtr(XPRBnewname("ConstraintUncertaintySet6%d%d%d", tau, t, s),
                                                    this->psi[tau][t][s] - this->xhi[tau][t][s] - this->rho2[t] - (t-tau- this->data->getLMin(s - 1) +1) * this->rho3[t] >=
                                                    this->data->getch() * this->Q[tau][s]);
                            }

                        }

                    }
                }


            }

        }
    }


}

ModelRobust::ModelRobust( void ){

}

double** ModelRobust::getQuantities( )
{


    double** sol_Q = new double*[this->data->getNPer()+1];
    for(int t=1; t <= this->data->getNPer(); t++)
    {
        sol_Q[t] = new double[this->data->getNSup()+1];

        for(int s=1; s<=this->data->getNSup(); s++)
        {sol_Q[t][s] = this->Q[t][s].getSol();
        }

    }
    return sol_Q;
}


int** ModelRobust::getY( )
{
    int** sol_Y = new int*[this->data->getNPer()+1];
    for(int t=1; t <= this->data->getNPer(); t++)
    {
        sol_Y[t] = new int[this->data->getNSup()+1];

        for(int s=1; s<=this->data->getNSup(); s++)
        {sol_Y[t][s] = this->Y[t][s].getSol();
        }

    }
    return sol_Y;
}

double ModelRobust::GetSetupCost( ){

    double result = 0;
    for(int t=1; t <= this->data->getNPer(); t++)
    {
        for(int s=1; s<=this->data->getNSup(); s++)
        {
            result += this->Y[t][s].getSol()*this->data->getSetup(s-1);
        }

    }
    return result;

}
double ModelRobust::GetPurshasingCosts( ){
    double result = 0;
    for(int t=1; t <= this->data->getNPer(); t++)
    {
        for(int s=1; s<=this->data->getNSup(); s++)
        {
            result += this->Q[t][s].getSol()*this->data->getPrice(s-1);
        }

    }
    return result;

}

ModelRobust::~ModelRobust(void){}
void ModelRobust::Solve(void){
    clock_t start, end;
    start = clock();
    double temps=0;
    XPRSprob opt_prob =  this->pbRob->getXPRSprob();
    XPRSsetintcontrol(opt_prob,XPRS_MAXTIME,  this->data->getTimeLimite());
    XPRSsetintcontrol(opt_prob,XPRS_MIPTHREADS,  1);
    //this->pbRob->exportProb(1,"lpr");
    this->pbRob->mipOptimise();
    cout<<"Cost after optimi::"<<this->pbRob->getObjVal()<<endl;
  /*  for(int t=1; t <= this->data->getNPer(); t++)
    {
         cout<<"C"<<t<<":"<<this->c[t].getSol();
        cout<<"  rhob1:"<<this->rhob1[t].getSol();      cout<<"  rho:"<<this->rho1[t].getSol()<<endl;
        for(int tau=1; tau<=this->data->getNPer(); tau++)
        {
            for(int s=1; s<=this->data->getNSup(); s++)
            {
                cout<<"psib"<<tau<<"-"<<s<<":"<<this->psib[t][tau][s].getSol()<<endl;
            }
        }
         cout<<this->c[t].getSol()<<endl;
         cout<<this->c[t].getSol()<<endl;

    }*/
    end = clock();
    temps = (double) (end-start)/ CLOCKS_PER_SEC;
    this->LastRunning = temps;

}