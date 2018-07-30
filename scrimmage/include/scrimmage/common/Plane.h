#ifndef SHAPE_PLANE_H_
#define SHAPE_PLANE_H_
/// ---------------------------------------------------------------------------
/// @file Plane.h
/// @author Kevin DeMarco <kevin.demarco@gmail.com>
///
/// Time-stamp: <2016-10-12 11:33:48 syllogismrxs>
///
/// @version 1.0
/// Created: 18 Jan 2017
///
/// ---------------------------------------------------------------------------
/// @section LICENSE
/// 
/// The MIT License (MIT)  
/// Copyright (c) 2012 Kevin DeMarco
///
/// Permission is hereby granted, free of charge, to any person obtaining a 
/// copy of this software and associated documentation files (the "Software"), 
/// to deal in the Software without restriction, including without limitation 
/// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
/// and/or sell copies of the Software, and to permit persons to whom the 
/// Software is furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in 
/// all copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
/// DEALINGS IN THE SOFTWARE.
/// ---------------------------------------------------------------------------
/// @section DESCRIPTION
/// 
/// The Plane class ...
/// 
/// ---------------------------------------------------------------------------
#include <scrimmage/common/Shape.h>

namespace scrimmage {

class Plane : public Shape {
public:
    Plane() : Shape()
    {
        center_ << 0,0,0;
        normal_ << 0,0,0;
    }    
    
    void set_center(Eigen::Vector3d & center) { center_ = center; }
    void set_normal(Eigen::Vector3d & normal) { normal_ = normal; }

    Eigen::Vector3d & center() { return center_; }
    Eigen::Vector3d & normal() { return normal_; }
    
protected:
    Eigen::Vector3d center_;
    Eigen::Vector3d normal_;
private:
};

typedef std::shared_ptr<Plane> PlanePtr;

} // namespace scrimmage

#endif
