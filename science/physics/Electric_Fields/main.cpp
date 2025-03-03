
#include <imguiwin.hpp>

#include <pos2d.hpp>

#include <vector>
#include <cmath>

#include <keyBind_handler.hpp>
#include <nodeChart/extra_imgui.hpp>


#include <useful.hpp>

#define unit(val) (val)/abs(val)
#define coulombs_constant double(8.99)*double(pow(10, 9))
#define f_chargeForce(q_a, q_b, radius) (double(coulombs_constant)*double(abs(double(q_a)*double(q_b)))/(double(radius)*double(radius))) // [(Nm)/C^2]
#define charge_electon double(-1.60217663)*double(pow(10, -19))


struct charge_q {
    pos2d<double>   pos;    // unit: meters [m] (absolute coordinates in 2d space to some frame of reference)
    double          charge; // unit: coulomb [C]
    charge_q(pos2d<double> _pos, double _charge): pos(_pos), charge(_charge) {}
};

pos2d<double> solveVector_sumChargeForce(
    std::vector<charge_q>   system_charges,
    pos2d<double>           pos,
    double                  charge
);
void exitFunc() { exit(0); }



double  meter_per_px    = 0.0001; // Conversion rate of how many meters is represented per each screen pixel (1meter = 100px <=> 1px = 0.01m)
double  force_per_px    = 1;    // Conversion rate of how many newtons is represented per each screen pixel.
float   fieldVectorDist = 20;   // pixel distance between each electric field vector along both axes

// Minimum/Maximum pixel length of the field vector to be drawn
std::vector<float> drawFieldVectorLenLim_chargeVec{
    // fieldVectorDist*float(0.1),
    // roundf(sqrt(pow(fieldVectorDist*float(0.1), 2)*2))
    15,
    80
};


float getFieldVectorDrawLen(std::vector<double> _lim_chargeVec, double _val) {
    if(_lim_chargeVec.size()!=2) throw std::invalid_argument("getFieldVectorDrawLen(std::vector<double>, double) _lim_chargeVec arg is invalid. Not size of 2.");
    if(_lim_chargeVec[1] == _lim_chargeVec[0]) throw std::invalid_argument("getFieldVectorDrawLen(std::vector<double>, double) lim args can't be the same.");
    if(_lim_chargeVec[1] <  _lim_chargeVec[0]) throw std::invalid_argument("getFieldVectorDrawLen(std::vector<double>, double) max lim can't be smaller than min lim.");

    // std::cout << "limDelta_chargeVec: " << _lim_chargeVec[1]-_lim_chargeVec[0] << "N ";
    // std::cout << "val:" << _val << " max_val_lim:" << _lim_chargeVec[1] << " ";
    double scal = (_val-_lim_chargeVec[0]) / (_lim_chargeVec[1]-_lim_chargeVec[0]);
    // std::cout << "scal:" << scal << " ";
    
    double val = roundf(
        scal*double(drawFieldVectorLenLim_chargeVec[1]-drawFieldVectorLenLim_chargeVec[0]) +
        double(drawFieldVectorLenLim_chargeVec[0])
    );
    // std::cout << "val:" << val << " ";
    // std::cout << "\n";
    return val;
}
pos2d<float> getDrawVectors(pos2d<double> fieldVector, float drawLen) {
    double scal_x = pow(fieldVector.x, 2) / pow(fieldVector.hypotenuse(), 2);
    double scal_y = pow(fieldVector.y, 2) / pow(fieldVector.hypotenuse(), 2);
    double drawLenSq = pow(drawLen, 2);
    pos2d<int> unitVec = (fieldVector / fieldVector.abs()).cast<int>([](double _var){return int(roundf(_var));});
    if(abs(unitVec.x) > 1 || abs(unitVec.y) > 1) {
        std::cout << "unitVec invalid: " << unitVec << " | fieldVector: " << fieldVector << " fieldVector_abs: " << fieldVector.abs() << std::endl;
    }
    return pos2d<float>(
        sqrt(drawLenSq * scal_x) * unitVec.x,
        sqrt(drawLenSq * scal_y) * unitVec.y
    );
}

