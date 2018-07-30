/// ---------------------------------------------------------------------------
/// @section LICENSE
///  
/// Copyright (c) 2016 Georgia Tech Research Institute (GTRI) 
///               All Rights Reserved
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
/// @file filename.ext
/// @author Kevin DeMarco <kevin.demarco@gtri.gatech.edu> 
/// @author Eric Squires <eric.squires@gtri.gatech.edu>
/// @version 1.0
/// ---------------------------------------------------------------------------
/// @brief A brief description.
/// 
/// @section DESCRIPTION
/// A long description.
/// ---------------------------------------------------------------------------
#ifndef CAMERAINTERFACE_H_
#define CAMERAINTERFACE_H_
#include <vtkCommand.h>
#include <vtkSmartPointer.h>
#include <vtkProperty.h>
#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

#include <scrimmage/viewer/Updater.h>

namespace scrimmage {
// Define interaction style
class CameraInterface : public vtkInteractorStyleTrackballCamera
{
public:    
    
    CameraInterface();

    static CameraInterface* New();
    //vtkTypeMacro(CameraInterface, vtkInteractorStyleTrackballCamera);
    
    virtual void OnKeyPress();
    virtual void OnLeftButtonDown();
    virtual void OnLeftButtonUp();
    virtual void Rotate();

    virtual void Pan();
    virtual void Dolly();
        
    void set_updater(vtkSmartPointer<Updater> & updater)
    { updater_ = updater; }
            
protected:
    vtkSmartPointer<Updater> updater_;        
private:    

};
//vtkStandardNewMacro(CameraInterface);
}

#endif
