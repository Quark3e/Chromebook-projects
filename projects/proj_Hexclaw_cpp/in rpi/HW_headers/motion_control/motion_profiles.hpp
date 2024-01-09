
#pragma once
#ifndef H_HEXCLAW_MOTION_PROFILES
#define H_HEXCLAW_MOTION_PROFILES


float mp1(float x) {
    float y=0, V_max=2, t3=1;
    float Pt1=0.5;
    float Pt2=Pt1;
    float t2=t3*Pt2;
    float t1=t3*Pt1;
    float a3 = (0-V_max)/(t3-t2);
    float a1 = (V_max-0)/(t1-0);
    if(x>0 && x<=t1) y=(a1*pow(x,2))/2;
    else if(x>t1 && x<t2) y=a1*t1*x-(a1*pow(t1,2))/2;
    else if(x>=t2 && x<=t3) y=(a3*pow(x-t2,2))/2+V_max*x+a1*(t1*t2-pow(t1,2)/2)-V_max*t2;
    return y;
}



#endif