pos2d<int> abs_cam_pixelPos(0, 0); //position of the viewpoint camera in px space

ImU32 col_fieldDir  = IM_COL32(200, 200, 200, 200);
ImU32 col_chargeNeg = IM_COL32(200, 10, 10, 100);
ImU32 col_chargePos = IM_COL32(10, 10, 200, 100);

int chargeDetectRadius = 20;

// bool setting_showGrid   = false;

guiwin_nc::imguiwin guiwin(false);


void _keyBind__Master_Close() {
    exit(1);
}
DIY_KBH::keyBind_handler keyBinds({
    {"Master Close",    {ImGuiKey_ReservedForModCtrl, ImGuiKey_LeftCtrl, ImGuiKey_W}, _keyBind__Master_Close},
    {"MouseLeft",       {ImGuiKey_MouseLeft}},
    {"MouseRight",      {ImGuiKey_MouseRight}}
});
pressed_key__struct guiKeys;



int main(int argc, char** argv) {
    guiwin_nc::win_dim = ImVec2(1000, 1000);
    guiwin.callback_func__running_exit = exitFunc;
    
    std::vector<charge_q> system_charges{
        charge_q{{0.02, 0.04}, -charge_electon},
        charge_q{{0.08, 0.04}, charge_electon}
        // charge_q{{0.04, 0.09}, -charge_electon}
    };
    meter_per_px = ((system_charges[1].pos.x - system_charges[0].pos.x) / 600);

    pos2d<size_t> numSides(
        int(roundf(guiwin_nc::win_dim.x / float(fieldVectorDist)))+1,
        int(roundf(guiwin_nc::win_dim.y / float(fieldVectorDist)))+1
    );
    /**
     * Force vectors affecting each point in current pixel space [F_x, F_y]
     * 
     */
    std::vector<std::vector<pos2d<double>>> bg_vectors(numSides.x, std::vector<pos2d<double>>(numSides.y, pos2d<double>(0, 0)));
    /**
     * Position of each vector in pixel space relative to current frame.
     * Used for drawing purposes.
     */
    std::vector<std::vector<pos2d<int>>>    bg_vectLoc(numSides.x, std::vector<pos2d<int>>(numSides.y, pos2d<int>(0, 0)));

    std::vector<double> lim_chargeVec(2, 0);


    /// Start drawing to window

    guiwin.init();
    while(guiwin.running()) {
        ImGuiIO io = ImGui::GetIO();
        guiwin.newFrame();
        
        guiKeys.update();
        keyBinds.update(guiKeys.pressed[guiKeys.pressed.size()-1]);
        for(size_t i=0; i<system_charges.size(); i++) {
            charge_q& charg = system_charges[i];
            pos2d<int> pixel_pos = pos2d<int>(
                charg.pos.modify(
                    [](double _var){return (_var/meter_per_px);}
                ).cast<int>([](double _var){ return int(roundf(_var)); })
            ) - abs_cam_pixelPos;
            guiwin.draw()->AddRect(guiwin_nc::toImVec2(pixel_pos-chargeDetectRadius), guiwin_nc::toImVec2(pixel_pos+chargeDetectRadius), IM_COL32(100, 100, 100, 100));
            if(guiwin_nc::inRegion(io.MousePos, guiwin_nc::toImVec2(pixel_pos-chargeDetectRadius), guiwin_nc::toImVec2(pixel_pos+chargeDetectRadius))) {
                if(keyBinds.pressing("MouseLeft")) {
                    charg.pos = guiwin_nc::toPos2d(io.MousePos).cast<double>().modify([](double _var){ return (_var*meter_per_px); });

                }
            }
        }
    
        for(size_t x=0; x<numSides.x; x++) {
            for(size_t y=0; y<numSides.y; y++) {
                pos2d<int> pixel_pos(x * fieldVectorDist, y * fieldVectorDist); // draw pixel coordinate
                bg_vectLoc[x][y] = pixel_pos;
    
                //"real" coordinate in meters
                pos2d<double> charge_pos(
                    double(pixel_pos.x + abs_cam_pixelPos.x) * meter_per_px,
                    double(pixel_pos.y + abs_cam_pixelPos.y) * meter_per_px
                );
                
                pos2d<double> chargeVector = solveVector_sumChargeForce(system_charges, charge_pos, -charge_electon);
                bg_vectors[x][y] = chargeVector;
                if(chargeVector.hypotenuse() < lim_chargeVec[0]) lim_chargeVec[0] = chargeVector.hypotenuse();
                if(chargeVector.hypotenuse() > lim_chargeVec[1]) lim_chargeVec[1] = chargeVector.hypotenuse();
            }
        }



        /// Draw bg field vectors
        for(size_t x=0; x<bg_vectors.size(); x++) {
            for(size_t y=0; y<bg_vectors[x].size(); y++) {
                
                double _scal = (bg_vectors[x][y].hypotenuse() / lim_chargeVec[1]);    // relative scalar percentage of current vector's magnitude vs the biggest magnitude
                float drawVecLen = getFieldVectorDrawLen(lim_chargeVec, bg_vectors[x][y].hypotenuse());
                
                // pixel XY offset values 
                pos2d<int> locOffset(
                    getDrawVectors(
                        bg_vectors[x][y],
                        drawVecLen
                    ).cast<int>([](float _var) { return int(roundf(_var)); }
                ));
                // std::cout << bg_vectors[x][y] << std::endl;
                // pos2d<int> locOffset(bg_vectors[x][y].cast<int>([](double _var){return int(roundf(_var*pow(10, 25))); })+pos2d<int>(5, 5));

                // pos2d<int> locOffset(drawVecLen, drawVecLen);
                
                guiwin.draw()->AddLine(
                    guiwin_nc::toImVec2(bg_vectLoc[x][y]),
                    guiwin_nc::toImVec2((bg_vectLoc[x][y]+locOffset)),
                    col_fieldDir
                );
                guiwin.draw()->AddCircleFilled(
                    guiwin_nc::toImVec2(bg_vectLoc[x][y]), 2, col_fieldDir, 10
                );
            }
        }

        /// Draw system charges
        for(size_t i=0; i<system_charges.size(); i++) {
            charge_q& charg = system_charges[i];
            pos2d<int> pixel_pos = pos2d<int>(charg.pos.modify([](double _var){return (_var/meter_per_px);}).cast<int>([](double _var){ return int(roundf(_var)); })) - abs_cam_pixelPos;
            
            guiwin.draw()->AddCircleFilled(guiwin_nc::toImVec2(pixel_pos), 10, (charg.charge>0? col_chargePos : col_chargeNeg), 10);
        }

        guiwin.endFrame();
    }


    return 0;
}


