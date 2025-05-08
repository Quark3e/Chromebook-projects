#pragma once
#ifndef HPP_LIB_POLYREGIONCHECK
#define HPP_LIB_POLYREGIONCHECK

#include <vector>
#include <string>
#include <iostream>


#include <pos2d.hpp>
#include "line_intersect2D.hpp"


#include "imguiwin.hpp"


#ifndef GET_2_MIN
#define GET_2_MIN(a, b) ((a)<(b)? (a) : (b))
#endif //GET_2_MIN

#ifndef GET_2_MAX
#define GET_2_MAX(a, b) ((a)>(b)? (a) : (b))
#endif //GET_2_MAX

namespace PRC {


    template<class _varType>
    struct polySide {
        pos2d<_varType> p0;
        pos2d<_varType> p1;

        pos2d<_varType> p_min{0, 0};
        pos2d<_varType> p_max{0, 0};

        polySide(pos2d<_varType> _p0, pos2d<_varType> _p1): p0(_p0), p1(_p1), p_min(GET_2_MIN(_p0.x, _p1.x), GET_2_MIN(_p0.y, _p1.y)), p_max(GET_2_MAX(_p0.x, _p1.x), GET_2_MAX(_p0.y, _p1.y)) {
            if(p0==p1) throw std::runtime_error("ERROR: polySide::polyside(pos2d, pos2d): the two end points coordinates can't be the exact same.");
            
        }
    
        /**
         * @brief Checks if a given value lies within the range of the X-axis bounds.
         * 
         * This function determines whether the provided `_x_value` falls within the 
         * range defined by `p_min.x` and `p_max.x`. The behavior of the check can 
         * include or exclude the boundary values based on the `_includeBB` parameter.
         * 
         * @tparam _varType The type of the value being checked (e.g., float, double, int).
         * @param _x_value The value to check against the X-axis bounds.
         * @param _includeBB A boolean flag indicating whether to include the boundary 
         *                   values in the range check. 
         * 
         *                   - If `true`, the range is inclusive (`<=` and `>=`).
         * 
         *                   - If `false`, the range is exclusive (`<` and `>`).
         * 
         * @return `true` if `_x_value` lies within the specified range, `false` otherwise.
         */
        bool inAxisRange_X(_varType _x_value, bool _includeBB=true) {
            if(_includeBB) {
                if(_x_value <= p_max.x && _x_value >= p_min.x) {
                    return true;
                }
            }
            else {
                if(_x_value <  p_max.x && _x_value >  p_min.x) {
                    return true;
                }
            }
            return false;
        }
        /**
         * @brief Checks if a given Y-axis value is within the range defined by the object's bounding box.
         * 
         * @tparam _varType The data type of the Y-axis value (e.g., float, double, int).
         * @param _y_value The Y-axis value to check.
         * @param _includeBB A boolean flag indicating whether to include the bounding box limits in the range check.
         * 
         *                    - If true, the range is inclusive of the bounding box limits (p_min.y <= _y_value <= p_max.y).
         * 
         *                    - If false, the range is exclusive of the bounding box limits (p_min.y < _y_value < p_max.y).
         * 
         * @return true If the Y-axis value is within the specified range.
         * @return false Otherwise.
         */
        bool inAxisRange_Y(_varType _y_value, bool _includeBB=true) {
            if(_includeBB) {
                if(_y_value <= p_max.y && _y_value >= p_min.y) {
                    return true;
                }
            }
            else {
                if(_y_value <  p_max.y && _y_value >  p_min.y) {
                    return true;
                }
            }
            return false;
        }

    };


