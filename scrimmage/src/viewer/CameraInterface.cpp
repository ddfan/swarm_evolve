#include <iostream>

#include <scrimmage/viewer/CameraInterface.h>

using std::cout;
using std::endl;

namespace scrimmage {

    CameraInterface::CameraInterface()
    {        
    }

    CameraInterface *CameraInterface::New()
    {
        CameraInterface *cb = new CameraInterface;
        return cb;
    }
    
    void CameraInterface::OnKeyPress()
    {
        // Get the keypress
        vtkRenderWindowInteractor *rwi = this->Interactor;
        std::string key = rwi->GetKeySym();

        // Handle an arrow key
        if(key == "Right") {
            updater_->inc_follow();
        } else if(key == "Left") {
            updater_->dec_follow();
        } else if(key == "a") {
            updater_->next_mode();        
        } else if(key == "c") {
            updater_->request_cached();        
        } else if(key == "t") {
            updater_->toggle_trails();        
        } else if(key == "bracketleft") {
            updater_->dec_warp();
        } else if(key == "bracketright") {
            updater_->inc_warp();
        } else if(key == "b") {
            updater_->toggle_pause();
        } else if (key == "space") {
            updater_->single_step();
        } else if (key == "plus" || key == "equal") {
            updater_->inc_scale();
        } else if (key == "minus") {
            updater_->dec_scale();
        } else if (key == "0") {
            updater_->reset_scale();
        } else if (key == "r") {
            updater_->reset_view();
        } else if (key == "z") {
            updater_->inc_follow_offset();
        } else if (key == "Z") {
            updater_->dec_follow_offset();
        } else {
            //cout << "key: " << key << endl;
        }
        //cout << "Key: " << key << endl;

        // Forward events
        vtkInteractorStyleTrackballCamera::OnKeyPress();
    }
    
    void CameraInterface::Rotate()
    {
        updater_->update();
        vtkInteractorStyleTrackballCamera::Rotate();
    }

    void CameraInterface::OnLeftButtonDown()
    {
        vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }
    
    void CameraInterface::OnLeftButtonUp()
    {
        vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
    }

    void CameraInterface::Pan()
    {
        updater_->update();
        vtkInteractorStyleTrackballCamera::Pan();
    }

    void CameraInterface::Dolly()
    {
        updater_->update();
        vtkInteractorStyleTrackballCamera::Dolly();
    }
}