pos2d<double> solveVector_sumChargeForce(
    std::vector<charge_q>   system_charges,
    pos2d<double>           pos,
    double                  charge
) {

    pos2d<double> chargeForce_vector_sum(0, 0);

    for(size_t i=0; i<system_charges.size(); i++) {
        if(pos==system_charges[i].pos) continue;
        charge_q& sys_charge = system_charges[i];
        pos2d<double>   pos_delta   = pos - sys_charge.pos;
        pos2d<double>   unitVec     = (pos_delta / pos_delta.abs()).modify([](double var) {
            if(isnan(var)) return double(0);
            return double(roundf(var));
        });
        if((charge>0 && sys_charge.charge>0) || (charge<0 && sys_charge.charge<0)) {
            /// Same charge. Force vectors are directed away from sys_charge.
        }
        else {
            /// Opposite charges
            unitVec[0] = (-1)*unitVec[0];
            unitVec[1] = (-1)*unitVec[1];
        }

        double chargeForce = f_chargeForce(sys_charge.charge, charge, pos_delta.hypotenuse());
        double deltaSQ[3] = {pow(pos_delta.x, 2), pow(pos_delta.y, 2), pow(pos_delta.hypotenuse(), 2)};
        pos2d<double>   scalars{
            (deltaSQ[0] * chargeForce) / deltaSQ[2],
            (deltaSQ[1] * chargeForce) / deltaSQ[2]
        };

        chargeForce_vector_sum += unitVec * scalars;
    }

    return chargeForce_vector_sum;
}

