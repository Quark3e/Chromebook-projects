
#include "SystemOfCameras.hpp"

namespace SOC {
    
    pos2d<double> drawCamU_lens(30, 5);
    pos2d<double> drawCamU_box(50, 20);

    CamU::CamU(const CamU &_copy): x(_copy.x), y(_copy.y), angle(_copy.angle), FOV(_copy.FOV) {

    }
    CamU::CamU(double _FOV, double _angle, double _x, double _y): x(_x), y(_y), angle(_angle), FOV(_FOV) {

    }
    CamU::~CamU() {

    }
    double &CamU::operator[](size_t _i) {
        switch (_i) {
            case 0: return x;
            case 1: return y;
            case 2: return angle;
            default:
                throw std::invalid_argument("&[](size_t): Invalid argument: "+std::to_string(_i));
        }
        return x;
    }
    double CamU::operator[](size_t _i) const {
        switch (_i) {
            case 0: return x;
            case 1: return y;
            case 2: return angle;
            default:
                throw std::invalid_argument("[](size_t) const: Invalid argument: "+std::to_string(_i));
        }
        return 0;
    }

    CamU &CamU::operator=(const CamU &_m) {
        x = _m.x;
        y = _m.y;
        angle = _m.angle;
        FOV = _m.FOV;
        return *this;
    }

    pos2d<double> CamU::pos() {
        return pos2d<double>(x, y);
    }

    double CamU::getCamAngle(pos2d<double> _objPos, bool _throwExcept_outOfFOV) {

        if(this->pos()==_objPos) {

            throw std::runtime_error("ERROR: getCamAngle: _objPos cannot be the same as the camera pos.");
        }
        double delta_x = _objPos.x - this->x;
        double delta_y = _objPos.y - this->y;
        double objDist = sqrt(std::pow(delta_x, 2)+std::pow(delta_y, 2));

        double angle_camPerpendicular = toDEGREES(delta_x < 0? 0-acos(delta_y/objDist) : acos(delta_y/objDist));

        double angle_camCenterToObj = (this->angle - 90) + angle_camPerpendicular;
        
        if(_throwExcept_outOfFOV) {
            if(angle_camCenterToObj < -this->FOV/2) throw std::out_of_range("getCamAngle: solved angle < -FOV/2 range.");
            if(angle_camCenterToObj >  this->FOV/2) throw std::out_of_range("getCamAngle: solved angle > FOV/2 range.");
        }

        return angle_camCenterToObj;
    }



    CamSystem::CamSystem(const CamSystem& _copy)
    : __objectPos(_copy.__objectPos), __system_CamU(_copy.__system_CamU), __system_CamU_angles(_copy.__system_CamU_angles)
    {


    }
    CamSystem::CamSystem(pos2d<double> _objPos, std::vector<CamU> _CamU_units)
    : __objectPos(_objPos), __system_CamU(_CamU_units)
    {
        for(size_t i=0; i<__system_CamU.size(); i++) {
            __system_CamU_angles.push_back(__system_CamU.at(i).getCamAngle(_objPos));
        }

    }
    CamSystem::~CamSystem() {

    }

    CamSystem &CamSystem::operator=(const CamSystem &_m) {
        __objectPos = _m.__objectPos;
        __system_CamU = _m.__system_CamU;
        __system_CamU_angles = _m.__system_CamU_angles;
        return *this;
    }
    CamU &CamSystem::operator[](size_t _i) {
        if(_i>=__system_CamU.size()) throw std::out_of_range("& [](size_t): _i value "+std::to_string(_i)+" is out of range "+std::to_string(__system_CamU.size()));
        return __system_CamU[_i];
    }
    CamU CamSystem::operator[](size_t _i) const {
        if(_i>=__system_CamU.size()) throw std::out_of_range("[](size_t) const: _i value "+std::to_string(_i)+" is out of range "+std::to_string(__system_CamU.size()));
        return __system_CamU[_i];
    }
    
    size_t CamSystem::size() const {
        return __system_CamU.size();
    }
    
    int CamSystem::set_objectPos(pos2d<double> _newPos) {
        __objectPos = _newPos;
        this->update();
        return 0;
    }

    double CamSystem::get_angle(size_t _i) {
        if(_i>=__system_CamU.size()) throw std::out_of_range("[](size_t) const: _i value "+std::to_string(_i)+" is out of range "+std::to_string(__system_CamU.size()));
        return __system_CamU_angles[_i];
    }

    pos2d<double> CamSystem::get_objectPos() {
        return this->__objectPos;
    }

    CamU &CamSystem::add(pos2d<double> _pos, double _angle, double _FOV) {
        __system_CamU.push_back({_FOV, _angle, _pos.x, _pos.y});
        try {
            double _toObjAngle = __system_CamU.at(__system_CamU.size()-1).getCamAngle(__objectPos);
            __system_CamU_angles.push_back(_toObjAngle);
        }
        catch(const std::exception& e) {
            __system_CamU_angles.push_back(-1);
        }
        
        return __system_CamU.at(__system_CamU.size()-1);
    }
    int CamSystem::erase(size_t _i) {
        if(_i>=__system_CamU.size()) throw std::out_of_range("erase(size_t): _i value "+std::to_string(_i)+" is out of range "+std::to_string(__system_CamU.size()));
        auto itr = __system_CamU.begin();
        std::advance(itr, _i);
        __system_CamU.erase(itr);

        return 0;
    }

    int CamSystem::update() {
        int success_code = 0;
        errorMsg = "";
        for(size_t i=0; i<__system_CamU.size(); i++) {
            try {
                double _toObjAngle = __system_CamU.at(i).getCamAngle(__objectPos);
                __system_CamU_angles.at(i) = _toObjAngle;
                __system_CamU.at(i).toObjectAngle = _toObjAngle;
            }
            catch(const std::exception& e) {
                __system_CamU_angles.at(i) = -1;
                if(errorMsg.size()==0) errorMsg = "objPos has same coord as CamU: {";
                errorMsg+=std::to_string(i)+", ";
                return 1;
            }
        }
        if(errorMsg.size()>0) errorMsg+="}";

        return success_code;
    }

}