    /**
     * @brief Determines if a given point lies within the perimeter of a polygon.
     * 
     * This function checks whether a specified point is inside the perimeter of a polygon
     * defined by a set of vertices. It also optionally calculates the minimum and maximum
     * bounding ranges of the polygon.
     * 
     * @tparam _varType The data type of the coordinates (e.g., float, double, int).
     * @param pos_pointToCheck The point to check for inclusion within the polygon perimeter.
     * @param pos_polygonPoints A vector of points representing the vertices of the polygon.
     *                          The polygon must have at least 3 points.
     * @param _get_range_min (Optional) Pointer to store the minimum bounding range of the polygon.
     * @param _get_range_max (Optional) Pointer to store the maximum bounding range of the polygon.
     * @return true If the point lies within the polygon perimeter.
     * @return false If the point lies outside the polygon perimeter.
     * 
     * @throws std::invalid_argument If the polygon does not have enough points to form a valid polygon.
     * 
     * @details
     * - The function first calculates the bounding box of the polygon and performs a quick
     *   check to see if the point lies within this bounding box.
     * - If the point is within the bounding box, the function uses a ray-casting algorithm
     *   to determine the number of times a ray extending from the point crosses the polygon's edges.
     * - If the number of crossings is odd, the point is inside the polygon; otherwise, it is outside.
     * - The function uses helper functions such as `getLineIntersect_2D` to calculate intersections.
     * 
     * @note
     * - The function assumes the polygon is defined in a 2D Cartesian coordinate system.
     * @note
     * - The function uses ImGui's drawing utilities for debugging purposes (e.g., visualizing the ray).
     */
    template<typename _varType>
    bool PosInPolygonPerimeter(
        pos2d<_varType> pos_pointToCheck,
        std::vector<pos2d<_varType>> pos_polygonPoints,
        pos2d<_varType>* _get_range_min=nullptr,
        pos2d<_varType>* _get_range_max=nullptr
    ) {
        if(pos_polygonPoints.size()<3) throw std::invalid_argument("ERROR: PosInPolygonPerimeter(pos2d, std::vector): pos_polygonPoints do not contain enough points to form a polygon: "+std::to_string(pos_polygonPoints.size()));

        pos2d<_varType> range_min(pos_polygonPoints.at(0));
        pos2d<_varType> range_max(pos_polygonPoints.at(0));

        /// Solve the absolute outer range of the polygon perimeter
        for(size_t i=0; i<pos_polygonPoints.size(); i++) {
            pos2d<_varType> &polyPointRef = pos_polygonPoints.at(i);
            if(polyPointRef.x < range_min.x) range_min.x = polyPointRef.x;
            if(polyPointRef.y < range_min.y) range_min.y = polyPointRef.y;
            if(polyPointRef.x > range_max.x) range_max.x = polyPointRef.x;
            if(polyPointRef.y > range_max.y) range_max.y = polyPointRef.y;
        }

        if(_get_range_min) *_get_range_min = range_min;
        if(_get_range_max) *_get_range_max = range_max;
        
        /// First basic inRegion check of the simple square.
        if(!pos_pointToCheck.inRegion(range_min, range_max)) {
            return false;
        }


        /// @brief Number of times that the pointToCheck has crossed a perimeter border.
        int perimeterTouchCount = 0;

        polySide<_varType> crossingPoint(pos_pointToCheck, pos2d<_varType>(range_max.x*1.1, pos_pointToCheck.y));


        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddLine(GUINC::toImVec2(crossingPoint.p0), GUINC::toImVec2(crossingPoint.p1), IM_COL32(100, 100, 200, 240));

        
        polySide<_varType> a_side(pos_polygonPoints.at(pos_polygonPoints.size()-1), pos_polygonPoints.at(0));
        if(getLineIntersect_2D(crossingPoint.p0, crossingPoint.p1, a_side.p0, a_side.p1, true, 3, 0.0001, false)!=pos2d<_varType>(-1, -1)) perimeterTouchCount++;
        for(size_t i=0; i<pos_polygonPoints.size()-1; i++) {
            a_side = polySide(pos_polygonPoints.at(i), pos_polygonPoints.at(i+1));
            pos2d<_varType> intersectPos = getLineIntersect_2D(crossingPoint.p0, crossingPoint.p1, a_side.p0, a_side.p1, true, 3, 0.0001, false);

            if(intersectPos!=pos2d<_varType>(-1, -1)) perimeterTouchCount++;
        }

        if(perimeterTouchCount%2==0) return false;
        return true;
    }

};


#endif //HPP_LIB_POLYREGIONCHECK