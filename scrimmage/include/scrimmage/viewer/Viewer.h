#ifndef VIEWER_H_
#define VIEWER_H_
/// ---------------------------------------------------------------------------
/// @file Viewer.h
/// @author Kevin DeMarco <kevin.demarco@gmail.com>
///
/// Time-stamp: <2016-10-12 11:33:48 syllogismrxs>
///
/// @version 1.0
/// Created: 24 Feb 2017
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
/// The Viewer class ...
/// 
/// ---------------------------------------------------------------------------

#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

#include <scrimmage/fwd_decl.h>

#include <thread>

namespace scrimmage {


    //class RenderWindowInteractor : public vtkRenderWindowInteractor {
    //public:
    //    static RenderWindowInteractor* New()
    //    {
    //        RenderWindowInteractor *cb = new RenderWindowInteractor;
    //        return cb;
    //    }
    //    vtkTypeMacro(RenderWindowInteractor, vtkRenderWindowInteractor);
    //};
    //vtkStandardNewMacro(RenderWindowInteractor);
    
    class Viewer {
    public:

        Viewer();
        
        void set_incoming_interface(InterfacePtr &incoming_interface);

        void set_outgoing_interface(InterfacePtr &outgoing_interface);
        
        void set_enable_network(bool enable);

        bool init();
        bool run();
        bool stop();

    protected:
        //Create a renderer, render window, and interactor
        vtkSmartPointer<vtkRenderer> renderer_;
        vtkSmartPointer<vtkRenderWindow> renderWindow_;
        vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor_;

        vtkSmartPointer<CameraInterface> cam_int_;
        InterfacePtr incoming_interface_;
        InterfacePtr outgoing_interface_;

        bool enable_network_;
        
        std::thread network_thread_;
    private:
    };

}
#endif
